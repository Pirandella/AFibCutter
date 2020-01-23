#ifndef _QRS_H_
#define _QRS_H_

#include <stdio.h>
#include <stdint.h>

#define SAMPLING_RATE       128

typedef struct{
   float value;
   int32_t index;
}SignalPoint;

typedef struct{
    int lowPass;
    int highPass;
    int derivative;
    int square;
    int movingWindowIntegral;
}QRS_Filter;

typedef enum {false, true} bool;

int lowPassFilter(int data);
int highPassFilter(int data);
int derivative(int data);
int square(int data);
int movingWindowIntegral(int data);
SignalPoint panTompkins(int sample,float value,int bandpass,int square,int integral);
#endif  // _QRS_H_
