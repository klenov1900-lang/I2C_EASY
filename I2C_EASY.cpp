// ============================================================
// I2C_EASY.cpp
// Низкоуровневый I2C (TWI) слой для ATmega328P.
// Без Wire.h. Прямая работа с регистрами TWI.
// ============================================================
#include "I2C_EASY.h"
#include <util/twi.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// ============================================================
// ВНУТРЕННИЕ ПЕРЕМЕННЫЕ
// ============================================================
static uint8_t i2c_error = I2C_OK;

// ============================================================
// ОЖИДАНИЕ ЗАВЕРШЕНИЯ TWINT
// ============================================================
static inline uint8_t i2c_wait(void) {
    uint16_t timeout = I2C_EASY_TIMEOUT;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            i2c_error = I2C_ERR_TIMEOUT;
            return 0;
        }
    }
    return 1;
}

// ============================================================
// ВНУТРЕННИЙ: РАСЧЁТ TWBR С ЗАЩИТОЙ ОТ ПЕРЕПОЛНЕНИЯ
// ============================================================
static void i2c_set_twbr(uint32_t freq_hz) {
    uint16_t twbr_val = (F_CPU / freq_hz);
    if (twbr_val < 16) twbr_val = 16;          // защита от отрицательного
    twbr_val = (twbr_val - 16) / 2;
    if (twbr_val < 1)   twbr_val = 1;          // минимум
    if (twbr_val > 255) twbr_val = 255;        // максимум
    TWBR = (uint8_t)twbr_val;
    TWSR = 0;
}

// ============================================================
// ИНИЦИАЛИЗАЦИЯ
// ============================================================
void i2c_init(uint32_t freq_hz) {
    DDRC |= (1 << PC5);
    DDRC &= ~(1 << PC4);
    PORTC |= (1 << PC4);

    // Сброс шины: 9 тактов SCL
    for (uint8_t i = 0; i < 9; i++) {
        PORTC |= (1 << PC5);  _delay_us(5);
        PORTC &= ~(1 << PC5); _delay_us(5);
    }

    // STOP вручную
    DDRC |= (1 << PC4);
    PORTC &= ~(1 << PC4); _delay_us(5);
    PORTC |= (1 << PC5);  _delay_us(5);
    PORTC |= (1 << PC4);  _delay_us(5);

    DDRC &= ~((1 << PC4) | (1 << PC5));
    PORTC |= ((1 << PC4) | (1 << PC5));

    i2c_set_twbr(freq_hz);
    TWCR = (1 << TWEN);

    _delay_ms(1);
    i2c_error = I2C_OK;
}

void i2c_set_freq(uint32_t freq_hz) {
    i2c_set_twbr(freq_hz);
}

// ============================================================
// БАЗОВЫЕ ОПЕРАЦИИ
// ============================================================
uint8_t i2c_start(uint8_t addr_rw) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!i2c_wait()) { i2c_error = I2C_ERR_START; return i2c_error; }

    uint8_t status = TWSR & 0xF8;
    if (status != TW_START && status != TW_REP_START) {
        i2c_error = (status == TW_MT_ARB_LOST) ? I2C_ERR_ARB_LOST : I2C_ERR_BUS;
        return i2c_error;
    }

    TWDR = addr_rw;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!i2c_wait()) return i2c_error;

    status = TWSR & 0xF8;
    if (status == TW_MT_SLA_NACK || status == TW_MR_SLA_NACK) {
        i2c_error = I2C_ERR_ADDR_NACK;
        return i2c_error;
    }
    if (status != TW_MT_SLA_ACK && status != TW_MR_SLA_ACK) {
        i2c_error = (status == TW_MT_ARB_LOST) ? I2C_ERR_ARB_LOST : I2C_ERR_BUS;
        return i2c_error;
    }

    i2c_error = I2C_OK;
    return I2C_OK;
}

uint8_t i2c_write_byte(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!i2c_wait()) return i2c_error;

    uint8_t status = TWSR & 0xF8;
    if (status == TW_MT_DATA_NACK) { i2c_error = I2C_ERR_DATA_NACK; return i2c_error; }
    if (status != TW_MT_DATA_ACK) {
        i2c_error = (status == TW_MT_ARB_LOST) ? I2C_ERR_ARB_LOST : I2C_ERR_BUS;
        return i2c_error;
    }

    i2c_error = I2C_OK;
    return I2C_OK;
}

uint8_t i2c_read_byte(uint8_t ack) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0);
    if (!i2c_wait()) return 0;

    uint8_t status = TWSR & 0xF8;
    if (status != TW_MR_DATA_ACK && status != TW_MR_DATA_NACK) {
        i2c_error = (status == TW_MT_ARB_LOST) ? I2C_ERR_ARB_LOST : I2C_ERR_BUS;
        return 0;
    }

    i2c_error = I2C_OK;
    return TWDR;
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(10);
}

