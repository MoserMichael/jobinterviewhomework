#ifndef __CRYPTOUTIL_H_
#define __CRYPTOUTIL_H_

#include <openssl/sha.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <assert.h>

class SHA256_hash
{
public:

    static const uint32_t  DIGEST_LEN = SHA256_DIGEST_LENGTH; 

    SHA256_hash() {
        clear();
    }

    void clear() {
        memset( hash_, 0, SHA256_hash::DIGEST_LEN );
    }

    SHA256_hash( const uint8_t *hash ) {
        memcpy( hash_, hash, SHA256_hash::DIGEST_LEN );
    }


    uint8_t *hash( const uint8_t *start, size_t length) {
        SHA256_CTX context;
        
        SHA256_Init(&context);
        SHA256_Update( &context, start, length );
        SHA256_Final( hash_, &context);
    
        return hash_;
    }

    uint8_t *double_hash( const uint8_t *start, size_t length )
    { 
        uint8_t hash2[ DIGEST_LEN ];

        hash( start, length );

        memcpy( hash2, hash_, DIGEST_LEN );

        hash( hash2, DIGEST_LEN );

        return hash_;
    }

    bool operator==( const SHA256_hash &arg ) const  {
        for(int32_t i = 0; i < DIGEST_LEN; ++i ) {
            if (hash_[i] != arg.hash_[i]) {
                return false;
            }
        }
        return true;
    }


    const uint8_t *get_hash() const  {
        return hash_;
    }

private:
   uint8_t hash_[ DIGEST_LEN ];
};

struct SHA256_hash_hash
{
    std::size_t operator()(const SHA256_hash& k) const {
        size_t ret=0;
        for(int32_t i = 0; i < SHA256_hash::DIGEST_LEN; ++i ) {
            ret = (ret<<8) | k.get_hash() [i];
        }
        return ret;
    }
};


#endif

