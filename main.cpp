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
#include "hardware/watchdog.h"


#include "PicoTM1637.h"
#include "pico/util/datetime.h"

#include "ds3231.h"
#include "wecker.h"

#define IDLE_TIMEOUT 20000

static int display_hue=0;

ButtonState::ButtonState(int s)
    :state(s)
{

}

bool ButtonState::reset() const
{
    if (state == (ButtonState::Settings | ButtonState::Up)) return true;
    return false;
}

bool ButtonState::usbBoot() const
{
    if (state == (ButtonState::Settings | ButtonState::Down)) return true;
    return false;
}

bool ButtonState::settings() const
{
    if (state == ButtonState::Settings) return true;
    return false;
}

bool ButtonState::alarmOff() const
{
    if (state == ButtonState::AlarmOff) return true;
    return false;
}

bool ButtonState::up() const
{
    if (state == ButtonState::Up) return true;
    return false;
}

bool ButtonState::down() const
{
    if (state == ButtonState::Down) return true;
    return false;
}

bool ButtonState::noButtonesPressed() const
{
    if (state == 0) return true;
    return false;
}

ButtonState getButtonState()
{
    int state=0;
    if (gpio_get(ALARM_OFF_PIN) == 0) state|=ButtonState::AlarmOff;
    if (gpio_get(UP_PIN) == 0) state|=ButtonState::Up;
    if (gpio_get(DOWN_PIN) == 0) state|=ButtonState::Down;
    if (gpio_get(SET_PIN) == 0) state|=ButtonState::Settings;
    return ButtonState(state);
}

void waitForButtonsReleased()
{
    while (1) {
        ButtonState s=getButtonState();
        if (s.noButtonesPressed()) return;
        sleep_ms(10);
    }
}

void enter_reset()
{
    TM1637_display_word("boot", true);
    waitForButtonsReleased();
    TM1637_display_word("----", true);
    watchdog_enable(1, 1);
    while (1);
}

void enter_usb_boot()
{
    TM1637_display_word("usb ", true);
    waitForButtonsReleased();
    TM1637_display_word("ProG", true);
    sleep_ms(100);
    reset_usb_boot(0, 0);
}

void set_display_hue()
{
    char buffer[5];
    waitForButtonsReleased();
    uint64_t idle_timeout=IDLE_TIMEOUT + (time_us_64() / 1000);
    while (1) {
        uint64_t now_ms=time_us_64() / 1000;
        sprintf(buffer, "hUE%01d", display_hue);
        TM1637_display_word(buffer, false);
        ButtonState buttons=getButtonState();
        if (buttons.settings()) break;
        else if (buttons.up() && display_hue < 7) {
            display_hue++;
            TM1637_set_brightness(display_hue);
            sleep_ms(200);
            idle_timeout=now_ms + IDLE_TIMEOUT;
        } else if (buttons.down() && display_hue > 0) {
            display_hue--;
            TM1637_set_brightness(display_hue);
            sleep_ms(200);
            idle_timeout=now_ms + IDLE_TIMEOUT;
        }
        if (now_ms > idle_timeout) break;
        sleep_ms(10);
    }
    waitForButtonsReleased();
}

void test_loop()
{
    waitForButtonsReleased();
    int state=0;
    int hue=0;
    uint64_t last_second=time_us_64() / 1000000;
    while (1) {
        uint64_t now_ms=time_us_64() / 1000;
        time_t now_seconds=now_ms / 1000;
        if (now_seconds > last_second) {
            last_second=now_seconds;
            state=!state;
            hue++;
            if (hue > 7) hue=0;
            TM1637_set_brightness(hue);
            gpio_put(RELAIS_PIN, state);
            gpio_put(BUZZER_PIN, state);
            if (state) TM1637_display_word("tESt", true);
            else TM1637_display_word("----", true);
        }
        ButtonState buttons=getButtonState();
        if (buttons.settings()) break;
        sleep_ms(10);
    }
    TM1637_set_brightness(display_hue);
    gpio_put(RELAIS_PIN, 0);
    gpio_put(BUZZER_PIN, 0);

    waitForButtonsReleased();
}


