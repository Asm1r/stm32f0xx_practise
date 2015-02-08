#include <string.h>
#include <stdbool.h>

/* Parts of STM32F0xx Discovery library */
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_usart.h>
#include <stm32f0xx_rtc.h>
#include <stm32f0xx_pwr.h>
#include <stm32f0xx_misc.h>

#include "stm32f0xx.h"	
#include "../inc/main.h"
#include "../inc/utils.h"
#include "../inc/usart_io.h"
#include "../inc/rtc_utils.h"

#define VERSION "pre-alpha :)"

static uint32_t DLSDateTrigger;	/* RTC->DR register alike for daylight saving check */
static uint32_t DLSTimeTrigger; /* RTC->TR register alike for daylight saving check */
static bool led2;		/* LED2 is_on/is_off */

#define INTR 0x07		/* Magic numbers used instead of enums */
#define NOT_INTR 0x0F

/* Enable alarm interrupt */
static void Interrupt_Setup(void)
{
	NVIC_InitTypeDef NvicStructure;
	NvicStructure.NVIC_IRQChannel = RTC_IRQn;
	NvicStructure.NVIC_IRQChannelPriority = 0;
	NvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NvicStructure);
}

/* Configure GPIOs for LED */
static void LED_Setup(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* Enable clock for GPIOC */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	/* Configure PC8 and PC9 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* Setup serial communication with default settings */
static void USART1_Setup(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	USART_InitStructure;

	/* USART1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* GPIOA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* GPIO Configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_0);   // USART1_TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_0);  // USART1_RX


	/* USARTx configuration */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE); 
}
/* Enable RTC clock, because of our hardware (STM32F030R8) which does not include
 * LSE/LSI clock sources we'll use HSE which powers core clock too.
 * Remember to use correct predividers when changing source */
static void RTC_Setup(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStruct;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset RTC Domain */
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	/* Enable the LSE OSC */
	/* STM32F030x8 does not have LSE/LSI */
	/* RCC_LSEConfig(RCC_LSE_ON); */

	/* Enable the HSE OSC */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait until HSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div32);

	/* Configure the RTC data register and RTC prescaler */
	/* RTC_InitStructure.RTC_AsynchPrediv = 0x7F; */
	/* RTC_InitStructure.RTC_SynchPrediv  = 0xFF; */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7C; 	/* This gives one */
	RTC_InitStructure.RTC_SynchPrediv  = 0x1F3F;	/* in ck_spre */
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* Set the time to 00h 00mn 00s AM */
	RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours   = 0x00;
	RTC_TimeStruct.RTC_Minutes = 0x00;
	RTC_TimeStruct.RTC_Seconds = 0x00;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStruct);
} 

static void Alarm_A_Setup(void)
{
	RTC_AlarmTypeDef RTC_AlarmStructure;

	/* Disable the Alarm A */
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	 
	/* Set the alarm to every second*/
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_PM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x00;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x00;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31; /* Nonspecific */
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All; /* Alarm every second */
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	 
	/* Enable RTC Alarm A Interrupt: this Interrupt will wake-up the system
	 * from STANDBY mode (RTC Alarm INTR not enabled in NVIC)
	 * When using HSE clock source clock does not work in sleep */

	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	 
	/* Enable the Alarm A */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	 
	/* Clear RTC Alarm Flag */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}

/* Blink LED1 and check for daylight saving change */
void RTC_IRQHandler(void)
{
	/* We write last bit of RTC->TR (time register) to GPIO so that
	 * it enables LED1 with odd and disables with even number of seconds */
	uint32_t odd = RTC->TR & (uint32_t)0x01;
	uint32_t even = ~odd & (uint32_t)0x01;
	GPIOC->BSRR = (odd << 8) | (even << 24);

	/* Check for Daylight Saving change */
	if ((RTC->DR == DLSDateTrigger) && (RTC->TR == DLSTimeTrigger)) {
		DLS_Calculate(INTR); /* This updates triggers too */
	}
}

/* Calculate daylight saving date and time and according to that change LED2
 * also update daylight saving triggers */