// ============================================================
// 8-БИТНЫЕ РЕГИСТРЫ
// ============================================================
uint8_t i2c_write(uint8_t addr, const uint8_t* data, uint8_t len) {
    if (i2c_start((addr << 1) | 0) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < len; i++) {
        if (i2c_write_byte(data[i]) != I2C_OK) { i2c_stop(); return i2c_error; }
    }
    i2c_stop();
    return I2C_OK;
}

uint8_t i2c_read(uint8_t addr, uint8_t* data, uint8_t len) {
    if (i2c_start((addr << 1) | 1) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < len; i++) {
        uint8_t ack = (i < len - 1) ? 1 : 0;
        data[i] = i2c_read_byte(ack);
        if (i2c_error != I2C_OK) { i2c_stop(); return i2c_error; }
    }
    i2c_stop();
    return I2C_OK;
}

uint8_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val) {
    uint8_t buf[2] = { reg, val };
    return i2c_write(addr, buf, 2);
}

uint8_t i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t* val) {
    return i2c_read_regs(addr, reg, val, 1);
}

uint8_t i2c_write_regs(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t len) {
    if (i2c_start((addr << 1) | 0) != I2C_OK) { i2c_stop(); return i2c_error; }
    if (i2c_write_byte(reg) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < len; i++) {
        if (i2c_write_byte(data[i]) != I2C_OK) { i2c_stop(); return i2c_error; }
    }
    i2c_stop();
    return I2C_OK;
}

uint8_t i2c_read_regs(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
    if (i2c_start((addr << 1) | 0) != I2C_OK) { i2c_stop(); return i2c_error; }
    if (i2c_write_byte(reg) != I2C_OK) { i2c_stop(); return i2c_error; }

    if (i2c_start((addr << 1) | 1) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < len; i++) {
        uint8_t ack = (i < len - 1) ? 1 : 0;
        data[i] = i2c_read_byte(ack);
    }
    i2c_stop();
    return i2c_error;
}

// ============================================================
// 16-БИТНЫЕ РЕГИСТРЫ
// ============================================================
uint8_t i2c_write_reg16(uint8_t addr, uint16_t reg, uint8_t val) {
    return i2c_write_regs16(addr, reg, &val, 1);
}

uint8_t i2c_read_reg16(uint8_t addr, uint16_t reg, uint8_t* val) {
    return i2c_read_regs16(addr, reg, val, 1);
}

uint8_t i2c_write_regs16(uint8_t addr, uint16_t reg, const uint8_t* data, uint8_t len) {
    if (i2c_start((addr << 1) | 0) != I2C_OK) { i2c_stop(); return i2c_error; }
    if (i2c_write_byte((uint8_t)(reg >> 8))   != I2C_OK) { i2c_stop(); return i2c_error; }
    if (i2c_write_byte((uint8_t)(reg & 0xFF)) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < len; i++) {
        if (i2c_write_byte(data[i]) != I2C_OK) { i2c_stop(); return i2c_error; }
    }
    i2c_stop();
    return I2C_OK;
}

uint8_t i2c_read_regs16(uint8_t addr, uint16_t reg, uint8_t* data, uint8_t len) {
    if (i2c_start((addr << 1) | 0) != I2C_OK) { i2c_stop(); return i2c_error; }
    if (i2c_write_byte((uint8_t)(reg >> 8))   != I2C_OK) { i2c_stop(); return i2c_error; }
    if (i2c_write_byte((uint8_t)(reg & 0xFF)) != I2C_OK) { i2c_stop(); return i2c_error; }

    if (i2c_start((addr << 1) | 1) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < len; i++) {
        uint8_t ack = (i < len - 1) ? 1 : 0;
        data[i] = i2c_read_byte(ack);
    }
    i2c_stop();
    return i2c_error;
}

// ============================================================
// БИТОВЫЕ ОПЕРАЦИИ (улучшенная читаемость проверок)
// ============================================================
uint8_t i2c_read_bit(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t* val) {
    uint8_t b;
    uint8_t err = i2c_read_reg(addr, reg, &b);
    if (err != I2C_OK) return err;
    *val = (b >> bit) & 0x01;
    return I2C_OK;
}

uint8_t i2c_read_bits(uint8_t addr, uint8_t reg, uint8_t start, uint8_t len, uint8_t* val) {
    if (len == 0 || len > 8 || start >= 8) return I2C_ERR_BUS;
    if (start + 1 < len) return I2C_ERR_BUS;   // поле не помещается в байт

    uint8_t b;
    uint8_t err = i2c_read_reg(addr, reg, &b);
    if (err != I2C_OK) return err;

    uint8_t shift = start - len + 1;
    uint8_t mask = ((1 << len) - 1) << shift;
    *val = (b & mask) >> shift;
    return I2C_OK;
}

