#include <stdint.h>
#include <stm32f0xx_usart.h>

#include "../inc/usart_io.h"

/* Gets number from USART, where digits is number of digits
 * max number of digits is 4 (hardcoded).*/
uint8_t USART_GetNum(uint32_t digits)
{
        /* assert((digits <= 4)); */
        uint8_t i = 0x00;
        uint8_t tmp[4] = {0x00}; 

        for (i = 0x00; i < digits; i++) { 

                /* Loop until RXNE = 1 */
                while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

                tmp[i] = USART_ReceiveData(USART1);

                if ((tmp[i] < 0x30) || (tmp[i] > 0x39)) { 
                        USART_Puts("\n Inserted  a non number \n");
                }  
        } 

        /* Calculate the Corresponding value */
        i =     ((tmp[0] - 0x30) * 1000) + ((tmp[1] - 0x30) * 100)
                + ((tmp[2] - 0x30) * 10) + (tmp[3] - 0x30);
        return i;
}

/* Prins string using USART, string should be NULL terminated
 * this function is dangerous if input is not valid (fixme)
 */
void USART_Puts(char *input)
{
        uint32_t i = 0x00;
        uint32_t size = strlen(input);

        for ( i = 0x00; input[i] <= size; i++) {
                while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
                USART_SendData(USART1, input[i]);
        }
}
