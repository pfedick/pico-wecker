
#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
#include "stdio.h"
#include "pico/types.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "ds3231.h"

// convert BCD to number
static inline uint8_t bcd2num(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

// convert number to BCD
static inline uint8_t num2bcd(uint8_t num)
{
    return ((num / 10) * 16) + (num % 10);
}


DS3231::DS3231()
{
    i2c_port=i2c0;
    i2c_scl=21;
    i2c_sda=20;
    addr=0x57;
    //i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}

DS3231::DS3231(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr)
{
    this->i2c_port=i2c0;
    this->i2c_scl=21;
    this->i2c_sda=20;
    this->addr=0x57;
    init(i2c_port, i2c_scl, i2c_sda, addr);
}

void DS3231::init(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr)
{
    this->i2c_port=i2c_port;
    this->i2c_scl=i2c_scl;
    this->i2c_sda=i2c_sda;
    this->addr=addr;

    i2c_init(i2c_port, 40000);
    gpio_set_function(i2c_sda, GPIO_FUNC_I2C);
    gpio_set_function(i2c_scl, GPIO_FUNC_I2C);
    gpio_pull_up(i2c_sda);
    gpio_pull_up(i2c_scl);

}

void DS3231::setTime(const datetime_t& time)
{
    uint8_t buf[2];
    // set second
    buf[0]=0x00;
    buf[1]=num2bcd(time.sec);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

    // set minute
    buf[0]=0x01;
    buf[1]=num2bcd(time.min);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

    // set hour
    buf[0]=0x02;
    buf[1]=num2bcd(time.hour);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

    // set weekday
    buf[0]=0x03;
    buf[1]=num2bcd(time.dotw + 1);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

    // set day
    buf[0]=0x04;
    buf[1]=num2bcd(time.day);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

    // set month
    buf[0]=0x05;
    buf[1]=num2bcd(time.month);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

    // set year
    buf[0]=0x06;
    buf[1]=num2bcd(time.year - 2000);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);

}

void DS3231::getTime(datetime_t& time)
{
    uint8_t buf[8];
    memset(buf, 0, 8);
    uint8_t val = 0x00;
    if (1 != i2c_write_blocking(i2c_port, addr, &val, 1, true)) {
        printf("ERROR: i2c_write_blocking failed\n");
    }
    int bytes=i2c_read_blocking(i2c_port, addr, buf, 7, false);
    time.sec=bcd2num(buf[0]);
    time.min=bcd2num(buf[1]);
    time.hour=bcd2num(buf[2]);
    time.dotw=bcd2num(buf[3]) - 1;
    time.day=bcd2num(buf[4]);
    time.month=bcd2num(buf[5]);
    time.year=bcd2num(buf[6]) + 2000;
}

float DS3231::getTemperature() const
{
    uint8_t val = 0x11;
    if (1 != i2c_write_blocking(i2c_port, addr, &val, 1, true)) {
        printf("ERROR: i2c_write_blocking failed\n");
        return 0.0f;
    }
    uint8_t buf[]={ 0,0 };
    if (i2c_read_blocking(i2c_port, addr, buf, 2, false) != 2) {
        printf("ERROR: i2c_read_blocking failed\n");
        return 0.0f;
    }
    /*Upper: 0xfe, Lower: 0xc0, v1=0x7ec0, v2=0xfffffe05
    */
    int16_t v1=(buf[0] << 8) | buf[1];
    int16_t v2=v1 / 64;
   // printf("Upper: 0x%x, Lower: 0x%x, v1=0x%x, v2=0x%x\n", buf[0], buf[1], v1, v2);


    return (float)v2 * 0.25f;
}
