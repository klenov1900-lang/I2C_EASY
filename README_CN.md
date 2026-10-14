# I2C_EASY

**语言:** [Русский](README.md) · 中文

[![Version](https://img.shields.io/badge/version-1.0.0-blue)](https://github.com/klenov1900-lang/I2C_EASY/releases)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-Nano%20%7C%20Uno%20%7C%20Pro%20Mini-teal)](https://www.arduino.cc/)
[![Platform](https://img.shields.io/badge/platform-AVR-orange)](https://www.microchip.com/)
[![Status](https://img.shields.io/badge/status-stable-brightgreen)](https://github.com/klenov1900-lang/I2C_EASY)
[![Library Manager](https://img.shields.io/badge/Arduino%20Library%20Manager-approved-blue)](https://www.arduino.cc/reference/en/libraries/)

适用于 ATmega328P 的低层 I2C（TWI）库。**不依赖 `Wire.h`。**
直接操作 TWI 寄存器。`*_EASY` 系列的基础。

## ✨ 特性

- ✅ **无 `Wire.h`** — 直接操作 TWI 寄存器
- ✅ **字节操作**（读/写，8 位寄存器）
- ✅ **位操作**（readBit/writeBit, readBits/writeBits）
- ✅ **16 位寄存器**（EEPROM、MLX90614、SHT3x）
- ✅ **字**（16 位）和字数组
- ✅ **通用操作「写入 → 读取」**
- ✅ **处处有超时** — 设备掉线不会卡死
- ✅ **总线恢复** — `i2c_reset()`
- ✅ **I2C 扫描仪** — 查找所有设备
- ✅ **频率测试** — 为每个设备确定最大频率
- ✅ **8 个错误码** 用于诊断

## ⚙️ 要求与兼容性

- **微控制器：** ATmega328P（8 位 AVR）。
- **开发板：** Arduino Uno、Nano、Pro Mini。
- **时钟频率：** 16 МГц（标准）。8 МГц — 支持，通过 `set_freq`。
- **其他平台：** ESP8266、ESP32、STM32 — **不支持**（无法直接访问 AVR 寄存器）。
## 📦 安装

### 方式 1：Arduino IDE 库管理器

1. 打开 Arduino IDE
2. 进入 **项目 → 加载库 → 管理库…**
3. 搜索 `I2C_EASY`
4. 点击 **安装**

### 方式 2：手动（ZIP）

1. 下载 [最新版本](https://github.com/klenov1900-lang/I2C_EASY/archive/refs/heads/main.zip) 的 ZIP 压缩包
2. Arduino IDE：**项目 → 加载库 → 添加 .ZIP 库…**
3. 选择下载的压缩包

### 方式 3：通过 Git

```bash
cd ~/Arduino/libraries/
git clone https://github.com/klenov1900-lang/I2C_EASY.git
```

## 🚀 快速开始

```c
#include <I2C_EASY.h>

void setup() {
    Serial.begin(9600);
    i2c_init(400000);   // 400 kHz

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
## 📖 示例

| 示例 | 描述 |
|---|---|
| **Scanner** | 简单总线扫描仪 |
| **Scanner_Freq** | 扫描仪 + 最大频率测试 |

## 📚 API

### 初始化

| 函数 | 描述 |
|---|---|
| `i2c_init(freq)` | 以指定频率初始化 |
| `i2c_set_freq(freq)` | 运行时更改频率 |

### 基本操作

| 函数 | 描述 |
|---|---|
| `i2c_start(addr_rw)` | START + 地址 |
| `i2c_write_byte(data)` | 写字节 |
| `i2c_read_byte(ack)` | 读字节 |
| `i2c_stop()` | STOP |

### 8 位寄存器

| 函数 | 描述 |
|---|---|
| `i2c_write(addr, data, len)` | 写 N 个字节 |
| `i2c_read(addr, data, len)` | 读 N 个字节 |
| `i2c_write_reg(addr, reg, val)` | 写寄存器 |
| `i2c_read_reg(addr, reg, &val)` | 读寄存器 |
| `i2c_write_regs(addr, reg, data, len)` | 写 N 个字节 |
| `i2c_read_regs(addr, reg, data, len)` | 读 N 个字节 |

### 16 位寄存器

| 函数 | 描述 |
|---|---|
| `i2c_write_reg16(addr, reg, val)` | 写 16 位寄存器 |
| `i2c_read_reg16(addr, reg, &val)` | 读 16 位寄存器 |
| `i2c_write_regs16(addr, reg, data, len)` | 写 N 个字节 |
| `i2c_read_regs16(addr, reg, data, len)` | 读 N 个字节 |

### 位操作

| 函数 | 描述 |
|---|---|
| `i2c_read_bit(addr, reg, bit, &val)` | 读一位 |
| `i2c_read_bits(addr, reg, start, len, &val)` | 读 N 位 |
| `i2c_write_bit(addr, reg, bit, val)` | 写一位 |
| `i2c_write_bits(addr, reg, start, len, val)` | 写 N 位 |

### 字（16 位）

| 函数 | 描述 |
|---|---|
| `i2c_read_word(addr, reg, &word)` | 读字（MSB first） |
| `i2c_read_words(addr, reg, words, len)` | 读 N 个字 |
| `i2c_write_word(addr, reg, word)` | 写字 |
| `i2c_write_words(addr, reg, words, len)` | 写 N 个字 |

### 通用操作

| 函数 | 描述 |
|---|---|
| `i2c_write_then_read(addr, wr, wr_len, rd, rd_len)` | 写 → Repeated START → 读 |

### 诊断

| 函数 | 描述 |
|---|---|
| `i2c_last_error()` | 最后错误码 |
| `i2c_reset()` | 总线恢复 |
| `i2c_ping(addr)` | 设备检查 |
| `i2c_bus_busy()` | 总线忙？ |
| `i2c_bus_free()` | 总线空闲？ |
| `i2c_scan(found, max)` | 总线扫描仪 |

## 📊 错误码

| 代码 | 常量 | 描述 |
|---|---|---|
| 0 | `I2C_OK` | 成功 |
| 1 | `I2C_ERR_TIMEOUT` | TWINT 超时 |
| 2 | `I2C_ERR_START` | START 错误 |
| 3 | `I2C_ERR_ADDR_NACK` | 设备无应答 |
| 4 | `I2C_ERR_DATA_NACK` | 数据 NACK |
| 5 | `I2C_ERR_BUS` | 总线状态错误 |
| 6 | `I2C_ERR_ARB_LOST` | 仲裁丢失 |
| 7 | `I2C_ERR_BUS_LOCKED` | SDA 被拉低 |

## 📐 频率计算

**公式：** `TWBR = ((F_CPU / freq_hz) - 16) / 2`

| F_CPU | freq_hz | TWBR | SCL |
|---|---|---|---|
| 16 МГц | 400 кГц | 12 | 400 кГц |
| 16 МГц | 200 кГц | 32 | 200 кГц |
| 16 МГц | 100 кГц | 72 | 100 кГц |
| 8 МГц | 400 кГц | 2 | 400 кГц |
| 16 МГц | 1 МГц | 1 | 888 кГц（最大） |
## 📝 版本历史

| 版本 | 日期 | 变更 |
|---|---|---|
| **v1.0.0** | 2026-10-14 | 首次发布。基础 API、扫描仪、频率测试。 |

## 📄 许可证

MIT License。参见 [LICENSE](LICENSE)。

## 📧 联系方式

- **Email:** gray_wolf19@mail.ru
- **GitHub:** [klenov1900-lang](https://github.com/klenov1900-lang)

---

⭐ 如果这个库对您有用，请在 GitHub 上点个星！⭐
