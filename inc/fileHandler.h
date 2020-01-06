#ifndef _FILE_HANDLER_H_
#define _FILE_HANDLER_H_

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "argParser.h"

#define H_OFFSET    11
#define M_OFFSET    14
#define S_OFFSET    17

#define SECONDS_IN_24H	86400

#define DIFFERECE(a, b) ((a > b) ? (a - b) : (b - a))

typedef struct{
    char s_hours[3];
    char s_minutes[3];
    char s_seconds[3];
    char e_hours[3];
    char e_minutes[3];
    char e_seconds[3];
}TIME;

TIME *getTimeIntervals(CONF *conf, uint8_t timeID);
TIME *getTime(const char *buffer);
void createFileName(const char *srcFile, char *dstFile, TIME *time);
void getOffset(TIME *timeIntervals, TIME *startTime, uint32_t *dataStartOffset, uint32_t *dataEndOffset, uint16_t freq);

#endif // _FILE_HANDLER_H_
