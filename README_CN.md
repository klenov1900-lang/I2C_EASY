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
