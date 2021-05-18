#include <stdio.h>
#include <time.h>
#include <string.h>
#include "token.h"
#include "lcd.h"
#include "TOTP-MCU/TOTP.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"

//----------- DO NOT EDIT ABOVE THIS LINE (unless you know what you're doing)

#define BUTTON_L_PIN 15 // GPIO number for left button
#define BUTTON_SEL_PIN 14 //GPIO number for select button
#define BUTTON_R_PIN 13 //GPIO number for right button
#define MAX_LINES 2 //Rows of LCD
#define MAX_CHARS 16 //Columns of LCD
#define TZ_OFFSET 3 //time zone offset, modify to your needs

//----------- DO NOT EDIT BELOW THIS LINE (unless you know what you're doing) ------------

char lcd_content[MAX_CHARS]; //variable for holding LCD text to be printed

int get_number(int min, int def, int max, char label[6]){ //used to ask for time settings on startup
    int curr_num = def;
    lcd_clear();
    lcd_set_cursor(0,0);
    sprintf(lcd_content, "%s:", label);
    lcd_string(lcd_content);
    lcd_set_cursor(1,0);
    sprintf(lcd_content, "%d", curr_num);
    lcd_string(lcd_content);
    while (gpio_get(BUTTON_SEL_PIN) == 1) { //inverted because it's pulled up internally
        if (gpio_get(BUTTON_L_PIN) == 0) curr_num--; //inverted because it's pulled up internally
        else if (gpio_get(BUTTON_R_PIN) == 0) curr_num++; //inverted because it's pulled up internally
        if (curr_num < min) curr_num = max;
        if (curr_num > max) curr_num = min; 

        if ((gpio_get(BUTTON_L_PIN) == 0) || (gpio_get(BUTTON_R_PIN) == 0)) {
            lcd_cr(1, MAX_CHARS);
            sprintf(lcd_content, "%d", curr_num);
            lcd_string(lcd_content);
        }
        
        sleep_ms(150); 
    } return curr_num;
}

int main() {
    //init IO

    stdio_init_all();
    i2c_init(i2c_default, 100 * 1000);
    //use pins 4 and 5 for SDA and SCL
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); 
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    gpio_init(BUTTON_L_PIN);
    gpio_init(BUTTON_R_PIN);
    gpio_init(BUTTON_SEL_PIN);
    gpio_pull_up(BUTTON_L_PIN);
    gpio_pull_up(BUTTON_R_PIN);
    gpio_pull_up(BUTTON_SEL_PIN);
    gpio_set_dir(BUTTON_L_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_R_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_SEL_PIN, GPIO_IN);

    lcd_init();
    lcd_set_cursor(0, 0);
    lcd_string("TOTP Token");
    lcd_set_cursor(1,0);
    lcd_string("By Devnol");
    sleep_ms(1000);
    

    //obtain date and time

        int year = get_number(0, 2021, 4095, "Year");
        sleep_ms(400);

        int month = get_number(1, 1, 12, "Month");
        sleep_ms(400);
        
        //determine limit of days of month
        int maxday;
        switch(month) {
            case 4:
            case 6:
            case 9:
            case 11:
                maxday = 30;
                break;
            case 2:
                if ( year % 4 ) maxday = 28;
                else maxday = 29;
                break;
            default: 
                maxday = 31;
        }
            
        
        int day = get_number(1, 1, maxday, "Day");
        sleep_ms(400);

        int hour = get_number(0, 0, 23, "Hour") - TZ_OFFSET;
        sleep_ms(400);

        int minute = get_number(0, 0, 59, "Min");
        sleep_ms(400);

        int second = get_number(0, 0, 59, "Sec");
        sleep_ms(400);

    char datetime_buf[256];
    char *datetime_str = &datetime_buf[0];

    datetime_t t = {
        .year = year,
        .month = month,
        .day = day,
        .hour = hour,
        .min = minute,
        .sec = second
     };

     //start rtc
     rtc_init();
     rtc_set_datetime(&t);

    struct tm tm;
    time_t epoch;

    //instantiate TOTP library
    TOTP(hmacKey, HMAC_KEY_LENGTH, OTP_INTERVAL);
    lcd_clear();
    lcd_set_cursor(0,0);
    //main Token generation loop
    while (true) {
        rtc_get_datetime(&t);
        datetime_to_str(datetime_str, sizeof(datetime_buf), &t);
        if ( strptime(datetime_str, "%A %d %b %H:%M:%S %Y", &tm) != NULL ) {
            epoch = mktime(&tm);
        }
        strptime(datetime_str, "%A %d %b %H:%M:%S %Y", &tm);
        uint32_t newCode = getCodeFromTimestamp(epoch);
        lcd_cr(0, MAX_CHARS);    
        sprintf(lcd_content, "Token: %d", newCode);
        lcd_string(lcd_content);
        sleep_ms(5000);
    }
    return 0;    
}
