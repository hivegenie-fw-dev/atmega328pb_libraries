/*
 * Hx711.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: agu
 *	modified by Niklas Merz
 */
#if 1
#include "hx711.h"
#include <stdlib.h>
#include "eeprom_content.hpp"
#include "pindefs.h"

#define FIXED_POINT_FRACTIONAL_BITS 4

Hx711::Hx711()
{

}

Hx711::~Hx711()
{

}

void Hx711::init()
{
	pinMode(HX711_CLK, OUTPUT);
	pinMode(HX711_DT, INPUT);

	digitalWrite(HX711_CLK, HIGH);
	delayMicroseconds(100);
	digitalWrite(HX711_CLK, LOW);

	averageValue();

	{
		int32_t offset = eeprom_read_dword((uint32_t*)&calibationOffsetEeprom);
//		EEPROM.get<int32_t>(100, offset);
		setOffset(offset);
		Serial.println(offset);
	}

	{
		float slope = eeprom_read_float(&calibationSlopeEeprom);
//		EEPROM.get<float>(104, slope);
		setScale(slope);
		Serial.println(slope);
	}
	Serial.println(F("Hx711 Init Complete"));
}

uint32_t Hx711::averageValue(byte times)
{
	unsigned long sum = 0;
	for (byte i = 0; i < times; i++)
	{
		sum += getValue();
	}

	return sum / times;
}

uint32_t Hx711::calibrate(int32_t weight)
{
	delay(5000);
	uint32_t average = averageValue(30);
	Serial.print('C'); Serial.println(weight);
	Serial.println(average);
	if (weight == 0)
	{
		eeprom_write_dword(&calibationOffsetEeprom, average);
		setOffset(average);
		Serial.println(average);
//		EEPROM.put<uint32_t>(100, average);
//		uint32_t marker = 0xC0FFEE0C;
//		EEPROM.put<uint32_t>(108, marker);
	}
	else
	{
	    float slope = ((double)((double)average - (double)getOffset())/(double)weight);

		eeprom_write_float(&calibationSlopeEeprom, slope);
		Serial.println(slope);
		setScale(slope);
//		EEPROM.put<float>(104, slope);
//		uint32_t marker = 0xC0FFEE0D;
//		EEPROM.put<uint32_t>(108, marker);
	}
	return average;
}

uint32_t Hx711::getValue(const uint32_t timeout)
{
	byte data[3];
	const uint32_t startTime = millis();
	while (digitalRead(HX711_DT))
	{
		if ((millis() - startTime) > timeout)
		{
			Serial.print(F("HX timeout"));
			return 0xFFFF;
		}
	}

	for (byte j = 3; j--;)
	{
		for (char i = 8; i--;)
		{
			digitalWrite(HX711_CLK, HIGH);
			bitWrite(data[j], i, digitalRead(HX711_DT));
			digitalWrite(HX711_CLK, LOW);
		}
	}
	digitalWrite(HX711_CLK, HIGH);
	digitalWrite(HX711_CLK, LOW);

	data[2] ^= 0x80;

	return ((uint32_t) data[2] << 16) | ((uint32_t) data[1] << 8)
			| (uint32_t) data[0];
}

float Hx711::getGram()
{
	long val = (averageValue(15) - _offset);
	return (float) val / _scale;
}

uint16_t Hx711::float_to_fixed(double input)
{
    return (uint16_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

#endif
