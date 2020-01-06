#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "argParser.h"
#include "thread.h"
#include "fileHandler.h"

int main(int argc, char **argv){

    CONF *conf = malloc(sizeof(CONF));
    pthread_t threads[MAX_THREAD];
    int8_t rc = 0;

    getArgs(conf, argc, argv);

    for(uint8_t i = 0; i < conf->timeCodeCount; i++){
        threadData *data = malloc(sizeof(threadData));
        data->conf = conf;
        data->timeID = i;
        rc = pthread_create(&threads[i], NULL, threadHandler, (void *)data);
        if(rc) puts("Thread could not be created!");
    }

    for(uint8_t i = 0; i < conf->timeCodeCount; i++){
        pthread_join(threads[i], NULL);
    }

    CONF_cleanup(conf);

    return 0;
}
