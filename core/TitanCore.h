#pragma once
// Experimental Titan-inspired model v1.0 research release; NOT a circuit-identical emulation.
// Single-header C++17 reference engine. No allocation on audio thread.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include "DetectorModels.h"
namespace titan {
constexpr double pi=3.141592653589793238462643;
inline double clamp(double x,double a,double b){return std::max(a,std::min(b,x));}
inline double dbToLin(double d){return std::pow(10.,d/20.);}
inline double linToDb(double v){return 20.*std::log10(std::max(v,1.e-12));}
inline double pole(double seconds,double fs){return std::exp(-1./(std::max(1.e-6,seconds)*fs));}
inline double smooth(double u){u=clamp(u,0.,1.);return u*u*(3.-2.*u);}
enum class FeedbackLaw { Legacy=0, Regularized=1, Equilibrium=2 };
struct Parameters {
 FeedbackLaw feedbackLaw=FeedbackLaw::Regularized; // numerical stabilization hypotheses, NOT established hardware topology
 double feedbackBlend=0.65; // equilibrium weight for Regularized law
 double feedbackGuardDb=1.5; // extra dB allowed above predicted static equilibrium
 double threshold=45,attack=30,release=30,shape=45; // uncalibrated 0..99
 double vcaColor=0,dynamicColor=0,parallelMix=15,makeupStep=0; // 0..15,0..23
 enum Tone{Fat=-1,Normal=0,Air=1} tone=Normal;
 bool bypass=false,stereoLink=true;
 double fatWeight=-1.,airWeight=-1.,linkAmount=-1.; // negative: use original discrete mode; plugin ramps these weights
 DetectorMode detectorMode=DetectorMode::Peak; // explicit candidate models, NOT established Titan algorithm
 double detectorHybridPeakWeight=0.65; // provisional 0..1
 double detectorRiseSeconds=0.00025,detectorFallSeconds=0.004;
 double referenceDbfs=-18.; // dBFS corresponding to +4 dBu, adjustable
 // Research knobs (not hardware controls): expose only in developer calibration panel.
 double vintageDrive=2.1; // 1..5
 double vintageStrength=0.78; // 0..1; fades in with GR
 double colorStrength=0.35; // 0..1, GR-dependent waveshaping
 double colorRiseWeight=0.0; // 0..1; experimental GR derivative coupling
 double detectorToneDepth=0.65; // sidechain EQ depth, experimental
 double audioToneDepth=0.20; // strength scaling for measured GR-dependent spectral correction
 double fatReferenceBoostDb=1.1; // musical excerpt estimate, NOT fixed hardware EQ
 double airReferenceBoostDb=0.55; // musical excerpt estimate, NOT fixed hardware EQ
 double diodeAsymmetry=0.04; // small even-harmonic term, not measured
};
class OnePole {double state_=0.,a_=0.;public:
 void prepare(double fs,double hz){a_=std::exp(-2.*pi*clamp(hz,1.,.45*fs)/fs);}
 double tick(double x){state_+=(1.-a_)*(x-state_);return state_;}
 void reset(){state_=0.;}
};
class Engine {
 struct Channel {
  double gr=0.,det=0.,lastWetControl=0.,prevGr=0.,envelope=0.;
  DetectorResponse detector, inputDetector;
  OnePole fat,air,scLow,scHigh,inputScLow,inputScHigh;
  void reset(){gr=det=lastWetControl=prevGr=envelope=0.;detector.reset();inputDetector.reset();fat.reset();air.reset();scLow.reset();scHigh.reset();inputScLow.reset();inputScHigh.reset();}
 };
 std::array<Channel,2> c_{};
 double fs_=48000., maxGR_=0.;int n_=2;Parameters p_{};
 double k_=1.,width_=1.,threshold_=-20.,ka_=0.,kr_=0.,mix_=1.,makeup_=1.;
 double fatWeight_=0.,airWeight_=0.,link_=1.;
 void coefficients(){
  const double s=p_.shape/99.;k_=8.9*std::pow(1.-s,1.18)+.1;
  width_=1.5+23.5*s;threshold_=p_.referenceDbfs-22.+40.*p_.threshold/99.;
  ka_=pole(.00005*std::pow(8000.,p_.attack/99.),fs_);
  kr_=pole(.05*std::pow(120.,p_.release/99.),fs_);
  mix_=.06+.94*p_.parallelMix/15.;makeup_=dbToLin(.5*p_.makeupStep);
  fatWeight_=p_.fatWeight<0.?(p_.tone==Parameters::Fat?1.:0.):clamp(p_.fatWeight,0.,1.);
  airWeight_=p_.airWeight<0.?(p_.tone==Parameters::Air?1.:0.):clamp(p_.airWeight,0.,1.);
  link_=p_.linkAmount<0.?(p_.stereoLink?1.:0.):clamp(p_.linkAmount,0.,1.);
 }
 double reduction(double outputDb) const {
  // Feedback-law attenuation: for hard knee G=k*(Y-T),
  // Y=X-G => closed-loop ratio R=1+k, NOT 1/(1-k).
  // Shape 0 is empirically stronger compression than Shape 99 (Amazona).
  const double over=outputDb-threshold_;
  if(over<=-width_/2.)return 0.;
  if(over>=width_/2.)return std::max(0.,k_*over);
  return std::max(0.,k_*(over+width_/2.)*(over+width_/2.)/(2.*width_));
 }
 double equilibriumGR(double estimatedInputDb) const {
  // Analytic solution of the SAME monotonic feedback equilibrium. Preserve
  // the reference 16-step bisection midpoint grid (50/65536 dB) exactly.
  const double over=estimatedInputDb-threshold_;
  double g=0.;
  if(over>=width_*(1.+k_)*.5)g=k_*over/(1.+k_);
  else if(over>-width_*.5){const double q=over+width_*.5,a=k_/(2.*width_);
   const double u=2.*q/(1.+std::sqrt(1.+4.*a*q));g=a*u*u;}
  constexpr double quantum=50./65536.;
  return (clamp(std::ceil(g/quantum)-1.,0.,65535.)+.5)*quantum;
 }
 double detectorSample(double x, Channel& c, bool inputPath){
  // Spectral detector weighting hypothesis: Fat reduces bass trigger,
  // Air reduces treble trigger, hence those bands survive compression.
  const double lo=inputPath?c.inputScLow.tick(x):c.scLow.tick(x);
  const double high=x-(inputPath?c.inputScHigh.tick(x):c.scHigh.tick(x));
  const double depth=clamp(p_.detectorToneDepth,0.,0.95);
  return x-depth*(fatWeight_*lo+airWeight_*high);
 }
 double wetSample(double x,Channel& c){
  const double attenuation=dbToLin(-c.gr),u=x*attenuation;
  // Both colour laws act inside the gain cell, at the incoming signal's
  // working level, then share its attenuation. Shaping only the already
  // reduced sample suppressed harmonic contrast as GR increased.
  // Inferred activation: resolve light compression without a hard onset.
  // The preserved equilibrium solver has a sub-millidecibel midpoint floor.
  // Exclude one grid quantum from colour activation so numerical floor alone
  // cannot switch either colour path on below compression onset.
  const double activeGr=std::max(0.,c.gr-50./65536.);
  const double activity=-std::expm1(-activeGr/3.);
  // Diode proxy: normalized, gain-preserving small-signal approximation.
  // Attenuation-dependent, odd soft-curvature with opposing third-harmonic sign.
  const double m=p_.vcaColor/15.;
  const double drive=clamp(p_.vintageDrive,1.,5.);
  const double intensity=clamp(p_.vintageStrength,0.,1.)*activity;
  const double norm=drive; // tanh(drive*u)/drive has unity small-signal slope
  const double bent=std::tanh(drive*x)/norm;
  const double vintage=x+intensity*(bent-x)+p_.diodeAsymmetry*intensity*x*x/(1.+x*x);
  double cell=(1.-m)*x+m*vintage;
  // Sign of cubic opposite the soft-saturating diode proxy; no color at zero GR.
  // x^3 coefficient >0 creates anti-saturating (peak-expanding) curvature.
  const double color=p_.dynamicColor/15.;
  const double delta=std::max(0.,c.gr-c.prevGr);
  const double edge=smooth(delta*30.);
  const double activation=activity*((1.-clamp(p_.colorRiseWeight,0.,1.))+
                          clamp(p_.colorRiseWeight,0.,1.)*edge);
  // bounded rational cubic: smooth peaks, avoids runaway at high input.
  const double z=clamp(cell,-4.,4.);
  const double nonlinear=z*z*z/(1.+z*z);
  cell+=clamp(p_.colorStrength,0.,1.)*color*activation*nonlinear;
  double y=cell*attenuation;
  // GR-dependent approximate shelves; exact sidechain implementation unknown.
  const double toneActive=smooth(c.gr/12.)*clamp(p_.audioToneDepth/0.20,0.,5.);
  const double lo=c.fat.tick(y),hiBase=c.air.tick(y);
  y+=fatWeight_*(dbToLin(p_.fatReferenceBoostDb*toneActive)-1.)*lo;
  y+=airWeight_*(dbToLin(p_.airReferenceBoostDb*toneActive)-1.)*(y-hiBase);
  // Makeup applies only to compressed path, before manual's parallel mixing.
  y*=makeup_;
  return y;
 }
public:
 void prepare(double fs,int channels){fs_=std::max(fs,8000.);n_=std::clamp(channels,1,2);
  for(auto&c:c_){c.fat.prepare(fs_,65.);c.air.prepare(fs_,3900.);c.scLow.prepare(fs_,130.);c.scHigh.prepare(fs_,4600.);c.inputScLow.prepare(fs_,130.);c.inputScHigh.prepare(fs_,4600.);c.reset();}maxGR_=0.;coefficients();}
 void reset(){for(auto&c:c_)c.reset();maxGR_=0.;}
 void setParameters(Parameters p){p.threshold=clamp(p.threshold,0.,99.);p.attack=clamp(p.attack,0.,99.);
  p.release=clamp(p.release,0.,99.);p.shape=clamp(p.shape,0.,99.);
  p.vcaColor=clamp(p.vcaColor,0.,15.);p.dynamicColor=clamp(p.dynamicColor,0.,15.);
  p.parallelMix=clamp(p.parallelMix,0.,15.);p.makeupStep=clamp(p.makeupStep,0.,23.);
  p.vintageDrive=clamp(p.vintageDrive,1.,5.);p.vintageStrength=clamp(p.vintageStrength,0.,1.);
  p.colorStrength=clamp(p.colorStrength,0.,1.);p.colorRiseWeight=clamp(p.colorRiseWeight,0.,1.);
  p.detectorToneDepth=clamp(p.detectorToneDepth,0.,0.95);p.audioToneDepth=clamp(p.audioToneDepth,0.,1.);
  p.diodeAsymmetry=clamp(p.diodeAsymmetry,-0.2,0.2);
  p.fatReferenceBoostDb=clamp(p.fatReferenceBoostDb,0.,6.);p.airReferenceBoostDb=clamp(p.airReferenceBoostDb,0.,6.);
  p.referenceDbfs=clamp(p.referenceDbfs,-36.,0.);
  p.feedbackBlend=clamp(p.feedbackBlend,0.,1.);
  p.feedbackGuardDb=clamp(p.feedbackGuardDb,0.,12.);
  p.detectorHybridPeakWeight=clamp(p.detectorHybridPeakWeight,0.,1.);
  p.detectorRiseSeconds=clamp(p.detectorRiseSeconds,0.00001,0.1);
  p.detectorFallSeconds=clamp(p.detectorFallSeconds,0.0001,1.0);
  p_=p;coefficients();}
 const Parameters& parameters()const{return p_;}
 double gainReductionDb()const{return maxGR_;}
 double staticReduction(double inputDb)const{return equilibriumGR(inputDb);}
 void process(const float*const* in,float*const* out,std::size_t frames){
  const double ka=ka_,kr=kr_;
  // Separate rectifier tracking from controlled GR ballistics; provisional.
  
  const double mix=mix_;
  for(std::size_t i=0;i<frames;++i){
   double x[2]={in[0][i], n_==2?in[1][i]:0.};
   // True bypass must pass samples bit-identically and not drive the detector.
   if(p_.bypass){out[0][i]=in[0][i];if(n_==2)out[1][i]=in[1][i];continue;}
   double levels[2]={0.,0.},inputLevels[2]={0.,0.};
   for(int j=0;j<n_;++j){auto&c=c_[j];
    const double signal=detectorSample(c.lastWetControl,c,false);
    c.det=c.detector.tick(signal,fs_,p_.detectorRiseSeconds,p_.detectorFallSeconds,
                           p_.detectorMode,p_.detectorHybridPeakWeight);
    levels[j]=c.det;
    inputLevels[j]=c.inputDetector.tick(detectorSample(x[j],c,true),fs_,p_.detectorRiseSeconds,p_.detectorFallSeconds,p_.detectorMode,p_.detectorHybridPeakWeight);
   }
   // Shared detector analogue-control approximation; avoids left/right gain mismatch.
   const double linked=n_==2?std::max(levels[0],levels[1]):levels[0];
   const double inputLinked=n_==2?std::max(inputLevels[0],inputLevels[1]):inputLevels[0];
   maxGR_=0.;
   for(int j=0;j<n_;++j){auto&c=c_[j];c.prevGr=c.gr;
    const double levelDb=linToDb(levels[j]+link_*(linked-levels[j]));
    const double legacyWant=reduction(levelDb);
    // Recover a quasi-static input level from post-VCA detector plus previous GR.
    // Assumes slowly varying gain and neglects nonlinear coloration.
    const double predicted=equilibriumGR(linToDb(inputLevels[j]+link_*(inputLinked-inputLevels[j])));
    double want=legacyWant;
    if(p_.feedbackLaw==FeedbackLaw::Regularized)
     want=(1.-p_.feedbackBlend)*legacyWant+p_.feedbackBlend*predicted;
    else if(p_.feedbackLaw==FeedbackLaw::Equilibrium)want=predicted;
    const double k=want>c.gr?ka:kr;
    c.gr=k*c.gr+(1.-k)*want;
    if(p_.feedbackLaw!=FeedbackLaw::Legacy)
      c.gr=std::min(c.gr,predicted+p_.feedbackGuardDb);
    c.gr=clamp(c.gr,0.,50.);
    maxGR_=std::max(maxGR_,c.gr);
    const double wet=wetSample(x[j],c);
    // Feedback control tap is PRE-makeup/PARALLEL and includes gain-cell color.
    // Provisional: wet sample divided by makeup gain; tones may influence detector.
    c.lastWetControl=wet/makeup_;
    double result=(1.-mix)*x[j]+mix*wet;
    out[j][i]=static_cast<float>(std::isfinite(result)?clamp(result,-100.,100.):0.);
   }
  }
 }
};
}
