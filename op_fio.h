#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unixcube/op_types.h>


_ul fget_size(char* fnam)
{
    _ul fsize;
    int fd = open(fnam, O_RDWR);

    if(fd){

// fd = fileno(f); //if you have a stream (e.g. from fopen), not a file descriptor.
        struct stat buf;
        fstat(fd, &buf);
        fsize = (int)buf.st_size;
        close(fd);
    } else {
        fsize = 0;
    }

    return((_ul)fsize);
}

_c* fget_data(char* fnam)
{
    _ul inbytes = 0;
    _ul fsize = fget_size(fnam);
    _c* inbuf=(_c*)malloc( (sizeof(_c) * fsize)+1);//was fsize+3

    int fd = open(fnam, O_RDWR );

    inbytes = read(fd, inbuf, fsize);
    inbuf[fsize]=0;//try to fix runlength io mismatching; causes cipher errors
    close(fd);
    return(inbuf);
}

_ul fput_data(char* fnam, _c* databuf)
{
    _ul obytes = 0x0;
    _ul fsize = strlen((_cc*)databuf);
    int fd = open(fnam, O_RDWR | O_TRUNC);

    obytes = write(fd, databuf, fsize);

        close(fd);

    return(obytes);
}


#ifdef __cplusplus
}
#endif