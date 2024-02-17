#ifndef __BT_H__
#define __BT_H__

#include <stdint.h>
#include <unordered_map>
#include "cryptoutil.hpp"
#include <algorithm>
#include <vector>
#include "util.hpp"

struct Tx_data {
    
    Tx_data( uint64_t block_index, uint64_t tx_index, uint64_t spos, uint64_t epos, const uint8_t *hash)
        : block_index_(block_index)
        , tx_index_(tx_index)
        , start_pos_(spos)
        , end_pos_(epos)
        , hash_( hash )
    {
    }

    bool operator<( const Tx_data &arg ) const {
        return length() < arg.length();
    }   


    bool operator==( const Tx_data &arg ) const {
        return length() == arg.length();
    }   

    uint64_t length() const { return end_pos_ - start_pos_; }

    uint64_t block_index_;
    uint64_t tx_index_;
    uint64_t start_pos_, end_pos_;
    SHA256_hash hash_;
};


class BtData {
friend class BtParser;
public:

    using Map_key_to_data = std::unordered_map< SHA256_hash, Tx_data *, SHA256_hash_hash>; 
    
    static const uint32_t LAST_N = 100;

    //using Hash_key = uint8_t[ SHA256_hash::DIGEST_LEN ];

    void init();
 
    std::vector<Tx_data>::iterator  get_tx_data_last() { 
        if (tx_data_.size() <= LAST_N) {
            return tx_data_.begin();
        }
        std::vector<Tx_data>::iterator it = tx_data_.begin();
        it += tx_data_.size();
        it -= LAST_N;
        return it;
    }


    std::vector<Tx_data>::iterator  get_tx_data_end() { 
        return tx_data_.end();
    }

    Map_key_to_data &get_map_key_to_data(){ 
        return map_key_to_data_; 
    }


private:

    Map_key_to_data map_key_to_data_;
    std::vector<Tx_data> tx_data_;
};

class BtParser {
public:


    BtParser(BtData *data) 
        : num_messages_(0), bt_data_(data) 
    {
    }

    bool parse_file( Buf_pos &pos );

    uint64_t get_num_messages() const { return num_messages_; }

private:
    bool parse_block( Buf_pos block_pos );
    bool parse_transactions( Buf_pos tx_pos, uint64_t tx_count );
    bool parse_transaction( Buf_pos &tx_pos, uint64_t tx_index );
    bool parse_tx_in( Buf_pos &tx_pos );
    bool parse_tx_out( Buf_pos &tx_pos );
    bool parse_witness( Buf_pos &tx_pos );
     
    bool parse_num_inc( Buf_pos &pos, uint64_t &value );
    size_t parse_num( Buf_pos pos, uint64_t &value );


    template<typename T>
    bool get_inc( Buf_pos &pos, T &value) {
        assert( pos.remaining() >= sizeof(T) );

        value = * (T *) pos.at();
        pos.inc( sizeof(T) );

        return pos.valid();
    }

    template<typename T>
    bool get_at( Buf_pos &pos, T &value) {
        assert( pos.remaining() >= sizeof(T) );

        value = * (T *) pos.at();

        return pos.valid();
    }

    uint32_t network_magic_;
    uint64_t num_messages_;
    BtData *bt_data_;
};


#endif

