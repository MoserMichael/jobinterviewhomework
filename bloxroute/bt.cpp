#include "util.hpp"
#include "cryptoutil.hpp"
#include "bt.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <map>
#include <algorithm>

#pragma pack(push, 1)
struct BtMessage {
    uint32_t magic_;
    uint8_t  command_[12];
    uint32_t length_;
    uint32_t checksum_;

    void convert() {
        //length_ = ntohl( length_ );
        magic_ = ntohl(magic_);
        //checksum_ = ntohl(checksum_);

    }
};

struct BtBlockHdr {
    int32_t version_;
    uint8_t prev_block_[32];
    uint8_t merkle_root_[32];
    uint32_t timestamp_;
    uint32_t bits_;
    uint32_t nonce_;
};

#pragma pack(pop)

void BtData::init()
{
#if 0
     std::nth_element( tx_data_.begin(), tx_data_.begin() + tx_data_.size() - LAST_N, tx_data_.end() );
     std::sort( tx_data_.begin() + tx_data_.size() - LAST_N, tx_data_.begin() + tx_data_.size() );
#else     
     std::sort( tx_data_.begin(), tx_data_.begin() + tx_data_.size() );
#endif

     if (tx_data_.size() > LAST_N) {
   
        TRACE("Insert last elements to map\n");
        for(auto it = tx_data_.begin() + tx_data_.size() - LAST_N; it != tx_data_.end(); ++it)
        {
            //auto res = map_key_to_data_.insert(  Map_key_to_data::value_type( it->hash_, &(*it) ) );
            //assert( res.second );
            //
            map_key_to_data_[ it->hash_ ] = &(*it);
        }
    }
    else {
        TRACE("Insert all elements to map\n");

        for(auto &elm : tx_data_) {
            auto res = map_key_to_data_.insert(  Map_key_to_data::value_type( elm.hash_, &elm ) );
            assert( res.second );
        }
    }

    #if 0
    for(auto it= get_tx_data_last(); it != get_tx_data_end(); ++it) {
       assert( map_key_to_data_.find( it->hash_ ) != map_key_to_data_.end() );
    }
    #endif
}



bool BtParser::parse_file( Buf_pos &pos )
{
    bool first_msg = true;
    TRACE("parsing file...\n");

    while( !pos.eof() )  {
        TRACE( "at=0x%p pos=%lu remaining=%lu\n", pos.at(), pos.pos(), pos.remaining());
        
        assert( pos.remaining() >= sizeof(BtMessage) );
        BtMessage msg =  * (BtMessage *) pos.at();

        if (first_msg) {
            network_magic_ = msg.magic_;
            first_msg = false;
        } else {
            while( !pos.eof() && msg.magic_ != network_magic_ ) {
                //TRACE("skipping till known network magic\n");
                pos.inc( 1 );
                assert( pos.remaining() >= sizeof(BtMessage) );
                msg = * (BtMessage *) pos.at();
            }
            TRACE( "found network after skip to: at=0x%p pos=%lu remaining=%lu\n", pos.at(), pos.pos(), pos.remaining()); 

         }

        msg.convert();
        TRACE("Message magic=0x%x command=%.12s length=%u checksum=0x%x (struct-size=%lu)\n", msg.magic_, msg.command_, msg.length_, msg.checksum_, sizeof(BtMessage));

        Buf_pos msg_payload = pos;
        msg_payload.inc( sizeof(BtMessage) );

        Buf_pos block_pos( msg_payload, msg.length_ );

        parse_block( block_pos );
        ++num_messages_;
        
        pos.inc( sizeof(BtMessage) + msg.length_ );
    }

    TRACE( "parsing file finished\n");


    return true;
}

bool BtParser::parse_block( Buf_pos block_pos )
{
    uint64_t tx_count;

    assert( block_pos.remaining() >= sizeof(BtBlockHdr) );
    BtBlockHdr msg =  * (BtBlockHdr *) block_pos.at();

    block_pos.inc( sizeof(BtBlockHdr) );

    parse_num_inc( block_pos, tx_count );
    
    Buf_pos tx_pos( block_pos, block_pos.remaining() );
    
    TRACE( "block-data verions=%d tx_count=%lu at=0x%p pos=%lu remaining=%lu\n", msg.version_, tx_count, block_pos.at(), block_pos.pos(), block_pos.remaining());

    parse_transactions( tx_pos, tx_count );

    return true;
}

bool BtParser::parse_transactions( Buf_pos tx_pos, uint64_t tx_count ) {

    for(uint64_t i=0; i < tx_count; ++i) {
        TRACE( "\ttxn #num=%lu at=0x%p pos=%lu remaining=%lu\n", i, tx_pos.at(), tx_pos.pos(), tx_pos.remaining());
        parse_transaction(tx_pos, i);
    }
}

