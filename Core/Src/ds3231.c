/*
 * ds3231.c
 *
 *  Created on: 15-Aug-2020
 *      Author: sandeep
 */

#include "main.h"
#include "ds3231.h"

static uint8_t rtcData[20];

static I2C_HandleTypeDef	ds3231_hi2c;

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

uint8_t DS3231_Init(I2C_HandleTypeDef *hi2c){

	uint8_t res;

	rtcData[0] = 0x00;

	ds3231_hi2c = *hi2c;

	res = HAL_OK;

	if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 1, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	// address 00 sent, now read
	if(HAL_I2C_Master_Receive(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, sizeof(rtcData), 10) != HAL_OK){
		res = HAL_ERROR;
	}

	if((rtcData[CONTROL_STATUS] & 0x80) == 0x80){// if osc stopped
		rtcData[0] = 0x00; // start register address

		rtcData[SECONDS+1] = 0x00;
		rtcData[MINUTES+1] = 0x00;
		rtcData[HOURS+1] = 0x00;
		rtcData[DAY+1] = 0x01; //day 1-7
		rtcData[DATE+1] = 0x01; //date 1-31
		rtcData[MONTH+1] = 0x01; //month 1-12 + century
		rtcData[YEAR+1] = 0x20; //00-99
		rtcData[AL1_SECONDS+1] = 0x00;
		rtcData[AL1_MINUTES+1] = 0x00;
		rtcData[AL1_HOURS+1] = 0x00;
		rtcData[AL1_DAY_DATE+1] = 0x00;
		rtcData[AL2_MINUTES+1] = 0x00;
		rtcData[AL1_HOURS+1] = 0x00;
		rtcData[AL1_DAY_DATE+1] = 0x00;
		rtcData[CONTROL+1] = 0x00; //osc enabled, 1Hz Sqw wave no AL int
		rtcData[CONTROL_STATUS+1] = 0x00; //osc enabled 32KHz disabled

		if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, sizeof(rtcData), 10) != HAL_OK){
			res = HAL_ERROR;
		}
	} else if((rtcData[CONTROL] & 0x04) == 0x04){ //if SQW not enabled
		rtcData[0] = CONTROL; //start reg address
		rtcData[1] = 0x00; //osc enabled, 1Hz SQW enabled

		if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 2, 10) != HAL_OK){
			res = HAL_ERROR;
		}
	}

	return res;

}

uint8_t DS3231_GetTime(RTC_TimeTypeDef *sTime){
	uint8_t res;

	rtcData[0] = 0x00;

	res = HAL_OK;

	if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 1, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	// address 00 sent now read
	if(HAL_I2C_Master_Receive(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 3, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	// populate time

//	sTime->Seconds = bcd2dec(rtcData[0] & 0x7F);
//	sTime->Minutes = bcd2dec(rtcData[1]);
//	sTime->Hours = bcd2dec(rtcData[2] & 0x3F);

	// we only need bcd
	sTime->Seconds = rtcData[SECONDS];
	sTime->Minutes = rtcData[MINUTES];
	sTime->Hours = rtcData[HOURS];

	return res;
}

uint8_t DS3231_SetTime(RTC_TimeTypeDef *sTime){
	uint8_t res;

	rtcData[0] = 0x00; // address


	//data[2] = dec2bcd(sTime->Minutes);
	//data[3] = dec2bcd(sTime->Hours); //24 hr format

	// we will work in BCD
	rtcData[1] = sTime->Seconds;
	rtcData[2] = sTime->Minutes;
	rtcData[3] = sTime->Hours; //24 hr format


	res = HAL_OK;

	if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 4, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	return res;
}

uint8_t DS3231_GetDate(RTC_DateTypeDef *sDate){
	uint8_t res;
	rtcData[0] = 0x30;

	res = HAL_OK;

	if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 1, 10) != HAL_OK){
		res = HAL_ERROR;
	}
	// address 03 sent, now read
	if(HAL_I2C_Master_Receive(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 4, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	// populate date
//	sDate->WeekDay = data[0];
//	sDate->Date = bcd2dec(data[1]);
//	sDate->Month = bcd2dec(data[2]);
//	sDate->Year = bcd2dec(data[3]);

	//bcd
	sDate->WeekDay = rtcData[0];
	sDate->Date = rtcData[1];
	sDate->Month = rtcData[2] & 0x1F; //mask out century
	sDate->Year = rtcData[3];
	return res;
}

uint8_t DS3231_SetDate(RTC_DateTypeDef *sDate){
	uint8_t res;
	res = HAL_OK;
	return res;
}

uint8_t DS3231_GetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime){
	uint8_t res;
	rtcData[0] = 0x00;

	res = HAL_OK;

	if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 1, 10) != HAL_OK){
		res = HAL_ERROR;
	}
	// address 00 sent, now read
	if(HAL_I2C_Master_Receive(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 7, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	// populate time
//	sTime->Seconds = bcd2dec(data[0] & 0x7F);
//	sTime->Minutes = bcd2dec(data[1]);
//	sTime->Hours = bcd2dec(data[2] & 0x3F);

	//bcd out
	sTime->Seconds = rtcData[SECONDS];
	sTime->Minutes = rtcData[MINUTES];
	sTime->Hours = rtcData[HOURS]; //24 hour format

	// populate date
//	sDate->WeekDay = data[3];
//	sDate->Date = bcd2dec(data[4]);
//	sDate->Month = bcd2dec(data[5]);
//	sDate->Year = bcd2dec(data[6]);

	//bcd out
	sDate->WeekDay = rtcData[DAY];
	sDate->Date = rtcData[DATE];
	sDate->Month = rtcData[MONTH] & 0x1F; //mask out century
	sDate->Year = rtcData[YEAR];

	return res;
}

uint8_t DS3231_SetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime){
	uint8_t res;

	rtcData[0] = 0x00; // address

//	rtcData[1] = dec2bcd(sTime->Seconds);
//	rtcData[2] = dec2bcd(sTime->Minutes);
//	rtcData[3] = dec2bcd(sTime->Hours); //24 hr format
//	rtcData[4] = dec2bcd(sDate->WeekDay); // day
//	rtcData[5] = dec2bcd(sDate->Date); //date
//	rtcData[6] = dec2bcd(sDate->Month); //month
//	rtcData[7] = dec2bcd(sDate->Year); //Year

	// dealing with BCD only, for now
	rtcData[1] = sTime->Seconds;
	rtcData[2] = sTime->Minutes;
	rtcData[3] = sTime->Hours & 0x3F; //24 hr format
	rtcData[4] = sDate->WeekDay; // day
	rtcData[5] = sDate->Date; //date
	rtcData[6] = sDate->Month; //month // no century
	rtcData[7] = sDate->Year; //Year

	res = HAL_OK;

	if(HAL_I2C_Master_Transmit(&ds3231_hi2c, DS3231_I2C_SLAVE_ADDRESS , rtcData, 8, 10) != HAL_OK){
		res = HAL_ERROR;
	}

	return res;
}
