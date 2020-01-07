#include "fileHandler.h"

TIME *getTimeIntervals(CONF *conf, uint8_t timeID){
    TIME *time = malloc(sizeof(TIME));

    time->s_hours[0]    = conf->timeCode[timeID][0];
    time->s_hours[1]    = conf->timeCode[timeID][1];
    time->s_minutes[0]  = conf->timeCode[timeID][3];
    time->s_minutes[1]  = conf->timeCode[timeID][4];
    time->s_seconds[0]  = conf->timeCode[timeID][6];
    time->s_seconds[1]  = conf->timeCode[timeID][7];

    time->e_hours[0]    = conf->timeCode[timeID][9];
    time->e_hours[1]    = conf->timeCode[timeID][10];
    time->e_minutes[0]  = conf->timeCode[timeID][12];
    time->e_minutes[1]  = conf->timeCode[timeID][13];
    time->e_seconds[0]  = conf->timeCode[timeID][15];
    time->e_seconds[1]  = conf->timeCode[timeID][16];

    return time;
}

TIME *getTime(const char *buffer){
    TIME *time = malloc(sizeof(TIME));

    if(buffer[H_OFFSET + 1] == 0x09){ // HTAB
        time->s_hours[0] = buffer[H_OFFSET];
        time->s_hours[1] = '\0';
        if(buffer[M_OFFSET] == 0x09){ // MTAB
            time->s_minutes[0] = buffer[M_OFFSET - 1];
            time->s_minutes[1] = '\0';
            if((buffer[S_OFFSET - 1] == '.') || (buffer[S_OFFSET - 1] == 0x09)){ // S.
                time->s_seconds[0] = buffer[S_OFFSET - 2];
                time->s_seconds[1] = '\0';
            }else{ // SS
                time->s_seconds[0] = buffer[S_OFFSET - 2];
                time->s_seconds[1] = buffer[S_OFFSET - 1];
            }
        }else{ // MM
            time->s_minutes[0] = buffer[M_OFFSET - 1];
            time->s_minutes[1] = buffer[M_OFFSET];
            if((buffer[S_OFFSET] == '.') || (buffer[S_OFFSET - 1] == 0x09)){ // S.
                time->s_seconds[0] = buffer[S_OFFSET - 1];
                time->s_seconds[1] = '\0';
            }else{ // SS
                time->s_seconds[0] = buffer[S_OFFSET - 1];
                time->s_seconds[1] = buffer[S_OFFSET];
            }
        }
    }else{ // HH
        time->s_hours[0] = buffer[H_OFFSET];
        time->s_hours[1] = buffer[H_OFFSET + 1];
        if(buffer[M_OFFSET + 1] == 0x09){ // MTAB
            time->s_minutes[0] = buffer[M_OFFSET];
            time->s_minutes[1] = '\0';
            if((buffer[S_OFFSET] == '.') || (buffer[S_OFFSET] == 0x09)){ // S.TAB
                time->s_seconds[0] = buffer[S_OFFSET - 1];
                time->s_seconds[1] = '\0';
            }else{ // SS
                time->s_seconds[0] = buffer[S_OFFSET - 1];
                time->s_seconds[1] = buffer[S_OFFSET];
            }
        }else{ // MM
            time->s_minutes[0] = buffer[M_OFFSET];
            time->s_minutes[1] = buffer[M_OFFSET + 1];
            if((buffer[S_OFFSET + 1] == '.') || (buffer[S_OFFSET + 1] == 0x09)){ // S.
                time->s_seconds[0] = buffer[S_OFFSET];
                time->s_seconds[1] = '\0';
            }else{ // SS
                time->s_seconds[0] = buffer[S_OFFSET];
                time->s_seconds[1] = buffer[S_OFFSET + 1];
            }
        }
    }

    return time;
}

void createFileName(const char *srcFile, char *dstFile, TIME *time){
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint32_t secStart, secEnd, secDiff;
    char tmp[23];

    secStart = (atoi(time->s_hours) * 3600) + (atoi(time->s_minutes) * 60) + (atoi(time->s_seconds));
    secEnd = (atoi(time->e_hours) * 3600) + (atoi(time->e_minutes) * 60) + (atoi(time->e_seconds));

    secDiff = DIFFERECE(secStart, secEnd);
    hours = secDiff / 3600;
    minutes = (secDiff - (3600 * hours)) / 60;
    seconds = (secDiff - (3600 * hours) - (minutes * 60));

    sprintf(tmp, "_%s:%s:%s_%02d:%02d:%02d.txt", time->s_hours, time->s_minutes, time->s_seconds, hours, minutes, seconds);
    strncpy(dstFile, srcFile, (strrchr(srcFile, '.') - srcFile));
    strcat(dstFile, tmp);
}

void getOffset(TIME *timeIntervals, TIME *startTime, uint32_t *dataStartOffset, uint32_t *dataEndOffset, uint16_t freq){
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint32_t secStart, secEnd;

    secStart = (atoi(startTime->s_hours) * 3600) + (atoi(startTime->s_minutes) * 60) + (atoi(startTime->s_seconds));

    secEnd = (atoi(timeIntervals->s_hours) * 3600) + (atoi(timeIntervals->s_minutes) * 60) + (atoi(timeIntervals->s_seconds));

    if(atoi(timeIntervals->s_hours) < 9) *dataStartOffset = (SECONDS_IN_24H - secStart + secEnd) * freq;
    else *dataStartOffset = (SECONDS_IN_24H - secStart + (secEnd - SECONDS_IN_24H)) * freq;

    secEnd = (atoi(timeIntervals->e_hours) * 3600) + (atoi(timeIntervals->e_minutes) * 60) + (atoi(timeIntervals->e_seconds));
    if(atoi(timeIntervals->e_hours) < 9) *dataEndOffset = (SECONDS_IN_24H - secStart + secEnd) * freq;
    else *dataEndOffset = (SECONDS_IN_24H - secStart + (secEnd - SECONDS_IN_24H)) * freq;
}
