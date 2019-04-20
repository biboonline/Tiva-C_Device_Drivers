#include "M4MemMap.h"
#include <stdint.h>
#include "DMA.h"


typedef volatile uint32_t* const ADDR_CASTING;
//DMA BASE
#define CTL_TABLE_BASE_ADDR 0x400FF000

//ADDRESS CALCULATION
#define DMA_ADDRESS(BASE,OFFSET) (BASE + OFFSET)
#define DMA_PrimaryChannel(ChannelNum)  (ChannelNum*16)           //calculate Primary channel offset (same as <<1 in hex)
                                                                    // aligned on a 16 byte boundary.
#define DMA_AlternateChannel(ChannelNum) (ChannelNum*16 + 0x200)  //calculate Alternate channel offset

//primary Control Structure offsets
#define SourceEndPtr_Primary(ChannelNum) (DMA_ADDRESS(DMA_PrimaryChannel(ChannelNum),0x00))
#define DestinationEndPtr_Primary(ChannelNum) (DMA_ADDRESS(DMA_PrimaryChannel(ChannelNum),0x04))
#define ControlWord_Primary(ChannelNum) (DMA_ADDRESS(DMA_PrimaryChannel(ChannelNum),0x08))
#define Reserved_Primary(ChannelNum) (DMA_ADDRESS(DMA_PrimaryChannel(ChannelNum),0x0C))

//Alternate control structure offsets
#define SourceEndPtr_Alternate(ChannelNum) (DMA_ADDRESS(DMA_AlternateChannel(ChannelNum),0x00))
#define DestinationEndPtr_Alternate(ChannelNum) (DMA_ADDRESS(DMA_AlternateChannel(ChannelNum),0x04))
#define ControlWord_Alternate(ChannelNum) (DMA_ADDRESS(DMA_AlternateChannel(ChannelNum),0x08))


//Control table base pointer (address is passed to register)

//ADDRESSES
#define DMASTAT                    *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x000))
#define DMACFG                     *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x004))
#define DMACTLBASE                 *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x008))
#define DMAALTBASE                 *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x00C))
#define DMAWAITSTAT                *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x010))
#define DMASWREQ                   *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x014))
#define DMAUSEBURSTSET             *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x018))
#define DMAUSEBURSTCLR             *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x01C))
#define DMAREQMASKSET              *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x020))
#define DMAREQMASKCLR              *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x024))
#define DMAENASET                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x028))
#define DMAENACLR                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x02C))
#define DMAALTSET                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x030))
#define DMAALTCLR                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x034))
#define DMAPRIOSET                 *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x038))
#define DMAPRIOCLR                 *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x03C))
#define DMAERRCLR                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x04C))
#define DMACHASGN                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x500))
#define DMACHIS                    *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x504))
#define DMACHMAP0                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x510))
#define DMACHMAP1                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x514))
#define DMACHMAP2                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x518))
#define DMACHMAP3                  *((ADDR_CASTING)DMA_ADDRESS(CTL_TABLE_BASE_ADDR,0x51C))


/*Constants*/
#define MAX_ARBITRATION_SIZE 1024
#define TRANSFER_SIZE_DEC    1
#define USED_DMA_CHANNEL     8
#define ZERO_CHANNEL_GROUP   7
#define FIRST_CHANNEL_GROUP  15
#define SECOND_CHANNEL_GROUP 23
#define THIRD_CHANNEL_GROUP  31
#define PRIMARY_CHANNEL      0
#define ALTERNATE_CHANNEL    1
#define OFFSET_MULTIPLIER    4
#define ERR_BIT              1

/*Offsets*/
#define TRANSFER_SIZE_OFFSET 4
#define CHMAP1_OFFSET        8
#define CHMAP2_OFFSET        16
#define CHMAP3_OFFSET        24
#define SRCENDPTR_OFFSET     0
#define DESENDPTR_OFFSET     4
#define CTRLWORD_OFFSET      8
#define MEMLOCPLUS1_OFFSET   1
#define MEMLOCPLUS2_OFFSET   2
#define MEMLOCPLUS3_OFFSET   3
#define DSTINC_OFFSET        30
#define DSTSIZE_OFFSET       28
#define SRCINC_OFFSET        26
#define SRCSIZE_OFFSET       24
#define ARBSIZE_OFFSET       14
#define XFERMODE_OFFSET       0



