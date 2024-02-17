#include "bt.hpp"
#include "util.hpp"
#include <unistd.h>

#define BLOCK_FILE "sample_block_bin"


void print_help(const char *name) {
    fprintf(stderr, "%s [-h|-f <file_name> [-l | -s <hash>]]\n"
                    "\n"
                    " -h        print help message\n"
                    " -f <file> parse file with bitcoin protocol messages\n"
                    " -l        list all hunded longest transactions\n"
                    " -s <hash> lookup entry by hash\n"
                    " -t        print traces\n", name);
    exit(1);
} 

void show_hash(const uint8_t *nums)
{
    for(int i = 0; i < SHA256_hash::DIGEST_LEN; ++i ) {
        fprintf(stderr,"%02x", nums[i] );
    }

}

void show_entry(const Tx_data *it) {

    fprintf(stderr, "block_index=%lu tx_index=%lu start_pos=%lu end_pos=%lu length=%lu hash=", 
        it->block_index_, it->tx_index_, it->start_pos_, it->end_pos_, it->length());  

    const uint8_t *nums = it->hash_.get_hash();
    show_hash(nums);
    
    fprintf(stderr,"\n");

}


void parse_string( uint8_t* hash, std::string arg_hash) {
    memset( hash, 0, SHA256_hash::DIGEST_LEN ); 
    for(size_t i = 0; i < arg_hash.size(); i += 2) {
        uint32_t byby = 0;

        std::string letters = arg_hash.substr(i, 2);
        sscanf(letters.c_str(), "%x", &byby);

        hash[ i/2 ] = (uint8_t) byby;
    }
}


int main(int argc, char *argv[])
{
   File_mapping map;
   size_t file_size;
   std::string bf;
   bool list_longest = false;
   bool find_by_hash = false;
   std::string arg_hash;
   int c;

   while(( c = getopt(argc, argv, "htls:f:" )) != -1) {
     switch( c ) {
        case 'f':
            bf = optarg;
            break;
        case 't':   
            LogLevel::get().set_level(100);
            break;
        case 'l':
            list_longest = true;
            break;
        case 's':
            find_by_hash = true;
            arg_hash = optarg;
            break;
        default:
        case 'h':
            print_help( argv[0] );
            break;
      }
   }

   TRACE( "input file %s\n", bf.c_str());

   if (!map.open( bf.c_str(), &file_size )) {
        TRACE("can't open file %s\n", bf.c_str());
        exit(1);
    }
    TRACE( "opened file ptr=%p size=%lu\n", map.get_bytes(), map.get_size() );
    
    Buf_pos pos( map.get_bytes(), map.get_size() );

    BtData   data;

    BtParser parser(&data);

    parser.parse_file( pos );

    data.init();
 
    TRACE( "Totals: num_messages=%lu\n", parser.get_num_messages() ); 

    if (list_longest) {
        for( auto it = data.get_tx_data_last(); it != data.get_tx_data_end(); ++it) {
            show_entry(&(*it));
        }

        #if 0
        fprintf(stderr, "dump key to data\n");
        for(auto e : data.get_map_key_to_data()) {
            fprintf(stderr,"---\n");
            show_hash(e.first.get_hash());
            fprintf(stderr, " ");
            show_entry(e.second);

        }
        #endif
     }

    if (find_by_hash) {
        fprintf(stderr, "lookup of hash=%s\n", arg_hash.c_str());

        uint8_t hash[ SHA256_hash::DIGEST_LEN ];
 
        parse_string( hash, arg_hash);

        fprintf(stderr, "lookup of hash=");
        show_hash(hash);
        fprintf(stderr,"\n");

        SHA256_hash key(hash);
        auto it = data.get_map_key_to_data().find( key );
        if (it != data.get_map_key_to_data().end()) {
            fprintf(stderr,"\nfound entry!\n");
            show_entry( it->second );
        } else {
            fprintf(stderr, "entry not found!\n");
        }

   }

    return 0;
}
