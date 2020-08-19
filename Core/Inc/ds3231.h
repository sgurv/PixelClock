/*
 * ds3231.h
 *
 *  Created on: 15-Aug-2020
 *      Author: sandeep
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

#include "main.h"

#define 	DS3231_I2C_SLAVE_ADDRESS	0xD0

#define     SECONDS     0x00
#define     MINUTES     0x01
#define     HOURS       0x02
#define     DAY         0x03
#define     DATE        0x04
#define     MONTH       0x05
#define     YEAR        0x06
#define     AL1_SECONDS 0x07
#define     AL1_MINUTES 0x08
#define     AL1_HOURS   0x09
#define     AL1_DAY_DATE    0x0A
#define     AL2_MINUTES     0x0B
#define     AL2_HOURS       0x0C
#define     AL2_DAY_DATE    0x0D
#define     CONTROL         0x0E
#define     CONTROL_STATUS  0x0F
#define     AGING_OFFSET    0x10
#define     MSB_TEMP        0x11
#define     LSB_TEMP        0x12


uint8_t dec2bcd(uint8_t num);
uint8_t bcd2dec(uint8_t num);
uint8_t DS3231_Init(I2C_HandleTypeDef *hi2c);
uint8_t DS3231_GetTime(RTC_TimeTypeDef *sTime);
uint8_t DS3231_SetTime(RTC_TimeTypeDef *sTime);
uint8_t DS3231_GetDate(RTC_DateTypeDef *sDate);
uint8_t DS3231_SetDate(RTC_DateTypeDef *sDate);
uint8_t DS3231_GetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
uint8_t DS3231_SetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);


#endif /* INC_DS3231_H_ */
