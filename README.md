# pico-2fa-token

Turn your raspberri Pi Pico into a time-based 2fa authenticator! Why, I hear you ask? Well why not?

Required applications and software-side:
-----
- git
- An HMAC key encoded in hexadecimal and split by byte 
Example (key in ASCII is "1234567890"):
```c
uint8_t hmacKey[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30}
```
Note: most websites provide the hmac key encoded in base32, for example in the form of ```abcd efgh hijk lmno``` or ```ABCDEFGHHIJKLMNO```. You can use [this site](https://cryptii.com/pipes/base32-to-hex) to convert the base32 string to hex (make sure you upcase it first if it's lowercase and remove all whitespaces).

Required hardware:
-----
- A Raspberry Pi pico and power supply
- 3x momentary switches
- A 3.3v character LCD display directly connected to an i2c adapter **or** a 5v LCD connected per the diagram to 5v power supply and the pico
- An i2c character LCD adapter for use with above LCD

Wiring diagram available as [png](Schematic_pico-2fa-token.png) or as a [Fritzing sketch](Schematic_pico-2fa-token.fzz)



Setup:
 -----
- ```git clone``` the repository to the desired location

- To set up the project, make sure you have the pico sdk installed (if you're using VSCode make sure that it is present in the directory specified in PICO_SDK_PATH in [settings.json](.vscode/settings.json) (same root folder as project by default)). You can find out how to do this [here](https://www.raspberrypi.org/documentation/rp2040/getting-started/).

- Run ```git submodule update --init``` to install the required library.

- Make a copy of ```token.example.h``` and rename it to ```token.h```, there replace the ```hmacKey[]``` array contents with your token, and ```HMAC_KEY_LENGTH``` with the length of your key and ```OTP_INTERVAL``` with the interval between generating OTP codes (default 30 seconds).

- Open main.c and edit the designated area to fit your time zone, LCD size and GPIO pin setup.
- Open lcd.c and edit addr in line one to your i2c adapter's address (default is 0x3f)

- Build the project and copy main.uf2 to the pico by plugging it in while holding the BOOTSEL button.