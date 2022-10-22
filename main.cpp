#include "pico/stdlib.h"
#include "stdlib.h"
#include "stdio.h"
#include <unistd.h>
#include "pico/types.h"
#include "pico/time.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "hardware/timer.h"


#include "PicoTM1637.h"
#include "pico/util/datetime.h"

#include "ds3231.h"


#define CLOCK_PIN_CLOCK 5
#define CLOCK_PIN_DATA 4
#define RELAIS_PIN 18
#define AUDIO_PIN 28
#define ADC_VOLTAGE_PIN 29
#define BUZZER_PIN 2

#define ALARM_OFF_PIN 10
#define SET_PIN 11
#define UP_PIN 12
#define DOWN_PIN 13


void enter_reset()
{
    TM1637_display_word("boot", true);
    while (1) {
        if (gpio_get(UP_PIN) != 0 || gpio_get(DOWN_PIN) != 0) break;
    }
    TM1637_display_word("----", true);
    sleep_ms(100);
    reset_usb_boot(0, 0);
}

void main_loop()
{
    TM1637_display_word("run ", true);
    datetime_t t;
    uint64_t last_second=time_us_64() / 1000000;
    printf("start second: %d\n", (int)last_second);
    int last_state=0;
    while (1) {
        time_t now=time_us_64() / 1000000;
        if (now > last_second) {
            printf("next second: %d\n", (int)now);
            last_second=now;
            last_state=!last_state;
            if (rtc_get_datetime(&t)) {
                printf("Time: %02d:%02d:%02d\n", t.hour, t.min, t.sec);
                TM1637_display_both(t.hour, t.min, true);
            } else {
                TM1637_display_word("err1", true);
            }
            TM1637_set_colon(last_state);
        }
        if (gpio_get(UP_PIN) == 0 && gpio_get(DOWN_PIN) == 0) enter_reset();

        sleep_ms(10);


    }

}

int main() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(RELAIS_PIN);
    gpio_set_dir(RELAIS_PIN, GPIO_OUT);
    gpio_put(RELAIS_PIN, 0);
    gpio_init(AUDIO_PIN);
    gpio_set_dir(AUDIO_PIN, GPIO_OUT);
    gpio_put(AUDIO_PIN, 0);

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    //malloc_stats();
    TM1637_init(CLOCK_PIN_CLOCK, CLOCK_PIN_DATA);
    TM1637_clear();
    TM1637_set_brightness(0); // max value, default is 0
    TM1637_display_word("init", true);

    adc_init();
    gpio_init(ADC_VOLTAGE_PIN);
    gpio_set_dir(ADC_VOLTAGE_PIN, GPIO_IN);
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_pull_down(25);
    gpio_put(25, 1);

    // Knoepfe
    gpio_init(ALARM_OFF_PIN);
    gpio_set_dir(ALARM_OFF_PIN, GPIO_IN);
    gpio_pull_up(ALARM_OFF_PIN);

    gpio_init(SET_PIN);
    gpio_set_dir(SET_PIN, GPIO_IN);
    gpio_pull_up(SET_PIN);

    gpio_init(UP_PIN);
    gpio_set_dir(UP_PIN, GPIO_IN);
    gpio_pull_up(UP_PIN);

    gpio_init(DOWN_PIN);
    gpio_set_dir(DOWN_PIN, GPIO_IN);
    gpio_pull_up(DOWN_PIN);


    adc_gpio_init(ADC_VOLTAGE_PIN);
    adc_select_input(3);

    sleep_ms(1000);
    printf("Start\n");

    DS3231 ds3231(i2c1, 15, 14, 0x68);
    //TM1637_display_word("time", true);

    char datetime_buf[256];
    char* datetime_str = &datetime_buf[0];
    rtc_init();
    datetime_t t ={
            .year  = 2022,
            .month = 10,
            .day   = 22,
            .dotw  = 6, // 0 is Sunday, so 5 is Friday
            .hour  = 19,
            .min   = 13,
            .sec   = 00
    };
    //ds3231.setTime(t);
    ds3231.getTime(t);
    rtc_set_datetime(&t);

    main_loop();
    return 0;
}

#ifdef OLDCODE
    /*

    char cbuffer[20];

    int state=0;
    while (1) {
        ds3231.getTime(t);
        printf("%02d:%02d:%02d\n", t.hour, t.min, t.sec);
        /*
        float voltage=(float)adc_read() * 3.0f * 3.3f / 65535.0f;
        printf("voltage: %0.3f\n", voltage);
        sprintf(cbuffer, "%04d", (int)(voltage * 1000));
        printf(">>%s<<\n", cbuffer);
        //TM1637_display_word(cbuffer, true);
        */

if (rtc_get_datetime(&t)) {
    TM1637_display_both(t.hour, t.min, true);
}

sleep_ms(1000);
state=!state;
//gpio_put(AUDIO_PIN, state);
//gpio_put(LED_PIN, state);
//gpio_put(RELAIS_PIN, state);
if (gpio_get(UP_PIN) == 0 && gpio_get(DOWN_PIN) == 0) enter_reset();
//gpio_put(BUZZER_PIN, state);
    }

}
#endif
