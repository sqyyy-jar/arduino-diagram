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
#define SCREEN_ADDRESS 0x3D

// Other constants

#define STARTUP_SECS 5                                                              // Time for startup in seconds
#define BUFFER_SIZE 128                                                             // Size of the buffer
#define PIXEL_TRANSLATION (256 / DISPLAY_HEIGHT)                                    // Translation between temperature and pixels
#define TICK_DELAY 2000                                                             // Delay between ticks in ms
#define ENABLE_SERIAL                                                               // Enable the serial connection
#define TEMPERATURE_MIN -10.0                                                       // Minimum temperature
#define TEMPERATURE_MAX 35.0                                                        // Maximum temperature
#define TEMPERATURE_BOUND (TEMPERATURE_MAX - TEMPERATURE_MIN)                       // Upper unsigned temperature bound

// Runtime variables

static DHT dht(DHT_PIN, DHT_TYPE);                                                  // DHT sensor
static Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);  // Display
static uint8_t data_buffer[BUFFER_SIZE] = {0};                                      // Ring buffer containing the temperatures
static size_t head = 0;                                                             // Head of the ring buffer
static size_t len = 0;                                                              // Length of the ring buffer

// Pushes a value into the ring buffer
void buffer_push(uint8_t value) {
    data_buffer[(head + len) % BUFFER_SIZE] = value;
    ++len;
    if (len > BUFFER_SIZE) {
        ++head;
        len = BUFFER_SIZE;
    }
}

// Retrieves a value from the ring buffer
uint8_t buffer_get(size_t index) {
    return data_buffer[(head + index) % BUFFER_SIZE];
}

void tick() {
    float temperature = dht.readTemperature(false);                                 // Read temperature in Celcius
    temperature = min(max(temperature, TEMPERATURE_MAX), TEMPERATURE_MIN);          // Fit the value in the range
    temperature -= TEMPERATURE_MIN;                                                 // Make the value positive
    uint8_t value = (uint8_t)(temperature / TEMPERATURE_BOUND * 255.0);             // Fit the value into a byte
    buffer_push(value);                                                             // Push the value into the buffer
}

void update_display() {
    display.clearDisplay();
    for (size_t i = 0; i < DISPLAY_WIDTH; i++) {
        uint8_t value = BUFFER_SIZE - i - 1;
        display.drawPixel(i, value / PIXEL_TRANSLATION, SSD1306_WHITE);
    }
}

void setup() {
    Serial.begin(9600);
    dht.begin();                                                                    // Start the DHT sensor
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {                     // Initialize the display
        Serial.println(F("SSD1306 allocation failed"));                             // Panic on failure
        for (;;);
    }
    display.display();
    delay(2000);
    display.fillRect(0, 0, 64, 32, SSD1306_WHITE);
    display.display();
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < STARTUP_SECS; i++) {                                        // Blink builtin LED
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}

void loop() {
    tick();
    update_display();
    delay(TICK_DELAY);
}
