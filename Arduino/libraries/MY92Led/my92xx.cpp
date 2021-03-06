/*
MY92XX LED Driver Arduino library 3.0.1
Based on the C driver by MaiKe Labs
Copyright (c) 2016 - 2026 MaiKe Labs
Copyright (C) 2017 - 2018 Xose Pérez for the Arduino compatible library
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Attention !!!
Some functions have been extended for convenience by
Robert Patzke (April 2018)
*/

#include "my92xx.h"

#if ARDUINO_ARCH_ESP8266

    extern "C" {
        void os_delay_us(unsigned int);
    }

#elif ARDUINO_ARCH_AVR

    #define os_delay_us delayMicroseconds

#endif

void my92xx::_di_pulse(unsigned int times) {
	for (unsigned int i = 0; i < times; i++) {
		digitalWrite(_pin_di, HIGH);
		digitalWrite(_pin_di, LOW);
	}
}

void my92xx::_dcki_pulse(unsigned int times) {
	for (unsigned int i = 0; i < times; i++) {
		digitalWrite(_pin_dcki, HIGH);
		digitalWrite(_pin_dcki, LOW);
	}
}

void my92xx::_write(unsigned int data, unsigned char bit_length) {

    unsigned int mask = (0x01 << (bit_length - 1));

    for (unsigned int i = 0; i < bit_length / 2; i++) {
        digitalWrite(_pin_dcki, LOW);
        digitalWrite(_pin_di, (data & mask));
        digitalWrite(_pin_dcki, HIGH);
        data = data << 1;
        digitalWrite(_pin_di, (data & mask));
        digitalWrite(_pin_dcki, LOW);
        digitalWrite(_pin_di, LOW);
        data = data << 1;
    }

}

void my92xx::_set_cmd(my92xx_cmd_t command) {

	// ets_intr_lock();

    // TStop > 12us.
	os_delay_us(12);

    // Send 12 DI pulse, after 6 pulse's falling edge store duty data, and 12
	// pulse's rising edge convert to command mode.
	_di_pulse(12);

    // Delay >12us, begin send CMD data
	os_delay_us(12);

    // Send CMD data
    unsigned char command_data = *(unsigned char *) (&command);
    for (unsigned char i=0; i<_chips; i++) {
        _write(command_data, 8);
    }

	// TStart > 12us. Delay 12 us.
	os_delay_us(12);

    // Send 16 DI pulse，at 14 pulse's falling edge store CMD data, and
	// at 16 pulse's falling edge convert to duty mode.
	_di_pulse(16);

    // TStop > 12us.
	os_delay_us(12);

    // ets_intr_unlock();

}

void my92xx::_send() {

    #ifdef DEBUG_MY92XX
        DEBUG_MSG_MY92XX("[MY92XX] Refresh: %s (", _state ? "ON" : "OFF");
        for (unsigned char channel = 0; channel < _channels; channel++) {
            DEBUG_MSG_MY92XX(" %d", _value[channel]);
        }
        DEBUG_MSG_MY92XX(" )\n");
    #endif

    unsigned char bit_length = 8;
	switch (_command.bit_width) {
		case MY92XX_CMD_BIT_WIDTH_16:
			bit_length = 16;
			break;
		case MY92XX_CMD_BIT_WIDTH_14:
			bit_length = 14;
			break;
		case MY92XX_CMD_BIT_WIDTH_12:
			bit_length = 12;
			break;
		case MY92XX_CMD_BIT_WIDTH_8:
			bit_length = 8;
			break;
		default:
			bit_length = 8;
			break;
	}

	// ets_intr_lock();

    // TStop > 12us.
	os_delay_us(12);

    // Send color data
    for (unsigned char channel = 0; channel < _channels; channel++) {
        _write(_state ? _value[channel] : 0, bit_length);
	}

	// TStart > 12us. Ready for send DI pulse.
	os_delay_us(12);

	// Send 8 DI pulse. After 8 pulse falling edge, store old data.
	_di_pulse(8);

	// TStop > 12us.
	os_delay_us(12);

	// ets_intr_unlock();

}

// -----------------------------------------------------------------------------

unsigned char my92xx::getChannels() {
    return _channels;
}

bool my92xx::setChannel(unsigned char channel, unsigned int value)
{
  if(channel >= _channels) return(false);
  if(_value[channel] == value) return(false);

  _value[channel] = value;
  return(true);
}

unsigned int my92xx::getChannel(unsigned char channel) {
    if (0 <= channel && channel < _channels) {
        return _value[channel];
    }
    return 0;
}

bool my92xx::getState() {
    return _state;
}

void my92xx::setState(bool state) {
    _state = state;
}

void my92xx::update() {
    _send();
}

// -----------------------------------------------------------------------------

my92xx::my92xx(my92xx_model_t model, unsigned char chips, unsigned char di, unsigned char dcki, my92xx_cmd_t command) {

    _model = model;
    _chips = chips;
	_pin_di = di;
	_pin_dcki = dcki;
    _command = command;

    // Init channels
    if (_model == MY92XX_MODEL_MY9291) {
        _channels = 4 * _chips;
    } else if (_model == MY92XX_MODEL_MY9231) {
        _channels = 3 * _chips;
    }
    _value = new uint16_t[_channels];
    for (unsigned char i=0; i<_channels; i++) {
        _value[i] = 0;
    }

    // Init GPIO
	pinMode(_pin_di, OUTPUT);
	pinMode(_pin_dcki, OUTPUT);
	digitalWrite(_pin_di, LOW);
	digitalWrite(_pin_dcki, LOW);

	// Clear all duty register
   	_dcki_pulse(32 * _chips);

    // Send init command
	_set_cmd(command);

    DEBUG_MSG_MY92XX("[MY92XX] Initialized\n");

}
