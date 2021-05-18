#include <inttypes.h>

void i2c_write_byte(uint8_t val);
void lcd_toggle_enable(uint8_t val);
void lcd_send_byte(uint8_t val, int mode);
void lcd_clear(void);
void lcd_cr(int line, int line_length);
void lcd_set_cursor(int line, int position);
static void inline lcd_char(char val);
void lcd_string(const char *s);
void lcd_init();