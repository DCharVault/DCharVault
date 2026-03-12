#ifndef SECUREALLOCATOR_H
#define SECUREALLOCATOR_H

#include<sodium.h>
#include<cstddef>
#include<new>
#include<vector>
#include<string>
#include<cstdint>

template <typename T>
struct SecureAllocator{
    using value_type = T;
    
    SecureAllocator() = default;

    template <typename U> // for shapte-shifter 
    //container will try to clone allocator and change its data type from T to U.
    constexpr SecureAllocator(const SecureAllocator<U>&) noexcept{} // a hole tube like constructor

    // allocator
    T* allocate(std::size_t n){
        // check if n fit in std::size_t size
        if(n>std::size_t(-1)/sizeof(T)){
            throw std::bad_alloc();
        }

        // sodium_malloc allocates memory and places guard pages around it
        // to instantly crash app if buffer overflow attack occurs
        
        // allocate size n*sizeof(T) from sodium malloc -> it return void* ptr if success else allocation failed it return nullptr
        // static cast <T*> = converts void* to T*
        // p here is a ptr to raw allocated memory large enough to hold n objects of type T
        if(auto p = static_cast<T*>(sodium_malloc(n*sizeof(T)))){
            return p;
        }
        throw std::bad_alloc();
    }

    //deallocator
    void deallocate(T* p, std::size_t n)noexcept{
        sodium_free(p);
    }
};

// ownership // Define SecureAllocator as stateless: any instance can deallocate memory from another.
// alowing == -> // All SecureAllocator instances are interchangeable (can free each other's memory)
template< typename T, typename U>
bool operator==(const SecureAllocator<T>&, const SecureAllocator<U>&){return true;}

// allowing =! -> // SecureAllocators are never unequal 
template<typename T, typename U>
bool operator!=(const SecureAllocator<T>&, const SecureAllocator<U>&){return false;}

using SecureVector = std::vector<uint8_t, SecureAllocator<uint8_t>>;
using SecureString = std::basic_string<char,std::char_traits<char>;
SecureAllocator<char>>;

#endif // SECUREALLOCATOR_H
