/* Arduino library for digital weight scale of hx711
 *
 * hardware design: syyyd
 * available at http://syyyd.taobao.com
 *
 * library design: Weihong Guan (@aguegu)
 * http://aguegu.net
 *
 * library host on
 * https://github.com/aguegu/Arduino
 *
 *  Created on: Oct 31, 2012
 */

#ifndef HX711_H_
#define HX711_H_

#include "Arduino.h"

class Hx711
{
public:
	Hx711();
	virtual ~Hx711();

	void init();

	uint32_t getValue(const uint32_t timeout = 3000);
	uint32_t averageValue(byte times = 2); // 32
	float getGram(bool enableAcExcitation = true);
	uint32_t calibrate(int32_t weight, bool enableAcExcitation = true);


	void setOffset(uint32_t offset)
	{
		_offset = offset;
	}

	uint32_t getOffset()
	{
		return _offset;
	}
	void setScale(float scale = 742.f)
	{
		_scale = scale;
	}
	float getScale()
	{
		return _scale;
	}
    uint16_t float_to_fixed(double input);

private:
	long _offset;
	float _scale;

	inline void powerOn()
	{
		PORTB |= (1 << PIN7);
	}

	inline void powerOff()
	{
		PORTB &= ~(1 << PIN7);
	}

	inline void posExcitation()
	{
		PORTB |= (1 << PIN6);
	}

	inline void negExcitation()
	{
		PORTB &= ~(1 << PIN7);
	}
};

#endif /* HX711_H_ */
