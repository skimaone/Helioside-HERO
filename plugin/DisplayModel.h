#pragma once
#include "TitanCore.h"
#include <juce_graphics/juce_graphics.h>

namespace hero_display {
// Display-only clean-cell probe. The axes remain input/output dBu: the amber
// trajectory rises over 500 ms and falls over 500 ms, revealing detector memory.
// It is a repeatable probe, not a measurement of the host's music or a new DSP path.
struct Curves {
    std::array<juce::Point<float>, 241> steady;
    std::array<juce::Point<float>, 481> timing;
};
inline Curves curves(double threshold, double attack, double release, double shape) {
    Curves result;
    titan::Parameters p;
    p.threshold=threshold;p.attack=attack;p.release=release;p.shape=shape;
    titan::Engine model;model.prepare(48000,1);model.setParameters(p);
    for(size_t i=0;i<result.steady.size();++i){const double dbu=-35.+i*.25;
        result.steady[i]={(float)dbu,(float)(dbu-model.staticReduction(dbu-22.))};}
    std::array<float,100> input{},output{};
    const float* in[]={input.data()};float* out[]={output.data()};
    input.fill((float)titan::dbToLin(-57.));
    for(int i=0;i<48;++i)model.process(in,out,input.size()); // 100 ms initial low level
    for(size_t i=0;i<result.timing.size();++i){
        const double dbu=-35.+.25*(i<=240?i:480-i);
        input.fill((float)titan::dbToLin(dbu-22.));model.process(in,out,input.size());
        result.timing[i]={(float)dbu,(float)(dbu-model.gainReductionDb())};
    }
    return result;
}

inline juce::Image glass(const juce::Image& content, bool powered) {
    using namespace juce;
    // Explicit area averaging preserves fine strokes when sampling the 100x80
    // matrix. All CPU-read images are software-backed, including on Direct2D.
    Image grid(Image::RGB,100,80,true,SoftwareImageType{});
    {const Image::BitmapData source(content,Image::BitmapData::readOnly);
    Image::BitmapData dest(grid,Image::BitmapData::writeOnly);
    const double scaleX=content.getWidth()/100.,scaleY=content.getHeight()/80.;
    for(int y=0;y<80;++y)for(int x=0;x<100;++x){
        const double x0=x*scaleX,x1=(x+1)*scaleX,y0=y*scaleY,y1=(y+1)*scaleY;
        double r=0,g=0,b=0,weight=0;
        for(int sy=(int)std::floor(y0);sy<jmin(content.getHeight(),(int)std::ceil(y1));++sy)
        for(int sx=(int)std::floor(x0);sx<jmin(content.getWidth(),(int)std::ceil(x1));++sx){
            const double w=(jmin(x1,double(sx+1))-jmax(x0,double(sx)))*(jmin(y1,double(sy+1))-jmax(y0,double(sy)));
            const auto c=source.getPixelColour(sx,sy);r+=w*c.getRed();g+=w*c.getGreen();b+=w*c.getBlue();weight+=w;}
        dest.setPixelColour(x,y,Colour::fromFloatRGBA(jlimit(0.f,1.f,float((r/weight/255.-.03)*1.25)),
            jlimit(0.f,1.f,float((g/weight/255.-.03)*1.25)),jlimit(0.f,1.f,float((b/weight/255.-.03)*1.25)),1.f));}}

    Image dots(Image::RGB,800,640,true,SoftwareImageType{});
    {const Image::BitmapData source(grid,Image::BitmapData::readOnly);
    Image::BitmapData dest(dots,Image::BitmapData::writeOnly);
    for(int y=0;y<640;++y)for(int x=0;x<800;++x){
        const int gx=x/8,gy=y/8;const auto c=source.getPixelColour(gx,gy);
        const float fx=(x%8+.5f)/8.f-.5f,fy=(y%8+.5f)/8.f-.5f;
        const float mask=jlimit(0.f,1.f,(.40f-std::sqrt(fx*fx+fy*fy))/.10f);
        const float activity=jlimit(0.f,1.f,3.f*jmax(c.getFloatRed(),c.getFloatGreen(),c.getFloatBlue()));
        // Fixed spatial variation: stable while controls move; never temporal noise.
        const auto hash=uint32(gx*374761393u+gy*668265263u+21u);
        const float variation=.91f+.18f*float((hash^(hash>>13))&1023u)/1023.f;
        const float light=powered?1.f:0.f;
        auto channel=[&](float v){return jlimit(0.f,1.f,.012f+mask*variation*(.08f*(1.f-activity*light)+v*1.15f*light));};
        dest.setPixelColour(x,y,Colour::fromFloatRGBA(channel(c.getFloatRed()),channel(c.getFloatGreen()),channel(c.getFloatBlue()),1.f));}}

    // Two small Gaussian bloom layers reproduce the supplied reference's
    // local light bleed and wider coloured halo. These caches never touch audio.
    Image small(Image::RGB,400,320,true,SoftwareImageType{});
    {Graphics g(small);g.setImageResamplingQuality(Graphics::highResamplingQuality);
        g.drawImage(dots,small.getBounds().toFloat());}
    Image nearGlow(Image::RGB,400,320,true,SoftwareImageType{}),farGlow(Image::RGB,400,320,true,SoftwareImageType{});
    ImageEffects::applyGaussianBlurEffect(1.f,small,nearGlow);
    ImageEffects::applyGaussianBlurEffect(3.f,small,farGlow);
    const auto nearFull=nearGlow.rescaled(800,640,Graphics::highResamplingQuality);
    const auto farFull=farGlow.rescaled(800,640,Graphics::highResamplingQuality);
    Image result(Image::RGB,800,640,true,SoftwareImageType{});
    {const Image::BitmapData base(dots,Image::BitmapData::readOnly),nearData(nearFull,Image::BitmapData::readOnly),farData(farFull,Image::BitmapData::readOnly);
    Image::BitmapData dest(result,Image::BitmapData::writeOnly);
    for(int y=0;y<640;++y)for(int x=0;x<800;++x){
        const float u=(x+.5f)/800.f,v=(y+.5f)/640.f;
        const float band=std::pow(jlimit(0.f,1.f,1.f-std::abs(u*.8f+v-.35f)/.30f),2.f);
        const float vignette=1.f-.25f*(std::pow((u-.5f)*1.8f,2.f)+std::pow((v-.5f)*1.8f,2.f));
        const auto h=uint32(x*2246822519u+y*3266489917u+11u);
        const float grain=(float((h^(h>>15))&255u)/255.f-.5f)*.008f;
        const auto a=base.getPixelColour(x,y),b=nearData.getPixelColour(x,y),c=farData.getPixelColour(x,y);
        const float glow=powered?1.f:.15f;
        auto channel=[&](float aa,float bb,float cc){return jlimit(0.f,1.f,(aa+glow*(.9f*bb+.5f*cc)+.06f*band)*vignette+grain);};
        dest.setPixelColour(x,y,Colour::fromFloatRGBA(channel(a.getFloatRed(),b.getFloatRed(),c.getFloatRed()),
            channel(a.getFloatGreen(),b.getFloatGreen(),c.getFloatGreen()),channel(a.getFloatBlue(),b.getFloatBlue(),c.getFloatBlue()),1.f));}}
    return result;
}
}
