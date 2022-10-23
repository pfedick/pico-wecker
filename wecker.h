#ifndef __WECKER_H__
#define __WECKER_H__
#include <stdlib.h>

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
