#include "thread.h"

void *threadHandler(void *data){
    threadData *tData = (threadData *)data;
    TIME *timeIntervals;
    TIME *time;
    uint8_t freq = 128;
    uint32_t dataStartOffset = 0;
    uint32_t dataEndOffset = 0;
    uint32_t dataDifferenceOffset = 0;
    char dstFileAddr[500];
    char buffer[100];
    char buffer_0[100];

    memset(buffer, 0, 100);
    memset(dstFileAddr, 0, 500);

    printf("Thread with ID: %d is working\n", tData->timeID);

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

    if(dataStartOffset < tData->conf->offset && dataStartOffset != 0) dataStartOffset -= DIFFERECE(dataStartOffset, tData->conf->offset);
    else if(dataStartOffset != 0)dataStartOffset -= tData->conf->offset;
    
    if(tData->conf->offset < (SECONDS_IN_24H * 128)) dataEndOffset += tData->conf->offset;
    else dataEndOffset += DIFFERECE(dataEndOffset, tData->conf->offset);

    dataDifferenceOffset = DIFFERECE(dataStartOffset, dataEndOffset);

    printf("%d\n", dataStartOffset);
    printf("%d\n", dataEndOffset);

    // Skip to data start time
    if(dataStartOffset != 0) while(--dataStartOffset > 0) fgets(buffer, 100, srcFile);
    printf("%s", buffer);
    while(dataDifferenceOffset-- > 0){
        fgets(buffer, 100, srcFile);
        strncpy(buffer_0, &buffer[H_OFFSET], 89);
        for(uint8_t i = 0; i < 6; i++) if(buffer_0[i] == 0x09) buffer_0[i] = ':';
        fprintf(dstFile, "%s", buffer_0);
    }
    printf("%s", buffer_0);

    printf("Thread with ID: %d finished work.\n", tData->timeID);

    srcFile = NULL;
    dstFile = NULL;
    free(tData);
    free(timeIntervals);
    free(time);
    pthread_exit(NULL);
}