static void  DLS_Calculate(uint8_t arg)
{
	RTC_DLSTypeDef DLS_Dates;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;

	if(arg == INTR) {
		if( led2 ) {	/* Change to winter time */
			RTC_DayLightSavingConfig(RTC_DayLightSaving_ADD1H, \
					RTC_StoreOperation_Set); 

		}else {		/* Change to summer time */
			RTC_DayLightSavingConfig(RTC_DayLightSaving_SUB1H, \
					RTC_StoreOperation_Reset);

		}
	}

	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

	DLS_Dates = RTC_GetDLSDate(RTC_DateStruct.RTC_Year);

	led2 = false;
	switch (RTC_DateStruct.RTC_Month) {
		case RTC_Month_March:
			if (DLS_Dates.StartDLS < RTC_DateStruct.RTC_Date) 
				break;
			if (DLS_Dates.StartDLS == RTC_DateStruct.RTC_Date)
				if(RTC_TimeStruct.RTC_Hours < 0x01)
					break;
		case RTC_Month_October:
			if (DLS_Dates.EndDLS >= RTC_DateStruct.RTC_Date)
				break;
			if (DLS_Dates.EndDLS == RTC_DateStruct.RTC_Date)
				if(RTC_TimeStruct.RTC_Hours < 0x01)
					break;
		case RTC_Month_April: /* Compiler will optimise this */
		case RTC_Month_June:
		case RTC_Month_July:
			/* It's summer time, LED on*/
			led2 = true;
			break;
		default:
			/* Winter time, LED ready set off */
			break;
	}

	if (led2) {
		GPIOC->BSRR = (1 << 9);
		RTC_Timestamp(RTC_DateStruct.RTC_Year, DLS_Dates.EndDLS, 0x01);
	}else {
		GPIOC->BSRR = (1 << 25);
		RTC_Timestamp(RTC_DateStruct.RTC_Year, DLS_Dates.StartDLS, 0x02);
	}
}
/* Helper function used to make create trigger date and time */
static void RTC_Timestamp(uint8_t RTC_Year, uint8_t RTC_Date, uint8_t season)
{
	uint8_t RTC_Month = 0x00;

	if (season == 0x01) {
		RTC_Month = RTC_Month_October;
	}else if (season == 0x02) {
		RTC_Month = RTC_Month_March;
	}else {
		return;
	}

	DLSDateTrigger =  (((uint32_t)RTC_ByteToBcd2(RTC_Year) << 16) | \
		      ((uint32_t)RTC_ByteToBcd2(RTC_Month) << 8) | \
		      ((uint32_t)RTC_ByteToBcd2(RTC_Date)) | \
		      ((uint32_t)RTC_Weekday_Sunday << 13));

	DLSTimeTrigger = (uint32_t)(((uint32_t)RTC_ByteToBcd2(0x01) << 16) | \
                   ((uint32_t)RTC_ByteToBcd2(0x00) << 8) | \
                   ((uint32_t)RTC_ByteToBcd2(0x00)) | \
                   (((uint32_t)RTC_H12_AM) << 16)); 
}

int main(void)
{
	uint8_t option; /* Input option a-d */

	Interrupt_Setup();
	LED_Setup();
	USART1_Setup();
	RTC_Setup();
	DLS_Calculate(NOT_INTR);
	Alarm_A_Setup();

	for (;;) {
		USART_Puts("Izaberite opciju : \n"
				"a) Podesavanje vremena.\n"
				"b) Podesavanje kalendara.\n"
				"c) Ispis vremena.\n"
				"d) Ispis kalendara.\n");

		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		option = USART_ReceiveData(USART1);

		switch (option) {
			case 'a' :
				RTC_SetClock(); 
				DLS_Calculate(NOT_INTR); /* Changed hours */
				Alarm_A_Setup(); /* Changed seconds */
				break;
			case 'b' :
				RTC_SetCalendar();
				DLS_Calculate(NOT_INTR); /* Changed calendar */
				break;
			case 'c' :
				RTC_PrintTime();
				break;
			case 'd' :
				RTC_PrintDate();
				break;
			default:
				USART_Puts("Unesite jedan od ponudenih karaktera.");
				break;
		}
	}
	return 0;
}
