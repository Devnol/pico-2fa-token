#include <stdio.h>
#include <time.h>
#include "token.h"
#include "TOTP-MCU/TOTP.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "hardware/gpio.h"
#include "hardware/rtc.h"

//----------- DO NOT EDIT ABOVE THIS LINE (unless you know what you're doing)

#define BUTTON_L_PIN 15 // GPIO number for left button
#define BUTTON_SEL_PIN 14 //GPIO number for select button
#define BUTTON_R_PIN 13 //GPIO number for right button
#define TZ_OFFSET 3 //time zone offset, modify to your needs
#define OTP_INTERVAL 30 //interval between new codes in seconds, usually 30

//----------- DO NOT EDIT BELOW THIS LINE (unless you know what you're doing) ------------

int get_number(int min, int def, int max, char label[6]){ //used to ask for time settings on startup
    int curr_num = def;
    printf("\n");
    while (!gpio_get(BUTTON_SEL_PIN)) {
        printf("\r");
        printf("%s: %d", label, curr_num);
        if (gpio_get(BUTTON_L_PIN)) curr_num--;
        else if (gpio_get(BUTTON_R_PIN)) curr_num++;
        
        if (curr_num < min) curr_num = max;
        if (curr_num > max) curr_num = min;
        sleep_ms(150); 
    } return curr_num;
}

int main() {
    stdio_init_all();
    //init buttons
    gpio_init(BUTTON_L_PIN);
    gpio_init(BUTTON_R_PIN);
    gpio_init(BUTTON_SEL_PIN);
    gpio_pull_up(BUTTON_L_PIN);
    gpio_pull_up(BUTTON_R_PIN);
    gpio_pull_up(BUTTON_SEL_PIN);
    gpio_set_dir(BUTTON_L_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_R_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_SEL_PIN, GPIO_IN);

    ////obtain date and time

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

    //main Token generation loop
    while (true) {
        rtc_get_datetime(&t);
        datetime_to_str(datetime_str, sizeof(datetime_buf), &t);
        if ( strptime(datetime_str, "%A %d %b %H:%M:%S %Y", &tm) != NULL ) {
            epoch = mktime(&tm);
        }
        strptime(datetime_str, "%A %d %b %H:%M:%S %Y", &tm);
        uint32_t newCode = getCodeFromTimestamp(epoch);
        printf("Token: %d\n", newCode);
        sleep_ms(1000);
    }
    return 0;    
}
