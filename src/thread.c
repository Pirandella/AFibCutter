#include "thread.h"

void *threadHandler(void *data){
    threadData *tData = (threadData *)data;
    TIME *timeIntervals;
    TIME *time;
    strData *sd = malloc(sizeof(strData));
    uint8_t freq = 128;
    uint32_t dataStartOffset = 0;
    uint32_t dataEndOffset = 0;
    uint32_t dataDifferenceOffset = 0;
    char dstFileAddr[500];
    char buffer[100];

    unsigned long int index = 0;
    QRS_Filter *qrs_0 = malloc(sizeof(QRS_Filter));
    SignalPoint peak;

    memset(buffer, 0, 100);
    memset(dstFileAddr, 0, 500);

    printf("Thread with ID: %d is working\n", tData->timeID);

    timeIntervals = getTimeIntervals(tData->conf, tData->timeID);
    printf("HH:%s\tMM:%s\tSS:%s\n", timeIntervals->s_hours, timeIntervals->s_minutes, timeIntervals->s_seconds);
    printf("HH:%s\tMM:%s\tSS:%s\n", timeIntervals->e_hours, timeIntervals->e_minutes, timeIntervals->e_seconds);

    createFileName(tData->conf->srcFile, dstFileAddr, timeIntervals);

    FILE *srcFile = fopen(tData->conf->srcFile, "r");
    FILE *dstFile = fopen(dstFileAddr, "w");
    FILE *qrsFile = fopen("./QRS.txt", "w");

    if(srcFile == NULL){
        puts("Source file problems!");
        free(tData);
        free(timeIntervals);
        free(time);
        free(sd);
        pthread_exit(NULL);
    }
    if(dstFile == NULL){
        puts("Destination file problems!");
        free(tData);
        free(timeIntervals);
        free(time);
        free(sd);
        pthread_exit(NULL);
	}

    // ------- Must be redone ------
    fgets(buffer, 100, srcFile);
    fgets(buffer, 100, srcFile);
    // Get first srting time
    time = getTime(buffer);
    // ---------------------------------

    // Get start and end of useful data
    getOffset(timeIntervals, time, &dataStartOffset, &dataEndOffset, freq);

    // Start of useful data calculation
    if(dataStartOffset < tData->conf->offset && dataStartOffset != 0) dataStartOffset -= DIFFERECE(dataStartOffset, tData->conf->offset);
    else if(dataStartOffset != 0)dataStartOffset -= tData->conf->offset;
    // End of useful data calculation
    if(tData->conf->offset < (SECONDS_IN_24H * 128)) dataEndOffset += tData->conf->offset;
    else dataEndOffset += DIFFERECE(dataEndOffset, tData->conf->offset);
    // Number of strings that would be writen to file
    dataDifferenceOffset = DIFFERECE(dataStartOffset, dataEndOffset);

    printf("%d\n", dataStartOffset);
    printf("%d\n", dataEndOffset);

    //index = dataStartOffset;

    // Skip to data start time
    if(dataStartOffset != 0) while(--dataStartOffset > 0) fgets(buffer, 100, srcFile);
    // Start of useful data
    while(dataDifferenceOffset-- > 0){
        fscanf(srcFile, "%d.%d.%d\t%d\t%d\t%f\t%f\t%f\t%f\n", &sd->year, &sd->month, &sd->day, &sd->hours, &sd->minutes, &sd->seconds, &sd->ch0, &sd->ch1, &sd->ch2); // Parse string data

        fprintf(dstFile, "%d:%d:%f\t%f\t%f\t%f\n", sd->hours, sd->minutes, sd->seconds, sd->ch0, sd->ch1, sd->ch2); // Write data to file

        qrs_0->lowPass = lowPassFilter((int)sd->ch0);
        qrs_0->highPass = highPassFilter(qrs_0->lowPass);
        qrs_0->derivative = derivative(qrs_0->highPass);
        qrs_0->square = square(qrs_0->derivative);
        qrs_0->movingWindowIntegral = movingWindowIntegral(qrs_0->square);
        peak = panTompkins(index, sd->ch0, qrs_0->highPass, qrs_0->square, qrs_0->movingWindowIntegral);
        fprintf(qrsFile, "%d\t%f\n", peak.index, peak.value);
        index++;
    }

    printf("Thread with ID: %d finished work.\n", tData->timeID);

    fclose(srcFile);
    fclose(dstFile);
    fclose(qrsFile);

    free(tData);
    free(timeIntervals);
    free(time);
    free(sd);
    pthread_exit(NULL);
}
