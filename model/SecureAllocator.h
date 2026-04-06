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
    constexpr SecureAllocator(const SecureAllocator<U>&) noexcept{} 

    // allocator
    T* allocate(std::size_t n){
        // check if n fit in std::size_t size
        if(n>std::size_t(-1)/sizeof(T)){
            throw std::bad_alloc();
        }

        void* raw = sodium_malloc(n*sizeof(T));
        if(!raw){
            throw std::bad_alloc();
        }

        // lock meme. from swapping into disk
        if(sodium_mlock(raw, n*sizeof(T)) != 0){
            sodium_free(raw);
            throw std::bad_alloc();
        }


        return static_cast<T*>(raw);
    }

    //deallocator
    void deallocate(T* p, std::size_t n)noexcept{

        if(p){
            sodium_munlock(p, n*sizeof(T)); // unlock for freeing mem.
            sodium_free(p);
        }
    }
};

// ownership // Define SecureAllocator as stateless: any instance can deallocate memory from another.
// alowing == -> // All SecureAllocator instances are interchangeable (can free each other's memory)
template< typename T, typename U>
bool operator==(const SecureAllocator<T>&, const SecureAllocator<U>&){return true;}

// allowing =! -> // SecureAllocators are never unequal 
template<typename T, typename U>
bool operator!=(const SecureAllocator<T>&, const SecureAllocator<U>&){return false;}

// Secure Containers as compare to String and Vector
using SecureVector = std::vector<uint8_t, SecureAllocator<uint8_t>>;
using SecureString = std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;
#endif // SECUREALLOCATOR_H
