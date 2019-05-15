/*

 The MIT License (MIT)

 Copyright (c) 2015-2016 Douglas J. Bakkum

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/


#include "led.h"
#include "drivers/config/mcu.h"
#include "flags.h"
#ifndef TESTING
#include <gpio.h>
#include <delay.h>
#include <ioport.h>
#else


#define LED_0_PIN               0
#define IOPORT_PIN_LEVEL_LOW    1
#define IOPORT_PIN_LEVEL_HIGH   0


static void ioport_set_pin_level(int led, int level)
{
    (void)led;
    (void)level;
}


static int ioport_get_pin_level(int led)
{
    (void)led;
    return 0;
}


#endif

/**
 * Do not expose led_on(), led_off(), led_short(), or led_long()
 * via API (to prevent possible security problems during 2FA pairing).
 * These functions should only be used internally by other led commands!
 * Or in the case of led_off()/on() in touch.c and bootloader.c for legacy reasons.
 */
void led_on(void)
{
    ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_LOW);
}

void led_off(void)
{
    ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);
}

static void led_short(void)
{
    led_off();
    led_on();
    delay_ms(100);
    led_off();
    delay_ms(100);
}

static void led_long(void)
{
    led_off();
    led_on();
    delay_ms(300);
    led_off();
    delay_ms(300);
}

void led_toggle(void)
{
    ioport_set_pin_level(LED_0_PIN, !ioport_get_pin_level(LED_0_PIN));
}

/**
 * When a LONG_TOUCH is aborted.
 */
void led_abort(void)
{
    led_off();
    delay_ms(300);
    for (int i = 0; i < 6; i++) {
        led_short();
    }
}

/**
 * Long blink <code> times.
 * Only for 2FA mobile pairing!
 */
int led_code(uint8_t code)
{
    if (code > LED_MAX_CODE_BLINKS)
    {
        return DBB_ERROR;
    }

    uint8_t i;
    delay_ms(500);
    for (i = 0; i < code; i++) {
        led_toggle();
        delay_ms(300);
        led_toggle();
        delay_ms(300);
    }
    delay_ms(500);

    return DBB_OK;
}

/**
 * Indicate u2f related commands.
 * Authenticate and u2f_device_hijack()
 * u2f_device_wink()
 */
void led_u2f(void)
{
    for (int i = 0; i < 3; i++)
    {
        led_short();
        led_long();
        delay_ms(200);
    }
}

/**
 * Indicate firmware boot and successful long touch; Test the LED for 1 sec from device settings 
 */
void led_success(void)
{
    led_short();
    led_long();
    led_short();
}

/**
 * Indicate request to set device password or access hidden wallet.
 */
void led_password(void)
{
    led_short();
    led_short();
}

/**
 * Indicate request to sign a transaction.
 */
void led_sign(void)
{
    led_short();
}

/**
 * Warning blink for the user to make sure they triggered a potentially dangerous command.
 * Applies to the lock device, erase backup, reset/re-seed device, bootloader lock/unlock, 
 * and ecdh key re-generation commands.
 */
void led_warn(void)
{
    led_short();
    led_short();
    led_short();
}
