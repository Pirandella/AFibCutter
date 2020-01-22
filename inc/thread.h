#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include "argParser.h"
#include "fileHandler.h"
#include "QRS.h"

#define MAX_THREAD  255

typedef struct{
    uint8_t timeID;
    CONF *conf;
}threadData;

void *threadHandler(void *data);

#endif // _THREAD_H_
