// ============================================================
// Scanner.ino
// Простой сканер I2C-шины.
// ============================================================
#include <I2C_EASY.h>

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println();
    Serial.println(F("== I2C Scanner =="));

    i2c_init(100000);

    uint8_t found[16];
    uint8_t n = i2c_scan(found, 16);

    Serial.print(F("Found: "));
    Serial.println(n);

    for (uint8_t i = 0; i < n; i++) {
        Serial.print(F("  0x"));
        if (found[i] < 16) Serial.print('0');
        Serial.println(found[i], HEX);
    }
}

void loop() {}
