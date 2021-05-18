#include <inttypes.h>
uint8_t hmacKey[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30}; //replace this with your hmac token encoded in hex, split by byte.
#define HMAC_KEY_LENGTH 10 //replace this with the length of the above array.
#define OTP_INTERVAL 30 //interval between new codes in seconds, usually 30