//array of control words
uint32_t CTRL_Words[NumOfUsedChannels];


//array of callbacks
DMA_CallBackPtr_t DoneCallBck[NumOfUsedChannels];

//Done receiving the required number = 1 , O.W = 0
uint8_t DMA_Done_Flag = 0;

//SW req generated flag
uint8_t SWReqGenerated = 0;



//Before the DMA is initialized, It must be enabled on the peripheral side.
//Initializes the DMA modules spcified in the configurations
DMA_RetType DMA_Init(void){

    uint8_t itrator = 0;
    DMA_RetType RetVar;
    const Channel_Attributes* Cfg_Ptr;
    const CTRLWordParameters* CTRL_Ptr;

    RCGCDMA_REG     |= (1 << 0); //enable DMA clock Gating control
    DMACFG          |= (1 << 0); // enable DMA controller
    DMACTLBASE      =  ((uint32_t)CHNControlTable);   //1024 byte aligned address.
                                                 //This specifies the location of the control table in the system memory.
    for(itrator=0;itrator<NumOfUsedChannels;itrator++)
    {
    Cfg_Ptr = &DMA_Cfg[itrator];
    CTRL_Ptr= &CTRLWord[itrator];

    //prepare the  ctrl word register structure according to configurations (without Transfer Size)
            CTRL_Words[Cfg_Ptr->DMA_ID] |=    (uint32_t)(CTRL_Ptr->DSTINC       << DSTINC_OFFSET)    |
                                              (uint32_t)(CTRL_Ptr->DST_SRC_SIZE << DSTSIZE_OFFSET)   |
                                              (uint32_t)(CTRL_Ptr->SRCINC       << SRCINC_OFFSET)    |
                                              (uint32_t)(CTRL_Ptr->DST_SRC_SIZE << SRCSIZE_OFFSET)   |
                                              (uint32_t)(CTRL_Ptr->ARBSize      << ARBSIZE_OFFSET)   |
                                              (uint32_t)(CTRL_Ptr->TransferMode << XFERMODE_OFFSET);


    //Peripheral To memory
    if(Target == Peripheral_to_Memory)
    {
        //CLEAR then set if configured
              DMAPRIOCLR |= (1 << Cfg_Ptr->ChannelNo);         //priority
              DMAPRIOSET |= (Cfg_Ptr->Priority<< Cfg_Ptr->ChannelNo);

              DMAALTCLR |= (1 << PRIMARY_CHANNEL);          //Primary Channel
              DMAALTSET |= (PRIMARY_CHANNEL << Cfg_Ptr->ChannelNo);

              DMAUSEBURSTCLR |= (1 << Cfg_Ptr->ChannelNo);     //Burst control
              DMAUSEBURSTSET |= (Cfg_Ptr->BurstOnly << Cfg_Ptr->ChannelNo);

              DMAREQMASKCLR |= (1 << Cfg_Ptr->ChannelNo);     //Enable \ Disable requests
              DMAREQMASKSET |= ((Cfg_Ptr -> EnableDMARequests) << (Cfg_Ptr->ChannelNo));

              //Store the CallBack Pointers in their place in the array
              DoneCallBck[itrator] = (Cfg_Ptr->DMADonePtr);

              //Channel assignment
              //channel assignment is divided into 4 groups
              //each register CHANNEL MAP holds 8 channels each has 4 bit field
              //values from 1 to 4 are  filled in the register to choose the channel assignment to a specific peripheral mentioned in the table.
              if((Cfg_Ptr -> ChannelNo) <= ZERO_CHANNEL_GROUP)
              {
                  DMACHMAP0 |= ((Cfg_Ptr -> ChannelAssignments) << ((Cfg_Ptr -> ChannelNo) * OFFSET_MULTIPLIER));
              }
              else if((Cfg_Ptr -> ChannelNo) <= FIRST_CHANNEL_GROUP)
              {
                  DMACHMAP1 |= ((Cfg_Ptr -> ChannelAssignments) << (((Cfg_Ptr -> ChannelNo) - CHMAP1_OFFSET) * OFFSET_MULTIPLIER));
              }
              else if((Cfg_Ptr -> ChannelNo) <= SECOND_CHANNEL_GROUP)
              {
                  DMACHMAP2 |= ((Cfg_Ptr -> ChannelAssignments) << (((Cfg_Ptr -> ChannelNo) - CHMAP2_OFFSET) * OFFSET_MULTIPLIER));
              }
              else if((Cfg_Ptr -> ChannelNo) <= THIRD_CHANNEL_GROUP)
              {
                  DMACHMAP3 |= ((Cfg_Ptr -> ChannelAssignments) << (((Cfg_Ptr -> ChannelNo) - CHMAP3_OFFSET) * OFFSET_MULTIPLIER));
              }
              else
              {
                  RetVar = DMA_NOK;
              }
        }
        else
        {
            RetVar = DMA_NOK;
        }
    }//end of the loop
return RetVar;
}


