// ============================================================
// Scanner_Freq.ino
// I2C-сканер + проверка максимальной рабочей частоты
// для каждого устройства на шине.
// ============================================================
// Спецификация:
//   - 100 пингов на каждую частоту
//   - Задержка между пингами: 10 мкс
//   - Частоты: 400 / 200 / 100 / 50 кГц
//   - Порядок частот: от высокой к низкой
//   - Порядок устройств: по адресам
//   - Формат: полный (все частоты + максимум)
//   - Язык: RU (по умолчанию), CN, EN
// ============================================================
#include <I2C_EASY.h>

// ============================================================
// ВЫБОР ЯЗЫКА (раскомментировать нужный)
// ============================================================
#define I2C_LANG_RU   0   // 🇷🇺 Русский (по умолчанию)
#define I2C_LANG_CN   1   // 🇨🇳 中文
#define I2C_LANG_EN   2   // 🇺🇸 English

#ifndef TEST_LANG
#define TEST_LANG   I2C_LANG_RU
#endif

// ============================================================
// ПАРАМЕТРЫ ТЕСТА
// ============================================================
#define TESTS_PER_FREQ  100           // Пингов на частоту
#define DELAY_US        10            // Задержка между пингами (мкс)

// Частоты для теста (от высокой к низкой)
static const uint32_t FREQS[] = {
    400000UL,   // 400 кГц
    200000UL,   // 200 кГц
    100000UL,   // 100 кГц
    50000UL     // 50  кГц
};
static const uint8_t NUM_FREQS = sizeof(FREQS) / sizeof(FREQS[0]);

// ============================================================
// СТРОКИ ПО ЯЗЫКАМ
// ============================================================
#if TEST_LANG == I2C_LANG_RU
    #define TXT_HEADER      "== I2C Scanner + Тест частоты =="
    #define TXT_CPU         "Частота CPU: "
    #define TXT_SCANNING    "Сканирование шины I2C..."
    #define TXT_FOUND       "Найдено устройств: "
    #define TXT_NONE        "Устройства не найдены."
    #define TXT_MAX         "  → Максимум: "
    #define TXT_DONE        "Тест завершён."
    #define TXT_PASS        "ПРОЙДЕН"
    #define TXT_FAIL        "СБОЙ"
    #define TXT_UNKNOWN     "Неизвестно"
    #define TXT_KHZ         " кГц"
#elif TEST_LANG == I2C_LANG_CN
    #define TXT_HEADER      "== I2C 扫描仪 + 频率测试 =="
    #define TXT_CPU         "CPU 频率: "
    #define TXT_SCANNING    "扫描 I2C 总线..."
    #define TXT_FOUND       "发现设备: "
    #define TXT_NONE        "未发现设备。"
    #define TXT_MAX         "  → 最大值: "
    #define TXT_DONE        "测试完成。"
    #define TXT_PASS        "通过"
    #define TXT_FAIL        "失败"
    #define TXT_UNKNOWN     "未知"
    #define TXT_KHZ         " kHz"
#else
    #define TXT_HEADER      "== I2C Scanner + Frequency Test =="
    #define TXT_CPU         "CPU frequency: "
    #define TXT_SCANNING    "Scanning I2C bus..."
    #define TXT_FOUND       "Found devices: "
    #define TXT_NONE        "No devices found."
    #define TXT_MAX         "  -> Max: "
    #define TXT_DONE        "Test complete."
    #define TXT_PASS        "OK"
    #define TXT_FAIL        "FAIL"
    #define TXT_UNKNOWN     "Unknown"
    #define TXT_KHZ         " kHz"
#endif

// ============================================================
// ТАБЛИЦА КАТЕГОРИЙ
// ============================================================
#if TEST_LANG == I2C_LANG_RU
    static const char* const CAT_NAMES[] = {
        "Расширитель портов",
        "Дисплей",
        "АЦП/Датчик",
        "EEPROM",
        "RTC",
        "Датчик среды",
        "Неизвестно"
    };
#elif TEST_LANG == I2C_LANG_CN
    static const char* const CAT_NAMES[] = {
        "扩展器",
        "显示器",
        "ADC/传感器",
        "EEPROM",
        "RTC",
        "环境传感器",
        "未知"
    };