static const char* menue_array[] ={
    "SEt ", // 0
    "UHr ", // 1
    "ALAr", // 2
    "OnOF", // 3
    "diSP", // 4
    "tESt", // 5
    "-00-"
};

void settings_loop()
{
    TM1637_display_word("SEt ", true);
    waitForButtonsReleased();
    int current_option=0;
    //int max=sizeof(menue_array) / sizeof(const char*);
    int max=5;
    uint64_t idle_timeout=IDLE_TIMEOUT + (time_us_64() / 1000);
    while (1) {
        TM1637_display_word(menue_array[current_option], true);
        uint64_t now_ms=time_us_64() / 1000;
        ButtonState buttons=getButtonState();
        if (buttons.up() && current_option > 0) {
            current_option--;
            TM1637_display_word(menue_array[current_option], true);
            sleep_ms(200);
            idle_timeout=now_ms + IDLE_TIMEOUT;
        } else if (buttons.down() && current_option < max) {
            current_option++;
            TM1637_display_word(menue_array[current_option], true);
            sleep_ms(200);
            idle_timeout=now_ms + IDLE_TIMEOUT;
        } else if (buttons.down()) {
            current_option=0;
            TM1637_display_word(menue_array[current_option], true);
            sleep_ms(200);
            idle_timeout=now_ms + IDLE_TIMEOUT;
        } else if (buttons.up() && current_option == 0) {
            current_option=max;
            TM1637_display_word(menue_array[current_option], true);
            sleep_ms(200);
            idle_timeout=now_ms + IDLE_TIMEOUT;
        } else if (buttons.settings()) {
            if (current_option == 0) break;
            if (current_option == 4) set_display_hue();
            if (current_option == 5) test_loop();
            now_ms=time_us_64() / 1000;
            idle_timeout=now_ms + IDLE_TIMEOUT;

        } else if (buttons.usbBoot()) enter_usb_boot();
        else if (buttons.reset()) enter_reset();
        else {
            sleep_ms(10);
        }
        if (now_ms > idle_timeout) break;
    }
    TM1637_display_word("----", true);
    waitForButtonsReleased();

}

void main_loop()
{
    //TM1637_display_word("run ", true);
    datetime_t t;
    uint64_t last_second=time_us_64() / 1000000;
    printf("main_loop start second: %d\n", (int)last_second);
    int last_state=0;
    uint64_t settings_delay=0;
    while (1) {
        uint64_t now_ms=time_us_64() / 1000;
        time_t now_seconds=now_ms / 1000;
        if (now_seconds > last_second) {
            //printf("next second: %d\n", (int)now_seconds);
            last_second=now_seconds;
            last_state=!last_state;
            if (rtc_get_datetime(&t)) {
                //printf("Time: %02d:%02d:%02d\n", t.hour, t.min, t.sec);
                TM1637_display_both(t.hour, t.min, true);
            } else {
                TM1637_display_word("Err1", true);
            }
            TM1637_set_colon(last_state);
        }
        //printf("now_ms: %d, settings_delay=%d\n", (int)now_ms, (int)settings_delay);
        ButtonState buttons=getButtonState();
        if (buttons.usbBoot()) enter_usb_boot();
        else if (buttons.reset()) enter_reset();
        else if (buttons.settings()) {
            if (settings_delay == 0) settings_delay=now_ms + 1000;
            else if (now_ms > settings_delay) {
                settings_loop();
                settings_delay=0;
            }
        } else {
            settings_delay=0;
        }

        sleep_ms(10);
    }
    printf("main_loop ended\n");

}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
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
    TM1637_init(DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN);
    TM1637_clear();
    TM1637_set_brightness(display_hue); // max value, default is 0
    TM1637_display_word("InIt", true);

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

    DS3231 ds3231(i2c1, DS3231_CLOCK_PIN, DS3231_DATA_PIN, 0x68);
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
    gpio_put(LED_PIN, 0);
    main_loop();
    return 0;
}
