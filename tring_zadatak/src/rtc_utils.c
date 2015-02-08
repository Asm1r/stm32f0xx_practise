#include <stdint.h>
#include <stm32f0xx_rtc.h>

#include "../inc/usart_io.h"
#include "../inc/rtc_utils.h"

void RTC_SetCalendar(void)
{
        RTC_DateTypeDef RTC_DateStruct;

        /* This gives an error in RTC_SetDate(); */
        RTC_DateStruct.RTC_WeekDay = 0xFF;
        RTC_DateStruct.RTC_Date = 0xFF;
        RTC_DateStruct.RTC_Month = 0xFF;
        RTC_DateStruct.RTC_Year = 0xFF;

        USART_Puts("\n Dan u sedmici (jedna cifra 0-6) : \n");
        while(RTC_DateStruct.RTC_WeekDay == 0xFF)
                RTC_DateStruct.RTC_WeekDay = USART_GetNum(0x01);

        USART_Puts("\n Dan u mjesecu (dvije cifre) : \n");
        while(RTC_DateStruct.RTC_Date == 0xFF)
                RTC_DateStruct.RTC_Date = USART_GetNum(0x02);

        USART_Puts("\n Mjesec (dvije cifre 0-31) : \n");
        while(RTC_DateStruct.RTC_Month == 0xFF)
                RTC_DateStruct.RTC_Month = USART_GetNum(0x02);

        USART_Puts("\n Godina (cetiri cifre) : \n");
        while(RTC_DateStruct.RTC_Year == 0xFF)
                RTC_DateStruct.RTC_Year = USART_GetNum(0x04);

        if ( RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct) == ERROR) {
                USART_Puts("\n Neuspijesno podesavanje kalendara \n");
        }else {
                USART_Puts("\n Kalendar Podesen \n");
                RTC_PrintDate();
        }
}

void RTC_SetClock(void)
{
        RTC_TimeTypeDef RTC_TimeStruct;

        RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
        RTC_TimeStruct.RTC_Hours = 0xFF;
        RTC_TimeStruct.RTC_Minutes = 0xFF;
        RTC_TimeStruct.RTC_Seconds = 0xFF;

        USART_Puts("\nSati (dvije cifre) : \n");
        while(RTC_TimeStruct.RTC_Hours == 0xFF)
                RTC_TimeStruct.RTC_Hours = USART_GetNum(0x02);

        USART_Puts("\nMinute (dvije cifre) : \n");
        while(RTC_TimeStruct.RTC_Minutes == 0xFF)
                RTC_TimeStruct.RTC_Minutes = USART_GetNum(0x02);

        USART_Puts("\nSekunde (dvije cifre) : \n");
        while(RTC_TimeStruct.RTC_Seconds == 0xFF)
                RTC_TimeStruct.RTC_Seconds = USART_GetNum(0x02);

        if (RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct) == ERROR) {
                USART_Puts("\n Neuspijesno podesavanje vremena! \n");
        }else {
                USART_Puts("\n Vrijeme podeseno \n");
                RTC_PrintTime();
        }
}


void RTC_PrintDate(void)
{
        char tmp[5];
        RTC_DateTypeDef RTC_DateStruct;

        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

        USART_Puts("Datum je:\n");

        GetASCIIDigits(RTC_DateStruct.RTC_Date, tmp, 2);
        tmp[2] = '\0';
        USART_Puts(tmp);

        USART_Puts(".");

        GetASCIIDigits(RTC_DateStruct.RTC_Month, tmp, 2);
        tmp[2] = '\0';
        USART_Puts(tmp);

        USART_Puts(".");

        GetASCIIDigits(RTC_DateStruct.RTC_Year, tmp, 4);
        tmp[4] = '\0';
        USART_Puts(tmp);
}

void RTC_PrintTime(void)
{
        char tmp[3];
        RTC_TimeTypeDef RTC_TimeStruct;

        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

        USART_Puts("Vrijeme je:\n");

        GetASCIIDigits(RTC_TimeStruct.RTC_Hours, tmp, 2);
        tmp[2] = '\0';
        USART_Puts(tmp);

        USART_Puts(":");

        GetASCIIDigits(RTC_TimeStruct.RTC_Minutes, tmp, 2);
        tmp[2] = '\0';
        USART_Puts(tmp);

        USART_Puts(":");

        GetASCIIDigits(RTC_TimeStruct.RTC_Seconds, tmp, 2);
        tmp[2] = '\0';
        USART_Puts(tmp);

}

