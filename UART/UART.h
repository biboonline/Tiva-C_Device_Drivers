

#ifndef UART_H_
#define UART_H_

#include "UART_Cfg.h"
#include "DMA.h"
#include <stdint.h>

typedef enum {UART_OK = 0, UART_NOK} UART_RetType;
typedef enum {OneStopBit = 0, TwoStopBit = 1} UART_StopBitsType;
typedef enum {Data_5 = 0, Data_6 = 1, Data_7 = 2, Data_8 = 3} UART_WordLength;
typedef enum {Disabled = 0, Enabled = 1} UART_EnableType;
typedef enum {Disable_ALL = 0, Enabled_RX = 1 ,Enabled_TX = 2,ENABLED_ERR = 8} UART_DMA_EnableType;
typedef enum {GroupZero = 0, NotAssigned = 0xFF}DMAGroupNumber;
typedef enum {FIFO_2=0, FIFO_4=1, FIFO_8=2, FIFO_12=3, FIFO_14=4}FIFO_INT_Level ;
typedef enum {Regular_Speed = 16, High_speed = 8} BaudRate_Speed;
typedef enum {SystemClock=0, PIOSC=5}UART_ClockCfg; // if PIOSC is selected, Must be 9MHz or higher.
typedef enum {UART0=0, UART1=1, UART2=2, UART3=3,UART4=4, UART5=5,UART6=6, UART7=7}UARTNumber;
typedef enum {DisableTxInt = 0, EnableTxInt = 32}TXInerrupts;
typedef enum {DisableRxInt = 0, EnableRxInt = 16}RXInerrupts;
typedef void (*UART_CallBackPtr_t)(void);


typedef struct {

uint8_t   UART_ID;
uint32_t BaudRate;
BaudRate_Speed BR_Speed;
UART_StopBitsType StopBits;
UART_WordLength WordLen;
UART_EnableType Parity;
UART_EnableType StickParity;
UART_EnableType FIFOEN;
UART_DMA_EnableType DMAEN;
DMAGroupNumber DMAGroupNum;
FIFO_INT_Level FIFO_Level_TX;
FIFO_INT_Level FIFO_Level_RX;
UART_ClockCfg ClockCfg;

/*interrupts*/
RXInerrupts RxInt;
TXInerrupts TxInt;

/*9 bit mode */
uint8_t  UART9BITAMASK;
uint32_t UART9BITADDR;

/*additional options*/

UART_EnableType  UARTILPR;    //IR low power register.
UART_CallBackPtr_t TxDonePtr;
UART_CallBackPtr_t RxDonePtr;

}UART_Cfg_Type;




UART_RetType UART_Init(void);
//prepare selected uart module for transmission and transmit the firt character.
UART_RetType UART_StartTransmission(uint8_t UART_ID,uint8_t* Tx_text,uint32_t TxLength);
UART_RetType UART_StopCurrentTransmission(uint8_t UART_ID);
UART_RetType UART_GetNumofTxBytes(uint8_t UART_ID, uint32_t *NumPtr);
UART_RetType UART_StartReceiving(uint8_t UART_ID, uint8_t* Rx_Text, uint32_t RxLength);
UART_RetType UART_StopCurrentReception(uint8_t UART_ID);
UART_RetType UART_GetNumofRxBytes(uint8_t UART_ID, uint32_t *NumPtr);
UART_RetType UART_ManageOngoingOperations(uint8_t UART_ID);
UART_RetType UART_ReceiveStatus(uint8_t UART_ID,uint8_t *status,uint8_t shift);
UART_RetType ErrorClear(uint8_t UART_ID,uint8_t shift);
UART_RetType UART_GetInterruptStatus(uint8_t UART_ID,uint16_t *status,uint8_t shift);
UART_RetType UART_ClearInterruptStatus(uint8_t UART_ID,uint8_t shift);
UART_RetType UART_PeripheralProperties(uint8_t UART_ID,uint8_t *status,uint8_t peripheral);

//Call back function to be used on the uart ISR.
void GPS_ReceptionDone();

extern const UART_Cfg_Type UART_CfgParam [USED_UART_MODULES];

#endif /* UART_H_ */
