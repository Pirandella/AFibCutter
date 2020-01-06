#include "thread.h"

void *threadHandler(void *data){
    threadData *tData = (threadData *)data;
    TIME *timeIntervals;
    TIME *time;
    uint8_t freq = 128;
    uint32_t dataStartOffset;
    uint32_t dataEndOffset;
    uint32_t dataDifferenceOffset;
    char dstFileAddr[500];
    char buffer[100];

    double elapsedTime = 0.0f;
    time_t begin, end;

    memset(buffer, 0, 100);
    memset(dstFileAddr, 0, 500);

    printf("Thread with ID: %d is working\n", tData->timeID);
    begin = clock();

    timeIntervals = getTimeIntervals(tData->conf, tData->timeID);
    printf("HH:%s\tMM:%s\tSS:%s\n", timeIntervals->s_hours, timeIntervals->s_minutes, timeIntervals->s_seconds);
    printf("HH:%s\tMM:%s\tSS:%s\n", timeIntervals->e_hours, timeIntervals->e_minutes, timeIntervals->e_seconds);

    createFileName(tData->conf->srcFile, dstFileAddr, timeIntervals);

    FILE *srcFile = fopen(tData->conf->srcFile, "r");
    FILE *dstFile = fopen(dstFileAddr, "w");

    if(srcFile == NULL){
        puts("Source file problems!");
        free(tData);
        free(timeIntervals);
        free(time);
        pthread_exit(NULL);
    }
    if(dstFile == NULL){
        puts("Destination file problems!");
        free(tData);
        free(timeIntervals);
        free(time);
        pthread_exit(NULL);
	}

    fgets(buffer, 100, srcFile);
    fgets(buffer, 100, srcFile);
    time = getTime(buffer);

    getOffset(timeIntervals, time, &dataStartOffset, &dataEndOffset, freq);
    dataStartOffset -= tData->conf->offset;
    dataEndOffset += tData->conf->offset;
    dataDifferenceOffset = DIFFERECE(dataStartOffset, dataEndOffset);

    // Skip to data start time
    while(--dataStartOffset > 0) fgets(buffer, 100, srcFile);
    while(dataDifferenceOffset-- != 0){
        fgets(buffer, 100, srcFile);
        fprintf(dstFile, "%s", buffer);
    }

    end = clock();
    elapsedTime = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Thread with ID: %d finished work.\tTime: %f s.\n", tData->timeID, elapsedTime);

    srcFile = NULL;
    dstFile = NULL;
    free(tData);
    free(timeIntervals);
    free(time);
    pthread_exit(NULL);
}
