#include "DMA.h"
#include "DMA_Cfg.h"

void DMA_Done(void);

const Channel_Attributes DMA_Cfg[NumOfUsedChannels] = {
//channel 8 is UART1 RX
/*
 * {CHANNEL ID,
 * Channel No
 * Priority
 * Burst only
 * Enable DMA Request
 * Channel Assignments.
 * DMA Callback
 */
        { 0, 8, 0, 0, 0, 1, DMA_Done}

};

const CTRLWordParameters CTRLWord[NumOfUsedChannels] = {eightBit, eightBit_D, No_increment, T1024, Basic};

TargetDMA Target = Peripheral_to_Memory;
