#ifndef _ARG_PARSER_H_
#define _ARG_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define ARG_NUM             6

typedef struct{
    char **timeCode;
    uint8_t timeCodeCount;
    uint32_t offset;
    char *srcFile;
    char *dstFile;
    uint8_t fileFormat;
    int8_t channels;
}CONF;

void getArgs(CONF *conf, int argc, char **argv);

void CONF_cleanup(CONF *conf);

#endif // _ARG_PARSER_H_
