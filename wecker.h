#ifndef __WECKER_H__
#define __WECKER_H__
#include <stdlib.h>

#define DISPLAY_CLOCK_PIN 5
#define DISPLAY_DATA_PIN 4
#define RELAIS_PIN 18
#define AUDIO_PIN 28
#define ADC_VOLTAGE_PIN 29
#define BUZZER_PIN 2
#define DS3231_CLOCK_PIN 15
#define DS3231_DATA_PIN 14
#define LED_PIN PICO_DEFAULT_LED_PIN

#define ALARM_OFF_PIN 10
#define SET_PIN 11
#define UP_PIN 12
#define DOWN_PIN 13

class ButtonState
{
public:
    enum {
        AlarmOff=1,
        Settings=2,
        Up=4,
        Down=8

    };
    int state=0;
    ButtonState(int state);

    bool alarmOff() const;
    bool reset() const;
    bool usbBoot() const;
    bool settings() const;
    bool up() const;
    bool down() const;
    bool noButtonesPressed() const;

};

ButtonState getButtonState();
void waitForButtonsReleased();


#endif
