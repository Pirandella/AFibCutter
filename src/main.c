#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#include "argParser.h"
#include "thread.h"
#include "fileHandler.h"

int main(int argc, char **argv){

    CONF *conf = malloc(sizeof(CONF));
    pthread_t threads[MAX_THREAD];
    int8_t rc = 0;

    double elapsedTime = 0.0f;
    time_t begin, end;

    getArgs(conf, argc, argv);

    begin = clock();

    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Mutex init failed\n");
        return 1;
    }

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
    pthread_mutex_destroy(&lock);

    end = clock();
    elapsedTime = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time: %f s.\n", elapsedTime);

    CONF_cleanup(conf);

    return 0;
}