uint8_t i2c_write_bit(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t val) {
    uint8_t b;
    uint8_t err = i2c_read_reg(addr, reg, &b);
    if (err != I2C_OK) return err;

    if (val) b |=  (1 << bit);
    else     b &= ~(1 << bit);

    return i2c_write_reg(addr, reg, b);
}

uint8_t i2c_write_bits(uint8_t addr, uint8_t reg, uint8_t start, uint8_t len, uint8_t val) {
    if (len == 0 || len > 8 || start >= 8) return I2C_ERR_BUS;
    if (start + 1 < len) return I2C_ERR_BUS;

    uint8_t b;
    uint8_t err = i2c_read_reg(addr, reg, &b);
    if (err != I2C_OK) return err;

    uint8_t shift = start - len + 1;
    uint8_t mask = ((1 << len) - 1) << shift;
    b &= ~mask;
    b |= (val << shift) & mask;

    return i2c_write_reg(addr, reg, b);
}

// ============================================================
// СЛОВА (16-БИТ, MSB FIRST)
// ============================================================
uint8_t i2c_read_word(uint8_t addr, uint8_t reg, uint16_t* word) {
    uint8_t buf[2];
    uint8_t err = i2c_read_regs(addr, reg, buf, 2);
    if (err != I2C_OK) return err;
    *word = ((uint16_t)buf[0] << 8) | buf[1];
    return I2C_OK;
}

uint8_t i2c_read_words(uint8_t addr, uint8_t reg, uint16_t* words, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        uint8_t err = i2c_read_word(addr, reg + i * 2, &words[i]);
        if (err != I2C_OK) return err;
    }
    return I2C_OK;
}

uint8_t i2c_write_word(uint8_t addr, uint8_t reg, uint16_t word) {
    uint8_t buf[2] = { (uint8_t)(word >> 8), (uint8_t)(word & 0xFF) };
    return i2c_write_regs(addr, reg, buf, 2);
}

uint8_t i2c_write_words(uint8_t addr, uint8_t reg, const uint16_t* words, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        uint8_t err = i2c_write_word(addr, reg + i * 2, words[i]);
        if (err != I2C_OK) return err;
    }
    return I2C_OK;
}

// ============================================================
// УНИВЕРСАЛЬНАЯ ОПЕРАЦИЯ «ЗАПИСЬ → ЧТЕНИЕ»
// ============================================================
uint8_t i2c_write_then_read(uint8_t addr,
                             const uint8_t* wr, uint8_t wr_len,
                             uint8_t* rd, uint8_t rd_len) {
    if (i2c_start((addr << 1) | 0) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < wr_len; i++) {
        if (i2c_write_byte(wr[i]) != I2C_OK) { i2c_stop(); return i2c_error; }
    }
    if (i2c_start((addr << 1) | 1) != I2C_OK) { i2c_stop(); return i2c_error; }
    for (uint8_t i = 0; i < rd_len; i++) {
        uint8_t ack = (i < rd_len - 1) ? 1 : 0;
        rd[i] = i2c_read_byte(ack);
    }
    i2c_stop();
    return i2c_error;
}

// ============================================================
// ДИАГНОСТИКА
// ============================================================
uint8_t i2c_last_error(void) { return i2c_error; }

void i2c_reset(void) {
    DDRC &= ~((1 << PC4) | (1 << PC5));
    PORTC |= ((1 << PC4) | (1 << PC5));
    _delay_us(10);

    TWCR = 0;
    TWCR = (1 << TWEN);
    _delay_us(10);

    i2c_error = I2C_OK;
}

uint8_t i2c_ping(uint8_t addr) {
    if (i2c_start((addr << 1) | 0) == I2C_OK) { i2c_stop(); return 1; }
    i2c_stop();
    return 0;
}

uint8_t i2c_bus_busy(void) {
    DDRC &= ~((1 << PC4) | (1 << PC5));
    PORTC |= ((1 << PC4) | (1 << PC5));
    _delay_us(5);
    uint8_t sda = (PINC & (1 << PC4)) ? 1 : 0;
    uint8_t scl = (PINC & (1 << PC5)) ? 1 : 0;
    return !(sda && scl);
}

uint8_t i2c_bus_free(void) { return i2c_bus_busy() ? 0 : 1; }

uint8_t i2c_scan(uint8_t* found, uint8_t max) {
    uint8_t count = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        if (i2c_ping(addr)) {
            if (count < max) found[count] = addr;
            count++;
        }
    }
    i2c_error = I2C_OK;
    return count;
}