#else
    static const char* const CAT_NAMES[] = {
        "Port expander",
        "Display",
        "ADC/Sensor",
        "EEPROM",
        "RTC",
        "Env sensor",
        "Unknown"
    };
#endif

// ============================================================
// ОПРЕДЕЛЕНИЕ КАТЕГОРИИ ПО АДРЕСУ
// ============================================================
static uint8_t category_index(uint8_t addr) {
    if (addr >= 0x20 && addr <= 0x27) return 0;
    if (addr == 0x3C || addr == 0x3D) return 1;
    if (addr >= 0x48 && addr <= 0x4B) return 2;
    if (addr >= 0x50 && addr <= 0x57) return 3;
    if (addr == 0x68 || addr == 0x69) return 4;
    if (addr == 0x76 || addr == 0x77) return 5;
    return 6;
}

// ============================================================
// ТЕСТ ОДНОЙ ЧАСТОТЫ
// Возвращает 1 = ПРОЙДЕН (все пинги успешны), 0 = СБОЙ
// ============================================================
static uint8_t test_freq(uint8_t addr, uint32_t freq) {
    i2c_set_freq(freq);

    for (uint8_t i = 0; i < TESTS_PER_FREQ; i++) {
        if (!i2c_ping(addr)) return 0;
        delayMicroseconds(DELAY_US);
    }
    return 1;
}

// ============================================================
// ПОИСК МАКСИМАЛЬНОЙ ЧАСТОТЫ
// Возвращает частоту (Гц) или 0, если ни одна не работает
// ============================================================
static uint32_t find_max_freq(uint8_t addr) {
    for (uint8_t i = 0; i < NUM_FREQS; i++) {
        if (test_freq(addr, FREQS[i])) {
            return FREQS[i];
        }
    }
    return 0;
}

// ============================================================
// ВЫВОД АДРЕСА (0xNN)
// ============================================================
static void print_addr(uint8_t addr) {
    Serial.print(F("0x"));
    if (addr < 16) Serial.print('0');
    Serial.print(addr, HEX);
}

// ============================================================
// setup
// ============================================================
void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println();
    Serial.println(F(TXT_HEADER));
    Serial.print(F(TXT_CPU));
    Serial.print(F_CPU / 1000000);
    Serial.println(F(" MHz"));
    Serial.println();

    // Инициализация на безопасной частоте
    i2c_init(100000);

    // Сканирование
    Serial.println(F(TXT_SCANNING));
    uint8_t found[16];
    uint8_t n = i2c_scan(found, 16);

    Serial.print(F(TXT_FOUND));
    Serial.println(n);
    Serial.println();

    if (n == 0) {
        Serial.println(F(TXT_NONE));
        return;
    }

    // Тест каждого устройства на всех частотах
    for (uint8_t i = 0; i < n; i++) {
        uint8_t addr = found[i];
        uint8_t cat = category_index(addr);

        print_addr(addr);
        Serial.print(F(" ("));
        Serial.print(CAT_NAMES[cat]);
        Serial.println(F("):"));

        uint32_t max_freq = 0;

        for (uint8_t f = 0; f < NUM_FREQS; f++) {
            uint32_t freq = FREQS[f];
            uint8_t ok = test_freq(addr, freq);

            Serial.print(F("  "));
            Serial.print(freq / 1000);
            Serial.print(F(TXT_KHZ));
            Serial.print(F(" — "));
            Serial.println(ok ? F(TXT_PASS) : F(TXT_FAIL));

            if (ok && max_freq == 0) {
                max_freq = freq;
            }
        }

        Serial.print(F(TXT_MAX));
        if (max_freq == 0) {
            Serial.println(F(TXT_FAIL));
        } else {
            Serial.print(max_freq / 1000);
            Serial.println(F(TXT_KHZ));
        }
        Serial.println();
    }

    Serial.println(F(TXT_DONE));

    // Возврат на безопасную частоту
    i2c_set_freq(100000);
}

// ============================================================
// loop
// ============================================================
void loop() {
    delay(10000);
}
