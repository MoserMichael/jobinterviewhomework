#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <cstddef>
#include <assert.h>


class File_mapping {

public:
    File_mapping()  :fd_(-1), file_size_(0), file_mapping_(nullptr) {} 
    ~File_mapping() { close(); }

    bool open(const char *file, size_t *file_size );
    bool close();

    size_t get_size() const { return file_size_; }
    uint8_t *get_bytes() const { return file_mapping_; }

private:
    bool get_file_size(const char *file);

    int fd_;
    size_t file_size_;
    uint8_t *file_mapping_;
};

class Buf_pos
{
public:
    Buf_pos(uint8_t *start, size_t len) : start_(start), end_(start + len ), at_( start ) { }


    Buf_pos( Buf_pos &from, size_t size ) {

        start_ = from.at_;
        end_ = from.at_ + size;
        at_ = from.at_;

        assert( from.at_ + size <= from.end_ );
    }

    bool valid() { 
        return start_ <= at_ && at_ < end_;
    }

    uint8_t *at() {  return at_; }


    uint8_t *get_rel( size_t idx ) {
        assert( start_ + idx < end_ );
        return start_ + idx;
    }

    bool eof() { 
        return at_ >= end_; 
    }

    bool inc( size_t len ) { 
        uint8_t *next = at_ + len; 
        if (next <= end_) { 
            at_ = next; 
            return true; 
        } 
        assert(0);
        return false; 
    }

    bool dec( size_t len ) { 
        uint8_t *next = at_ - len; 
        if (next >= start_) { 
            at_ = next; 
            return true; 
        }
        assert(0);
        return false; 
    }

    size_t pos() { 
        return at_ - start_; 
    }

    size_t remaining() { 
        return end_ - at_; 
    }

    uint8_t *start_, *end_, *at_;
};


class LogLevel
{
public:
    static LogLevel &get() { 
        static LogLevel level; 
        return level;
    }

    void set_level(int level) {
        level_ = level;
    }

    bool is_trace_on() {
        return level_ > 3; 
    }
    bool is_info_on() { 
        return level_ > 0; 
    }

private:
    LogLevel() : level_(0) {}

    int level_;
};


#define TRACE(format, ...) do { if (LogLevel::get().is_trace_on()) { fprintf (stderr, format, ##__VA_ARGS__); } } while(0);

#define INFO(format, ...) do { if (LogLevel::get().is_info_on()) { fprintf (stderr, format, ##__VA_ARGS__); } } while(0);


#endif

