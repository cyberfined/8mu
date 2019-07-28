#include "load.h"

char* load_prog(char *filename) {
    int fd = open(filename, O_RDONLY);
    if(fd < 0) return strerror(errno);

    struct stat buf;
    if(fstat(fd, &buf) < 0) return strerror(errno);

    if(buf.st_size > MEM_SIZE - 0x200) return "Too big file size";

    if(read(fd, memory + 0x200, buf.st_size) < 0) return strerror(errno);

    close(fd);
    return NULL;
}
