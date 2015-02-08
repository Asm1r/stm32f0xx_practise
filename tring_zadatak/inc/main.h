#ifndef MAIN_H
#define MAIN_H

/* Variable types */

/* Function definitions */

/* Init functions */
static void LED_Setup(void);
static void USART1_Setup(void);
static void RTC_Setup(void);
static void Alarm_A_Setup(void);
static void Interrupt_Setup(void);

/* Helper functions */
static void RTC_Timestamp(uint8_t, uint8_t, uint8_t);
static void DLS_Calculate(uint8_t);

/* Interrupt handlers */
void RTC_IRQHandler(void);

#endif
