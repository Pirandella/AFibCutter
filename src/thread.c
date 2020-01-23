#include "thread.h"

pthread_mutex_t lock;

void *threadHandler(void *data){
    pthread_mutex_lock(&lock);
    threadData *tData = (threadData *)data;
    TIME *timeIntervals;
    TIME *time;
    strData *sd = malloc(sizeof(strData));
    uint8_t freq = 128;
    uint32_t dataStartOffset = 0;
    uint32_t dataEndOffset = 0;
    uint32_t dataDifferenceOffset = 0;
    char dstFileAddr[500];
    char qrsFileAddr[500];
    char buffer[100];

    uint32_t localIndex = 0;
    uint32_t globalIndex = 0;
    QRS_Filter *qrs = malloc(sizeof(QRS_Filter));
    // SignalPoint peak;
    SignalPoint pt_peak, hch_peak;
    uint32_t peakCount = 0;
    uint8_t aFibKnown = 0;
    uint8_t aFibFound = 0;
    uint32_t aFibStartTime, aFibEndTime;

    memset(buffer, 0, 100);
    memset(dstFileAddr, 0, 500);

    printf("Thread with ID: %d is working\n", tData->timeID);
    timeIntervals = getTimeIntervals(tData->conf, tData->timeID);
    printf("HH:%s\tMM:%s\tSS:%s\n", timeIntervals->s_hours, timeIntervals->s_minutes, timeIntervals->s_seconds);
    printf("HH:%s\tMM:%s\tSS:%s\n", timeIntervals->e_hours, timeIntervals->e_minutes, timeIntervals->e_seconds);

    createFileName(tData->conf->srcFile, dstFileAddr, qrsFileAddr, timeIntervals);

    FILE *srcFile = fopen(tData->conf->srcFile, "r");
    FILE *dstFile = fopen(dstFileAddr, "w");
    FILE *qrsFile = fopen(qrsFileAddr, "w");

    if((srcFile == NULL) || (dstFile == NULL) || (qrsFile == NULL)){
        puts("Files error!");
        fclose(srcFile);
        fclose(dstFile);
        fclose(qrsFile);
        free(tData);
        free(timeIntervals);
        free(time);
        free(sd);
        free(qrs);
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

    aFibStartTime = dataStartOffset;
    aFibEndTime = dataEndOffset;

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

    // Global indexing of R-peaks
    globalIndex = dataStartOffset;

    // Skip to data start time
    if(dataStartOffset != 0) while(--dataStartOffset > 0) fgets(buffer, 100, srcFile);
    // Start of useful data
    // pthread_mutex_lock(&lock);
    while(dataDifferenceOffset-- > 0){
        fscanf(srcFile, "%d.%d.%d\t%d\t%d\t%f\t%f\t%f\t%f\n", &sd->year, &sd->month, &sd->day, &sd->hours, &sd->minutes, &sd->seconds, &sd->ch0, &sd->ch1, &sd->ch2); // Parse string data

        fprintf(dstFile, "%d:%d:%f\t%f\t%f\t%f\n", sd->hours, sd->minutes, sd->seconds, sd->ch0, sd->ch1, sd->ch2); // Write data to file

        qrs->lowPass = lowPassFilter((int)sd->ch0);
        qrs->highPass = highPassFilter(qrs->lowPass);
        qrs->derivative = derivative(qrs->highPass);
        qrs->square = square(qrs->derivative);
        qrs->movingWindowIntegral = movingWindowIntegral(qrs->square);
        //peak = panTompkins(globalIndex, sd->ch0, qrs->highPass, qrs->square, qrs->movingWindowIntegral);
        //pt_peak = panTompkins(globalIndex, sd->ch0, qrs->highPass, qrs->square, qrs->movingWindowIntegral);
        hch_peak.index = HC_Chen_detect(qrs->highPass);
        if(hch_peak.index == 1){
            hch_peak.index = globalIndex;
            hch_peak.value = sd->ch0;
        }else{
            hch_peak.index = 0;
            hch_peak.value = 0.0;
        }

        // Used with panTompkins algorithm
        // if(peak.value > 0){
        //     if((globalIndex >= aFibStartTime) && (globalIndex <= aFibEndTime)) aFibKnown = 1; // If R-peak founde in boundaries of aFib set variable to 1
        //     else aFibKnown = 0;
        //     peakCount++;
        // }else{
        //     aFibKnown = 0;
        // }

        // For tests only
        fprintf(qrsFile, "%9d\t%f\t%9d\t%f\n", pt_peak.index, pt_peak.value, hch_peak.index, hch_peak.value);
        //--------------
        //fprintf(qrsFile, "%02d:%02d:%f\t%9d\t%4d\t%12f\t%d\t%d\n", sd->hours, sd->minutes, sd->seconds, peak.index, (peak.value) ? peakCount : 0, peak.value, aFibKnown, aFibFound);
        globalIndex++;
        localIndex++;
    }
    pthread_mutex_unlock(&lock);

    printf("Thread with ID: %d finished work.\n", tData->timeID);

    fclose(srcFile);
    fclose(dstFile);
    fclose(qrsFile);
    free(tData);
    free(timeIntervals);
    free(time);
    free(sd);
    free(qrs);
    pthread_exit(NULL);
}
