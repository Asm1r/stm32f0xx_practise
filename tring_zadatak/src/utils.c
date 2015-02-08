#include <stdint.h>

#include "../inc/utils.h"

/* Converts input to an array of characters, output, representing it's
 * decimal notation. Size is number of digits max number of digits is 
 * 4 (hardcoded),
 */
void GetASCIIDigits(uint8_t input, char output[], uint8_t size)
{
        uint8_t i = 0x00;

        for (i = 0x00; i < size; i++) {
                output[i] = 0x30;
        }   

        for (i = 0x00; i < size; i++) {
                output[i] += (char)input % 10; 

                if(!input)
                        break;

                input /= 10; 
        }   
}

/* Convert binar to BCD notation */
uint8_t RTC_ByteToBcd2(uint8_t Value)
{
        uint8_t bcdhigh = 0;

        while (Value >= 10) {
          bcdhigh++;
          Value -= 10;
        }

        return  ((uint8_t)(bcdhigh << 4) | Value);
}

/* Calculate date of Daylight Saving Time for given year */
RTC_DLSTypeDef RTC_GetDLSDate(uint8_t RTC_Year)
{
        RTC_DLSTypeDef DLS_DateStructure;
            
        DLS_DateStructure.StartDLS = (31 - ((((5 * RTC_Year) / 4) + 4) % 7));
        DLS_DateStructure.EndDLS = (31 - ((((5 * RTC_Year) / 4) + 1) % 7));

        return DLS_DateStructure;
}

