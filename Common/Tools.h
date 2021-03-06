#ifndef _TOOLS_H_
#define _TOOLS_H_

/* Includes ------------------------------------------------------------------*/
#include "Module/Module_Conf.h"
#include "Convert.h"

/* Private typedef -----------------------------------------------------------*/
struct LogStruct
{
    struct CalendarStruct *calendar;
    int flag;
    void *param;
    
    bool (*CallBack_Transmit)(uint8_t *, uint16_t, void *);
};
/* Private define ------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
void Delay_ms(uint16_t count);
void Delay_us(uint16_t count);
void Log(struct LogStruct *log, uint8_t level, const char *format, ...);
uint32_t Crc32(void *start, uint32_t length, uint32_t crcLast);

#endif
