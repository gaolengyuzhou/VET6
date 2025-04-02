#ifndef __GPS982_H__
#define __GPS982_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define UART1RX_MAX_LENGTH 256
	
extern uint8_t gUart1RcecBuf[UART1RX_MAX_LENGTH];
//extern ReceDataFrame receDataFrame;

typedef struct {
    uint8_t isGetData;
    uint8_t isValidData;
    char Frame_Buffer[256];
    char UTCTime[20];
    char UsefullFlag[2];
    char latitude[20];
    char N_S[2];
    char longitude[20];
    char E_W[2];
} ReceDataFrame;

extern ReceDataFrame receDataFrame;

void If_valid(void);

void extract_lat_lon(const char *sentence, double *lat, double *lon);
void convert_to_dms(double coord, int *degrees, int *minutes, double *seconds);

#endif /* __GPS982_H__ */
