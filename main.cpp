#include "pico/stdlib.h"
#include "stdlib.h"
#include "stdio.h"
#include <unistd.h>
#include "pico/types.h"
#include "pico/time.h"

#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "PicoTM1637.h"
#include "pico/util/datetime.h"

#include "ds3231.h"


#define CLOCK_PIN_CLOCK 21
#define CLOCK_PIN_DATA 20



int main() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    sleep_ms(1000);
    printf("Start\n");
    //malloc_stats();
    TM1637_init(CLOCK_PIN_CLOCK, CLOCK_PIN_DATA);
    TM1637_clear();
    TM1637_set_brightness(7); // max value, default is 0

    TM1637_display_word("init", true);

    DS3231 ds3231(i2c1, 15, 14, 0x68);
    //TM1637_display_word("time", true);

    char datetime_buf[256];
    char* datetime_str = &datetime_buf[0];
    rtc_init();
    datetime_t t ={
            .year  = 2022,
            .month = 10,
            .day   = 19,
            .dotw  = 3, // 0 is Sunday, so 5 is Friday
            .hour  = 22,
            .min   = 9,
            .sec   = 00
    };
    //ds3231.setTime(t);
    ds3231.getTime(t);
    rtc_set_datetime(&t);

    TM1637_display_word("run1", true);
    while (1) {
        ds3231.getTime(t);
        printf("%02d:%02d:%02d\n", t.hour, t.min, t.sec);
        if (rtc_get_datetime(&t)) {
            TM1637_display_both(t.hour, t.min, true);
        }
        sleep_ms(1000);
    }

}
