
#ifndef DMA_H_
#define DMA_H_

#include "DMA_Cfg.h"
#include <stdint.h>

#define RECEPTION_ARRAY_SIZE    1024

#pragma DATA_ALIGN(CHNControlTable,1024);
uint8_t CHNControlTable[RECEPTION_ARRAY_SIZE];

typedef volatile uint32_t ADDR_CASTING_CTRL;
typedef void (*DMA_CallBackPtr_t)(void);

typedef struct
{
uint8_t DMA_ID;
uint8_t ChannelNo;
uint8_t Priority;            //0->default , 1->high
uint8_t BurstOnly;           //0-> Respond to all 1->Respond to Burst requests Only.
uint8_t EnableDMARequests;   //0-> Enable DMA Requests 1->Disable DMA Requests.
uint8_t ChannelAssignments;  // [0, 1, 2, 3, 4] according to table in data sheet.

DMA_CallBackPtr_t DMADonePtr;
}Channel_Attributes;


typedef enum {DMA_OK = 0, DMA_NOK} DMA_RetType;
typedef enum {Peripheral_to_Memory}TargetDMA;

typedef enum {eightBit=0x0,sixteenBit,ThirtyTwoBit,No_increment}DST_SRC_INC;//DST shift by 30 SRC shift by 26
typedef enum {eightBit_D=0x0,sixteenBit_D,ThirtyTwoBit_D}DST_SRC_SIZE;  //DST shift by 28 SRC shift by 24

typedef enum {T1 = 0x0,T2,T4,T8,T16,T32,T64,T128,T256,T512,T1024}ARBSIZE;     //Number of transfers
typedef enum {Stop = 0x0, Basic, AutoRequest}XFERMODE;

typedef struct
{   DST_SRC_INC DSTINC;
    DST_SRC_SIZE DST_SRC_SIZE;
    DST_SRC_INC SRCINC;
    ARBSIZE ARBSize;
    XFERMODE TransferMode;
}CTRLWordParameters;


typedef struct
{
    uint32_t SRCENDP;
    uint32_t DESENDP;
    uint32_t CHCTLP;
    uint32_t RESERVED;
}DMA_ADDRESSES;

extern const Channel_Attributes DMA_Cfg[NumOfUsedChannels];
extern const CTRLWordParameters CTRLWord[NumOfUsedChannels];

extern TargetDMA Target;

DMA_RetType DMA_Init(void);
DMA_RetType DMA_StartChannel(uint8_t DMAGroupID, uint32_t SRC, uint32_t DES, uint16_t TransferSize);
DMA_RetType DMA_StopChannel(uint8_t ChannelID);


void DMA_Done(void);
void GenerateSWReq(uint8_t ChannelNum);

uint8_t DMA_ReqStatus(uint8_t ChannelNum);

#endif /* DMA_H_ */