bool BtParser::parse_transaction( Buf_pos &tx_pos, uint64_t tx_index ) {

    uint32_t version;
    uint16_t flags;
    bool has_flags;
    uint64_t tx_in_count;
    uint64_t tx_out_count;
    uint32_t lock_time;


    uint64_t start_pos, end_pos;


    start_pos = tx_pos.pos();
      
    get_inc( tx_pos, version );

    get_at( tx_pos, flags );
    flags = ntohs(flags);

    has_flags = flags == 1;
    tx_pos.inc( has_flags ? sizeof(uint16_t) : 0 ); 

    parse_num_inc( tx_pos, tx_in_count );
    TRACE("\ttx-in-count=%lu\n", tx_in_count);
    for( uint64_t i = 0; i < tx_in_count; ++i) {
        parse_tx_in( tx_pos );
    }
   
    parse_num_inc( tx_pos, tx_out_count );
    TRACE("\ttx-out-count=%lu\n", tx_out_count);
    for( uint64_t i = 0; i < tx_out_count; ++i ) {
        parse_tx_out( tx_pos );
    }

    if (has_flags) {
        parse_witness( tx_pos );
    }
    
    get_inc( tx_pos, lock_time );

    end_pos = tx_pos.pos();
 
    TRACE( "\ttxn: version=%u flags=%hu tx_in_count=%lu tx_out_count=%lu lock_time=%x\n", version, flags, tx_in_count, tx_out_count, lock_time);
    

    SHA256_hash hash;

    hash.double_hash( tx_pos.get_rel(start_pos), end_pos - start_pos );
    
    bt_data_->tx_data_.emplace_back( Tx_data( num_messages_, tx_index, start_pos, end_pos, hash.get_hash() ) );
    
    
    return true;
}

bool BtParser::parse_witness( Buf_pos &tx_pos )
{
    // https://bitcoin.stackexchange.com/questions/68924/complete-definition-of-tx-witness-data-structure-used-in-tx-data-structure

    uint64_t wit_count;

    TRACE( "\t\twitness at=0x%p pos=%lu remaining=%lu\n", tx_pos.at(), tx_pos.pos(), tx_pos.remaining());
 
    parse_num_inc( tx_pos, wit_count );

    for(uint64_t i=0; i < wit_count; ++i) {
        uint64_t comp_len;

        parse_num_inc( tx_pos, comp_len );
        tx_pos.inc( comp_len );
    }
}

bool BtParser::parse_tx_in( Buf_pos &tx_pos ) {
 
    uint64_t script_length;
    uint32_t sequence;

    TRACE( "\t\ttxn-in at=0x%p pos=%lu remaining=%lu\n", tx_pos.at(), tx_pos.pos(), tx_pos.remaining());

    tx_pos.inc( 36 ); // previous output

    parse_num_inc( tx_pos, script_length ); // length of sign script

    tx_pos.inc( script_length ); // sign script.

    get_inc( tx_pos, sequence ); // transaction version

    return true;
}

bool BtParser::parse_tx_out( Buf_pos &tx_pos ) {
    uint64_t tvalue;
    uint64_t pk_script_len;

    TRACE( "\t\ttxn-out at=0x%p pos=%lu remaining=%lu\n", tx_pos.at(), tx_pos.pos(), tx_pos.remaining());
    
    get_inc( tx_pos, tvalue );
    
    parse_num_inc( tx_pos, pk_script_len );

    tx_pos.inc( pk_script_len );

    return true;
}



bool BtParser::parse_num_inc( Buf_pos &pos, uint64_t &value )
{
    size_t size = parse_num( pos, value );

    return pos.inc( size );
}

size_t BtParser::parse_num( Buf_pos pos, uint64_t &value )
{
    assert( pos.remaining() >= sizeof(uint8_t) );

    uint8_t *val_byte = pos.at_;
    if (*val_byte < 0xFD) {
        value = *val_byte;
        
        return sizeof(uint8_t);
    }

    if (*val_byte == 0xFD) {
        assert(pos.remaining() >= sizeof(uint16_t) + sizeof(uint8_t));
        
        uint16_t *val_short =  (uint16_t *) (pos.at_ + 1);
        value = *val_short;
        
        return sizeof(uint16_t) + sizeof(uint8_t);
    }

    if (*val_byte == 0xFE) {
        assert(pos.remaining() >= sizeof(uint32_t) + sizeof(uint8_t));
        
        uint32_t *val_int = (uint32_t *) (pos.at_ + 1);
        value = *val_int;
 
        return sizeof(uint32_t) + sizeof(uint8_t);
    }

    if (*val_byte == 0xFF) {
        assert(pos.remaining() >= sizeof(uint64_t) + sizeof(uint8_t));
        
        uint64_t *val_int = (uint64_t *) (pos.at_ + 1);
        value = *val_int;
 
        return sizeof(uint64_t) + sizeof(uint8_t);
    }

    assert(0);
    return (size_t) -1;
}



