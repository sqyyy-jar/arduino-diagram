#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Constants for the DHT11

#define DHT_PIN 2
#define DHT_TYPE DHT11

// Constants for the OLED Display
// The pins are SDA=20 and SCL=21

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Other constants

#define BUFFER_SIZE 128                                                             // Size of the buffer
#define TICK_DELAY 1000                                                             // Delay between ticks in ms
#define ENABLE_SERIAL false                                                         // Enable the serial connection
#define TEMPERATURE_MIN -10.0                                                       // Minimum temperature
#define TEMPERATURE_MAX 40.0                                                        // Maximum temperature
#define TEMPERATURE_RANGE (TEMPERATURE_MAX - TEMPERATURE_MIN)                       // Range of the possible temperatures

// Runtime variables

static DHT dht(DHT_PIN, DHT_TYPE, 1);                                               // DHT sensor
static Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);  // Display
static uint8_t data_buffer[BUFFER_SIZE] = {0};                                      // Ring buffer containing the temperatures
static size_t head = 0;                                                             // Head of the ring buffer
static size_t len = 0;                                                              // Length of the ring buffer

// Pushes a value into the ring buffer
void buffer_push(uint8_t value) {
    data_buffer[(head + len) % BUFFER_SIZE] = value;
    ++len;
    if (len > BUFFER_SIZE) {
        head = (head + 1) % BUFFER_SIZE;
        len = BUFFER_SIZE;
    }
}

// Retrieves a value from the ring buffer
uint8_t buffer_get(size_t index) {
    return data_buffer[(head + index) % BUFFER_SIZE];
}

void tick() {
    float temperature = dht.readTemperature(false);                                 // Read temperature in Celcius
    Serial.println(temperature);
    temperature = max(min(temperature, TEMPERATURE_MAX), TEMPERATURE_MIN);          // Fit the value in the range
    temperature -= TEMPERATURE_MIN;                                                 // Make value relative to 0
    Serial.println(temperature);
    uint8_t value = (uint8_t)((temperature / TEMPERATURE_RANGE) * 255.0);           // Fit the value into a byte
    Serial.println(value);
    buffer_push(value);                                                             // Push the value into the buffer
    if (ENABLE_SERIAL) {
        Serial.write(value);
    }
}

void update_display() {
    display.clearDisplay();
    for (size_t i = 0; i < DISPLAY_WIDTH && i < len; i++) {
        uint8_t value = buffer_get(i);
        uint8_t peak = (uint8_t)(((uint16_t)value * DISPLAY_HEIGHT) / 255L);
        for (int j = 0; j <= peak; j++) {                                           // Draw column
            display.drawPixel(i, j, SSD1306_WHITE);
        }
    }
    display.display();
}

void setup() {
    dht.begin();                                                                    // Start the DHT sensor
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {                     // Initialize the display
        Serial.println(F("SSD1306 allocation failed"));                             // Panic on failure
        for (;;);
    }
    display.clearDisplay();                                                         // Clear the display
    display.display();
    if (ENABLE_SERIAL) {
        Serial.begin(9600);
        while (!Serial) {;}                                                         // Wait for serial to connect
        uint8_t buf;
        while ((buf = Serial.read()) != 0x16) {;}                                   // Wait for SYN
        Serial.write(0x06);                                                         // Respond with ACK
    }
}

void loop() {
    tick();
    update_display();
    delay(TICK_DELAY);
}
