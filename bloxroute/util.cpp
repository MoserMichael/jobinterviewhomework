#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.hpp"

bool File_mapping::open(const char *file, size_t *file_size )
{
    
    if (!get_file_size( file )) {
        return false;
    }

    int fd = ::open(file, O_RDONLY);
    if (fd == -1) {
        return false;
    }

    file_mapping_ = (uint8_t *) mmap( nullptr, file_size_, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);

    if (file_mapping_ == (void *) -1) {
        file_mapping_ = nullptr;
        fprintf(stderr, "mmap error errno=%d, fsize=%lu\n", errno, *file_size);
        return false;
    }

    *file_size = file_size_;

    return true;
}

bool File_mapping::close()
{
    /*
    if (file_mapping_ != nullptr) {
        munmap( file_mapping_, file_size_ );
    }
    */
    if (fd_ != -1) {
        ::close(fd_);
    }
}   

bool File_mapping::get_file_size(const char *file)
{
    struct stat sstat;

    if (stat(file, &sstat) == -1) {
        return false;
    }

    file_size_ = sstat.st_size;

    return true;
}

