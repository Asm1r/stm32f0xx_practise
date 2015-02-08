#ifndef UTILS_H
#define UTILS_H

struct RTC_DLS {
	uint8_t StartDLS;
	uint8_t EndDLS;
};
typedef struct RTC_DLS RTC_DLSTypeDef;

/* Function definitions */
void GetASCIIDigits(uint8_t, char *, uint8_t);
uint8_t RTC_ByteToBcd2(uint8_t);
RTC_DLSTypeDef RTC_GetDLSDate(uint8_t);

#endif