//This function enables the specified channel (related to the peripheral)
//by assigning the Configurations in the control table located in the memory.
DMA_RetType DMA_StartChannel(uint8_t DMAGroupID,uint32_t SRC, uint32_t DES, uint16_t TransferSize)
{
    DMA_RetType RetVar;
    const Channel_Attributes* Cfg_Ptr;
    Cfg_Ptr = &DMA_Cfg[DMAGroupID];
    //Check if the Transfer Size is a valid number
    if(TransferSize <= MAX_ARBITRATION_SIZE)
    {
        //Disable DMA Channel
        DMAENACLR |=  (1 << (Cfg_Ptr->ChannelNo));

        // Assigning Source, Destination & control word to the specfied memory location
        // Based on their channel number
        ((DMA_ADDRESSES*)DMACTLBASE)[USED_DMA_CHANNEL].SRCENDP = (uint32_t)SRC;
        ((DMA_ADDRESSES*)DMACTLBASE)[USED_DMA_CHANNEL].DESENDP = (uint32_t)DES;
        //assigning transfer size to the CTRL Word
        CTRL_Words[Cfg_Ptr->DMA_ID] |= ((TransferSize - TRANSFER_SIZE_DEC) <<  TRANSFER_SIZE_OFFSET);
        ((DMA_ADDRESSES*)DMACTLBASE)[USED_DMA_CHANNEL].CHCTLP |= CTRL_Words[Cfg_Ptr->DMA_ID];

        //Enable DMA channel
        DMAENASET |= (1<<Cfg_Ptr->ChannelNo);

        RetVar = DMA_OK;
    }
    else
    {
        RetVar = DMA_NOK;
    }
    return RetVar;

}


//generate a request by software.
void GenerateSWReq(uint8_t ChannelNum)
{

    if(SWReqGenerated == 0)
    {
    DMASWREQ |= (1 << ChannelNum);
    SWReqGenerated = 1;
    }


}


void ISR_ErrorHandler()
{   //if an error is detected , clear the flag and increment error counter.
    if (((DMAERRCLR & ERR_BIT) == ERR_BIT))
    {
        /*ToDo:*/

        //clear the error flag.
        DMAERRCLR |= 1;
    }
    else
    {
        /*ToDo*/
    }
}

//Disable the Current channel.
DMA_RetType DMA_StopChannel(uint8_t ChannelID)
{
    DMA_RetType RetVar;
    const Channel_Attributes* Cfg_Ptr;
    Cfg_Ptr = &DMA_Cfg[ChannelID];

    if(ChannelID < NumOfUsedChannels)
    {
        //Disable DMA Channel
        DMAENASET &= ~(1 << (Cfg_Ptr->ChannelNo));

        RetVar = DMA_OK;
    }
    else
    {
        RetVar = DMA_NOK;
    }
        return RetVar;
}




//callback function on DMA Done transmitting
void DMA_Done(void)
{
    if(DMA_Done_Flag == 0)
    {
        DMA_Done_Flag = 1;
    }

}

