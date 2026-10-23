#pragma once
#include <cstdlib>
#include <new>
namespace hero_alloc {inline thread_local bool watching=false;inline thread_local size_t calls=0;}
void* operator new(size_t n){if(hero_alloc::watching)++hero_alloc::calls;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,size_t)noexcept{std::free(p);}
void operator delete[](void* p,size_t)noexcept{std::free(p);}
