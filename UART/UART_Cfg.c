#include "UART.h"
#include "UART_Cfg.h"
void trans0 (void);
void receive0 (void);

void trans1 (void);
void receive1 (void);

const UART_Cfg_Type UART_CfgParam [USED_UART_MODULES]={
             {
          0x00,                 //ID
          115200,               //Baud Rate
          Regular_Speed,        // 16 in the baud rate equation
          OneStopBit,
          Data_8,               //Data size
          Disabled,             // parity
          Disabled,             //stick parity
          Disabled,             //fifo
          Disable_ALL,          //DMA
          NotAssigned,         //DMA Group Number
          FIFO_2,              //fifo tx
          FIFO_2,               //fifo rx
          SystemClock,          //UART clock = sytem clock.
          DisableRxInt,         //Recevier interrupt mask.
          DisableTxInt,         //Transmitter interrupt mask.
          0xFF,                 //9Bit mask
          0x0000,               //9Bit address.
          Disabled,             //IR
          trans0,
          receive0

         },

         {
      0x01,                 //ID
      9600,               //Baud Rate
      Regular_Speed,        // 16 in the baud rate equation
      OneStopBit,
      Data_8,               //Data size
      Disabled,             // parity
      Disabled,             //stick parity
      Disabled,              //fifo
      Enabled_RX,          //DMA
      GroupZero,            //DMA Group Number
      FIFO_14,              //FIFO tx
      FIFO_2,               //fifo rx
      SystemClock,          //UART clock = sytem clock.
      DisableRxInt,         //Receiver interrupt mask.
      DisableTxInt,         //Transmitter interrupt mask.
      0xFF,                 //9Bit mask
      0x0000,               //9Bit address.
      Disabled,              //IR
      trans1,
      receive1

     }
};
