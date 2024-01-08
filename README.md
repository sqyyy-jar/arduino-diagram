# Arduino Temperature Diagram

This program shows the temperature curve on an OLED-display.

## Devices

These are the devices required to use the program.

- DHT-sensor
  - Current pin: 2
  - Current type: DHT11
- SSD1306-display
  - Current pins: SDA(20), SCL(21)
  - Current width: 128
  - Current height: 64

## Resources

These are the resources I used to create the project.

- Libraries
  - [DHT](https://github.com/adafruit/DHT-sensor-library)
  - [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- Code
  - [SSD1306 Example](https://github.com/adafruit/Adafruit_SSD1306/blob/master/examples/ssd1306_128x64_i2c/ssd1306_128x64_i2c.ino)
