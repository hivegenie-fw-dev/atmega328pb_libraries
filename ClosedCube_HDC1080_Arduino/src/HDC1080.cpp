/*

Arduino Library for Texas Instruments HDC1080 Digital Humidity and Temperature Sensor
Written by AA for ClosedCube
---

The MIT License (MIT)

Copyright (c) 2016 ClosedCube Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "Wire2.h"

#include "HDC1080.h"


HDC1080::HDC1080()
{
}

void HDC1080::i2cScanner2()
{

	int nDevices = 0;
	for (uint8_t address = 1; address < 127; address++)
	{

		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		delay(10);
		Wire2.beginTransmission(address);
		int8_t error = Wire2.endTransmission();
		if (error == 0)
		{
			Serial.print(F("I2C device found at address 0x"));
			if (address < 16)
				Serial.print(F("0"));
			Serial.print(address, HEX);
			Serial.println(F("  !"));

			nDevices++;
		}
		else if (error == 4)
		{
			Serial.print(F("Unknown error at address 0x"));
			if (address < 16)
				Serial.print(F("0"));
			Serial.println(address, DEC);
		}
	}
	if (nDevices == 0)
		Serial.println(F("No I2C devices found\n"));
	else
		Serial.println(F("done\n"));
}

uint8_t HDC1080::begin(uint8_t address) {
	_address = address;
	/*
	 Heater disabled,
	 Temperature and Humidity Measurement Resolution 14 bit
	*/
	Wire2.beginTransmission(_address);
	Wire2.write(CONFIGURATION);
	Wire2.write(0x0);
	Wire2.write(0x0);

	return Wire2.endTransmission();

}

float HDC1080::readT() {
	return readTemperature();
}

float HDC1080::readTemperature() {
	uint16_t rawT = readData(TEMPERATURE);
	return (rawT / pow(2, 16)) * 165 - 40;
}

float HDC1080::readH() {
	return readHumidity();
}

float HDC1080::readHumidity() {
	uint16_t rawH = readData(HUMIDITY);
	return (rawH / pow(2, 16)) * 100;
}

uint16_t HDC1080::readManufacturerId() {
	return readData(MANUFACTURER_ID);
}

uint16_t HDC1080::readDeviceId() {
	return readData(DEVICE_ID);
}

uint16_t HDC1080::readData(uint8_t pointer)
{

	Wire2.beginTransmission(_address);
	Wire2.write(pointer);
	Wire2.endTransmission();

	delay(30);
	const uint8_t len = Wire2.requestFrom(_address, (uint8_t)2);

//	while (!Wire2.available());
	if (len != 2)
	{
		return -1;
	}
	byte msb = Wire2.read();
	byte lsb = Wire2.read();

	return msb << 8 | lsb;
}

HDC1080 g_hdc1080;
