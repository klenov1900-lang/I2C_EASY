# I2C_EASY

**Языки:** Русский · [中文](README_CN.md)

[![Version](https://img.shields.io/badge/version-1.0.0-blue)](https://github.com/klenov1900-lang/I2C_EASY/releases)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-Nano%20%7C%20Uno%20%7C%20Pro%20Mini-teal)](https://www.arduino.cc/)
[![Platform](https://img.shields.io/badge/platform-AVR-orange)](https://www.microchip.com/)
[![Status](https://img.shields.io/badge/status-stable-brightgreen)](https://github.com/klenov1900-lang/I2C_EASY)
[![Library Manager](https://img.shields.io/badge/Arduino%20Library%20Manager-approved-blue)](https://www.arduino.cc/reference/en/libraries/)

Низкоуровневый I2C (TWI) слой для ATmega328P. **Без `Wire.h`.**
Прямая работа с регистрами TWI. Основа семейства **`*_EASY`.**

## ✨ Особенности

- ✅ **Без `Wire.h`** — прямая работа с регистрами TWI
- ✅ **Байтовые операции** (read/write, 8-битные регистры)
- ✅ **Битовые операции** (readBit/writeBit, readBits/writeBits)
- ✅ **16-битные регистры** (EEPROM, MLX90614, SHT3x)
- ✅ **Слова** (16-бит) и массивы слов
- ✅ **Универсальная операция «запись → чтение»**
- ✅ **Timeout везде** — не зависает при отвале устройства
- ✅ **Восстановление шины** — `i2c_reset()`
- ✅ **Сканер I2C** — находит все устройства
- ✅ **Тест частоты** — определяет максимальную для каждого устройства
- ✅ **8 кодов ошибок** для диагностики

## ⚙️ Требования и совместимость

- **Микроконтроллер:** ATmega328P (8-битный AVR).
- **Платы:** Arduino Uno, Nano, Pro Mini.
- **Тактовая частота:** 16 МГц (стандарт). На 8 МГц — работает с `set_freq`.
- **Другие платформы:** ESP8266, ESP32, STM32 — **не поддерживаются** (нет прямого доступа к регистрам AVR).

## 📦 Установка

### Способ 1: Через менеджер библиотек Arduino IDE

1. Откройте Arduino IDE
2. Перейдите в **Скетч → Подключить библиотеку → Управлять библиотеками…**
3. В поиске введите `I2C_EASY`
4. Нажмите **Установить**

### Способ 2: Вручную (через ZIP)

1. Скачайте ZIP-архив с [последней версией](https://github.com/klenov1900-lang/I2C_EASY/archive/refs/heads/main.zip)
2. Arduino IDE: **Скетч → Подключить библиотеку → Добавить .ZIP библиотеку…**
3. Выберите скачанный архив

### Способ 3: Через Git

```bash
cd ~/Arduino/libraries/
git clone https://github.com/klenov1900-lang/I2C_EASY.git
```
## 🚀 Быстрый старт

```c
#include <I2C_EASY.h>

void setup() {
    Serial.begin(9600);
    i2c_init(400000);   // 400 кГц

    uint8_t found[16];
    uint8_t n = i2c_scan(found, 16);

    Serial.print(F("Found: "));
    Serial.println(n);

    for (uint8_t i = 0; i < n; i++) {
        Serial.print(F("0x"));
        if (found[i] < 16) Serial.print('0');
        Serial.println(found[i], HEX);
    }
}

void loop() {}
```
## 📚 API

### Инициализация

| Функция | Описание |
|---|---|
| `i2c_init(freq)` | Инициализация на заданной частоте |
| `i2c_set_freq(freq)` | Смена частоты на лету |

### Базовые операции

| Функция | Описание |
|---|---|
| `i2c_start(addr_rw)` | START + адрес |
| `i2c_write_byte(data)` | Запись байта |
| `i2c_read_byte(ack)` | Чтение байта |
| `i2c_stop()` | STOP |

### 8-битные регистры

| Функция | Описание |
|---|---|
| `i2c_write(addr, data, len)` | Запись N байт |
| `i2c_read(addr, data, len)` | Чтение N байт |
| `i2c_write_reg(addr, reg, val)` | Запись в регистр |
| `i2c_read_reg(addr, reg, &val)` | Чтение из регистра |
| `i2c_write_regs(addr, reg, data, len)` | Запись N байт в регистры |
| `i2c_read_regs(addr, reg, data, len)` | Чтение N байт из регистров |

### 16-битные регистры

| Функция | Описание |
|---|---|
| `i2c_write_reg16(addr, reg, val)` | Запись в 16-битный регистр |
| `i2c_read_reg16(addr, reg, &val)` | Чтение из 16-битного регистра |
| `i2c_write_regs16(addr, reg, data, len)` | Запись N байт |
| `i2c_read_regs16(addr, reg, data, len)` | Чтение N байт |

### Битовые операции

| Функция | Описание |
|---|---|
| `i2c_read_bit(addr, reg, bit, &val)` | Чтение бита |
| `i2c_read_bits(addr, reg, start, len, &val)` | Чтение N бит |
| `i2c_write_bit(addr, reg, bit, val)` | Запись бита |
| `i2c_write_bits(addr, reg, start, len, val)` | Запись N бит |

### Слова (16-бит)

| Функция | Описание |
|---|---|
| `i2c_read_word(addr, reg, &word)` | Чтение слова (MSB first) |
| `i2c_read_words(addr, reg, words, len)` | Чтение N слов |
| `i2c_write_word(addr, reg, word)` | Запись слова |
| `i2c_write_words(addr, reg, words, len)` | Запись N слов |

### Универсальная операция

| Функция | Описание |
|---|---|
| `i2c_write_then_read(addr, wr, wr_len, rd, rd_len)` | Запись → Repeated START → Чтение |

### Диагностика

| Функция | Описание |
|---|---|
| `i2c_last_error()` | Код последней ошибки |
| `i2c_reset()` | Восстановление шины |
| `i2c_ping(addr)` | Проверка устройства (1 = есть) |
| `i2c_bus_busy()` | Шина занята? |
| `i2c_bus_free()` | Шина свободна? |
| `i2c_scan(found, max)` | Сканер шины |

## 📊 Коды ошибок

| Код | Константа | Описание |
|---|---|---|
| 0 | `I2C_OK` | Успех |
| 1 | `I2C_ERR_TIMEOUT` | Таймаут ожидания TWINT |
| 2 | `I2C_ERR_START` | Ошибка START |
| 3 | `I2C_ERR_ADDR_NACK` | Устройство не ответило (NACK на адрес) |
| 4 | `I2C_ERR_DATA_NACK` | NACK на данные |
| 5 | `I2C_ERR_BUS` | Ошибка состояния шины |
| 6 | `I2C_ERR_ARB_LOST` | Потеря арбитража (multi-master) |
| 7 | `I2C_ERR_BUS_LOCKED` | SDA удерживается низко |

## 📐 Расчёт частоты

**Формула:** `TWBR = ((F_CPU / freq_hz) - 16) / 2`

| F_CPU | freq_hz | TWBR | SCL |
|---|---|---|---|
| 16 МГц | 400 кГц | 12 | 400 кГц |
| 16 МГц | 200 кГц | 32 | 200 кГц |
| 16 МГц | 100 кГц | 72 | 100 кГц |
| 8 МГц | 400 кГц | 2 | 400 кГц |
| 16 МГц | 1 МГц | 1 | 888 кГц (макс.) |

## 📝 История версий

| Версия | Дата | Изменения |
|---|---|---|
| **v1.0.0** | 2026-10-14 | Первый релиз. Базовый API, сканер, тест частоты. |

## 📄 Лицензия

MIT License. См. [LICENSE](LICENSE).

## 📧 Контакты

- **Email:** gray_wolf19@mail.ru
- **GitHub:** [klenov1900-lang](https://github.com/klenov1900-lang)

---

⭐ Если библиотека вам полезна, поставьте звезду на GitHub! ⭐
