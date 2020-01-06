#include "argParser.h"

static char *args[ARG_NUM] = {"-t", "-o", "-s", "-d", "-c", "-h"};

void getArgs(CONF *conf, int argc, char **argv){

    if(argc < 2){
        puts("Not enought argument. Type -h to for help.");
        free(conf);
        exit(2);
    }

    uint16_t timeCodeOffset = 0;
    uint8_t timeCodeFlag = 0;

    uint8_t argCheck = 0;

    int8_t srcFileExist = 0;
    int8_t dstFileExist = 0;

    conf->offset = 0;
    conf->channels = -1;
    conf->dstFile = "none";

    for(uint8_t i = 1; i < argc; i++){
        for(uint8_t j = 0; j < ARG_NUM; j++){
            if(!strcmp(argv[i], args[j])){
                timeCodeFlag = 0;
                switch(j){
                    case 0: // "-t" time codes (Start time: HH:MM:SS (Separator '/' or any other symbol axcept for 'Space') End time: HH:MM:SS) (Array)
                        timeCodeOffset = i + 1;
                        timeCodeFlag = 1;
                        argCheck |= 1;
                        break;
                    case 1: // "-o" offset in seconds (optional)
                        conf->offset = atoi(argv[i + 1]);
                        argCheck |= (1 << 1);
                        break;
                    case 2: // "-s" source file
                        conf->srcFile = argv[i + 1];
                        srcFileExist = access(conf->srcFile, F_OK | R_OK);
                        argCheck |= (1 << 2);
                        break;
                    case 3: // "-d" destionation file (optional)
                        conf->dstFile = argv[i + 1];
                        dstFileExist = access(conf->dstFile, F_OK | R_OK);
                        argCheck |= (1 << 3);
                        break;
                    case 4: // "-c" data channel (optional)
                        conf->channels = atoi(argv[i + 1]);
                        if(conf->channels > 16) conf->channels = 16;
                        if(conf->channels < (-1)) conf->channels = -1;
                        argCheck |= (1 << 4);
                        break;
                    case 5: // "-h" help
                        // TODO write help
                        exit(1);
                }
            }
        }
        if(timeCodeFlag) conf->timeCodeCount++;
    }

    if(!(argCheck < 5)){
        // Check if srcFile doesn't exist
        if(srcFileExist == -1){
            puts("Source file doesn't exist or doesn't have read permission!");
            free(conf);
            exit(3);
        }

        if(argCheck & 2){
            // Check if dstFile doesn't exist
            if(dstFileExist == -1){
                puts("Destionation file doesn't exist or doesn't have read permission!");
                free(conf);
                exit(3);
            }
        }

    }else{
        puts("Not enought argument. Type -h to for help.");
        free(conf);
        exit(2);
    }

    conf->timeCodeCount -= 1;

    conf->timeCode = malloc(conf->timeCodeCount);
    for(uint8_t i = 0; i < conf->timeCodeCount; i++)
        conf->timeCode[i] = argv[timeCodeOffset + i];

}

void CONF_cleanup(CONF *conf){
    free(conf->timeCode);
    free(conf);
}
