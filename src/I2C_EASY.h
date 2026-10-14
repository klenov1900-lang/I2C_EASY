// ============================================================
// I2C_EASY.h
// Низкоуровневый I2C (TWI) слой для ATmega328P.
// Без Wire.h. Прямая работа с регистрами TWI.
// ============================================================
#ifndef I2C_EASY_H
#define I2C_EASY_H

#include <stdint.h>

// ============================================================
// НАСТРОЙКИ ПО УМОЛЧАНИЮ
// ============================================================
#ifndef I2C_EASY_FREQ
#define I2C_EASY_FREQ       100000UL
#endif

#ifndef I2C_EASY_TIMEOUT
#define I2C_EASY_TIMEOUT    5000
#endif

// ============================================================
// КОДЫ ОШИБОК
// ============================================================
#define I2C_OK              0
#define I2C_ERR_TIMEOUT     1
#define I2C_ERR_START       2
#define I2C_ERR_ADDR_NACK   3
#define I2C_ERR_DATA_NACK   4
#define I2C_ERR_BUS         5
#define I2C_ERR_ARB_LOST    6
#define I2C_ERR_BUS_LOCKED  7

// ============================================================
// ИНИЦИАЛИЗАЦИЯ
// ============================================================
void    i2c_init(uint32_t freq_hz);
void    i2c_set_freq(uint32_t freq_hz);

// ============================================================
// БАЗОВЫЕ ОПЕРАЦИИ
// ============================================================
uint8_t i2c_start(uint8_t addr_rw);
uint8_t i2c_write_byte(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);   // при ошибке → 0, проверяйте i2c_last_error()
void    i2c_stop(void);

// ============================================================
// 8-БИТНЫЕ РЕГИСТРЫ
// ============================================================
uint8_t i2c_write(uint8_t addr, const uint8_t* data, uint8_t len);
uint8_t i2c_read (uint8_t addr, uint8_t* data, uint8_t len);
uint8_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val);
uint8_t i2c_read_reg (uint8_t addr, uint8_t reg, uint8_t* val);
uint8_t i2c_write_regs(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t len);
uint8_t i2c_read_regs (uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len);

// ============================================================
// 16-БИТНЫЕ РЕГИСТРЫ
// ============================================================
uint8_t i2c_write_reg16 (uint8_t addr, uint16_t reg, uint8_t val);
uint8_t i2c_read_reg16  (uint8_t addr, uint16_t reg, uint8_t* val);
uint8_t i2c_write_regs16(uint8_t addr, uint16_t reg, const uint8_t* data, uint8_t len);
uint8_t i2c_read_regs16 (uint8_t addr, uint16_t reg, uint8_t* data, uint8_t len);

// ============================================================
// БИТОВЫЕ ОПЕРАЦИИ
// ============================================================
uint8_t i2c_read_bit  (uint8_t addr, uint8_t reg, uint8_t bit, uint8_t* val);
uint8_t i2c_read_bits (uint8_t addr, uint8_t reg, uint8_t start, uint8_t len, uint8_t* val);
uint8_t i2c_write_bit (uint8_t addr, uint8_t reg, uint8_t bit, uint8_t val);
uint8_t i2c_write_bits(uint8_t addr, uint8_t reg, uint8_t start, uint8_t len, uint8_t val);

// ============================================================
// СЛОВА (16-БИТ)
// ============================================================
uint8_t i2c_read_word  (uint8_t addr, uint8_t reg, uint16_t* word);
uint8_t i2c_read_words (uint8_t addr, uint8_t reg, uint16_t* words, uint8_t len);
uint8_t i2c_write_word (uint8_t addr, uint8_t reg, uint16_t word);
uint8_t i2c_write_words(uint8_t addr, uint8_t reg, const uint16_t* words, uint8_t len);

// ============================================================
// УНИВЕРСАЛЬНАЯ ОПЕРАЦИЯ «ЗАПИСЬ → ЧТЕНИЕ»
// ============================================================
uint8_t i2c_write_then_read(uint8_t addr,
                             const uint8_t* wr, uint8_t wr_len,
                             uint8_t* rd, uint8_t rd_len);

// ============================================================
// ДИАГНОСТИКА
// ============================================================
uint8_t i2c_last_error(void);
void    i2c_reset(void);
uint8_t i2c_ping(uint8_t addr);
uint8_t i2c_bus_busy(void);
uint8_t i2c_bus_free(void);
uint8_t i2c_scan(uint8_t* found, uint8_t max);

#endif // I2C_EASY_H