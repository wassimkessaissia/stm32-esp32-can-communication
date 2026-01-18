# STM32 ↔ ESP32 CAN Bus Communication

CAN bus communication between **STM32F401RE** (using MCP2515) and **ESP32-S3** (using TWAI). A complete tutorial demonstrating how to add CAN capabilities to microcontrollers without built-in CAN controllers.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [System Architecture](#system-architecture)
- [Wiring Connections](#wiring-connections)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Troubleshooting](#troubleshooting)
- [What We Learned](#what-we-learned)

---

##  Overview

This project demonstrates **CAN bus communication** between two popular microcontrollers:

- **ESP32-S3**: Has built-in CAN controller (TWAI) - sends messages
- **STM32F401RE**: No built-in CAN - uses external MCP2515 controller - receives messages

**What it does:**
- ESP32 sends a CAN message every second with ID `0x36`
- Message data toggles between `0x01` and `0x00`
- STM32 receives the message and toggles an LED accordingly

---

##  Hardware Requirements

### ESP32 Side

| Component | Description | Notes |
|-----------|-------------|-------|
| ESP32-S3 | Microcontroller with TWAI/CAN | Any ESP32 with CAN support works |
| MCP2551 | CAN transceiver | Converts TWAI signals to CAN bus levels |
| 120Ω resistor | Termination resistor | Between CANH and CANL |

### STM32 Side

| Component | Description | Notes |
|-----------|-------------|-------|
| STM32F401RE | Microcontroller (Nucleo-64) | No built-in CAN controller |
| MCP2515 module | SPI-to-CAN controller | **Must have 8 MHz crystal** |
| TJA1050 | CAN transceiver | Usually built into MCP2515 module |
| 120Ω resistor | Termination resistor | Between CANH and CANL |
| LED | Visual indicator | Or use onboard LED on PB8 |

### Common

| Component | Description |
|-----------|-------------|
| CAN bus wires | CANH and CANL connections |
| Common ground | Shared between ESP32 and STM32 |

---

##  System Architecture
```
┌─────────────┐                                    ┌─────────────┐
│   ESP32-S3  │                                    │ STM32F401RE │
│             │                                    │             │
│    TWAI     │                                    │     SPI     │
│  (Built-in) │                                    │             │
└──────┬──────┘                                    └──────┬──────┘
       │                                                  │
       │ TX/RX                                            │ MOSI/MISO/SCK/CS
       │                                                  │
┌──────▼──────┐         CAN Bus          ┌───────────────▼──────┐
│   MCP2551   │◄────────────────────────►│ MCP2515 + TJA1050    │
│ Transceiver │  CANH ─────────── CANH   │   Controller +       │
│             │  CANL ─────────── CANL   │   Transceiver        │
└─────────────┘         ║         ║      └──────────────────────┘
                        ║         ║
                      [120Ω]   [120Ω]
                   (Termination Resistors)
```

---

##  Wiring Connections

### ESP32 to MCP2551

| ESP32 Pin | MCP2551 Pin | Description |
|-----------|-------------|-------------|
| GPIO 21   | TX          | CAN TX      |
| GPIO 20   | RX          | CAN RX      |
| 5V        | VCC         | Power       |
| GND       | GND         | Ground      |

### STM32 to MCP2515

| STM32 Pin | MCP2515 Pin | Function |
|-----------|-------------|----------|
| PA5       | SCK         | SPI Clock |
| PA6       | SO (MISO)   | SPI Data Out |
| PA7       | SI (MOSI)   | SPI Data In |
| PB6       | CS          | Chip Select |
| 5V        | VCC         | Power (check module) |
| GND       | GND         | Ground |

### CAN Bus Connection

| ESP32 Side (MCP2551) | STM32 Side (MCP2515/TJA1050) |
|----------------------|------------------------------|
| CANH                 | CANH                         |
| CANL                 | CANL                         |
| GND                  | GND (common ground!)         |

**CRITICAL:** Add **120Ω resistors** between CANH and CANL at **both ends** (MCP2515 has a built in 120Ω)!

---

##  Quick Start

### 1. Hardware Setup

1. **Wire everything** according to the connections above
2. **Add 120Ω termination resistors** at both ends
3. **Verify common ground** between ESP32 and STM32
4. **Check MCP2515 crystal** - must be **8 MHz** (look at the silver component)

### 2. ESP32 Setup

1. Navigate to `ESP32/` folder
2. Follow instructions in `ESP32/README.md`
3. Upload code using Arduino IDE or PlatformIO
4. Open Serial Monitor at 115200 baud

### 3. STM32 Setup

1. Navigate to `STM32/` folder
2. Follow instructions in `STM32/README.md`
3. Build and flash using STM32CubeIDE
4. Connect UART at 115200 baud for debug output

### 4. Test

1. Power on both boards
2. ESP32 should show: `[TX] CAN ID: 0x36 Data: 01 00 00 00...`
3. STM32 should show: `RX: ID=0x36, Data[0]=1` and `LED ON/OFF`
4. LED on STM32 should toggle every second

---

##  Project Structure
```
stm32-esp32-can-communication/
├── README.md                    # This file
├── ESP32/
│   ├── README.md               # ESP32-specific setup
│   └── esp32_can_transmitter.ino
└── STM32/
    ├── README.md               # STM32-specific setup
    └── Core/
        ├── Src/
        │   ├── main.c          # Main application
        │   ├── CANSPI.c        # CAN SPI driver
        │   └── MCP2515.c       # MCP2515 low-level driver
        └── Inc/
            ├── main.h
            ├── CANSPI.h
            └── MCP2515.h       # Register definitions
```

---

##  Troubleshooting

### ESP32: "BUS-OFF! Recovery..." Messages

**Symptoms:** ESP32 serial shows periodic BUS-OFF errors

**Causes:**
1. Missing 120Ω termination resistors
2. STM32 not powered or not running
3. CAN bus wiring incorrect (CANH/CANL swapped)
4. No common ground

**Solutions:**
-  Add 120Ω resistors between CANH and CANL at **both ends**
-  Verify STM32 is running (check debug UART output)
-  Check wiring: CANH-to-CANH, CANL-to-CANL
-  Connect GND between ESP32 and STM32

---

### STM32: "MCP2515 Init FAILED! Error: -2"

**Symptoms:** STM32 shows initialization failure

**Causes:**
1. SPI communication failure
2. Wrong CS pin
3. MCP2515 not powered
4. Wiring issues (MOSI/MISO/SCK)

**Solutions:**
-  Check diagnostic output: CANSTAT should be `0x80`, CANCTRL should be `0x87`
-  Verify CS pin is PB6 and correctly wired
-  Check MCP2515 power LED is ON
-  Verify SPI wiring (don't swap MOSI/MISO!)

---


### STM32: "SPI communication FAILED!"

**Symptoms:** CANSTAT reads as 0xFF or 0x00 instead of 0x80

**Causes:**
1. CS pin not connected
2. MOSI/MISO swapped
3. MCP2515 not powered
4. Wrong SPI pins

**Solutions:**
- Verify all SPI connections (PA5, PA6, PA7, PB6)
- Check MCP2515 power (3.3V or 5V depending on module)
- Don't swap MOSI and MISO!

---

### No Communication at All

**Check these in order:**
1.  Both boards powered and running
2.  Common ground connected
3.  120Ω resistors at both ends
4.  CANH-to-CANH, CANL-to-CANL (not swapped)
5.  Correct bit rate (500 kbps on both sides)
6.  Crystal frequency correct (8 MHz for provided code)

---
### STM32: High Receive Error Counter (REC = 128, EFLG = 0x0B)

**Symptoms:** Messages not received, REC increases to 128, EFLG shows error flags

**Cause:** **Crystal frequency mismatch** - This was the main issue we encountered!

**The Problem:**
- Most MCP2515 libraries assume a **16 MHz crystal**
- Many cheap MCP2515 modules come with **8 MHz crystals**
- Wrong configuration = wrong bit timing = NO communication

**Solution:**
1. **Check your MCP2515 crystal frequency** (look at the silver component on module - labeled "8.000" or "16.000")
2. **If 8 MHz** → Use the provided code (configured for 8 MHz) ✓
3. **If 16 MHz** → Modify `CANSPI.c`:
```c
   // In CANSPI_Initialize(), change CNF registers:
   MCP2515_WriteByte(MCP2515_CNF1, 0x00);
   MCP2515_WriteByte(MCP2515_CNF2, 0xE5);  // For 16 MHz
   MCP2515_WriteByte(MCP2515_CNF3, 0x83);  // For 16 MHz
```

**Why this happens:** 
- Bit timing calculations depend on crystal frequency
- 8 MHz crystal with 16 MHz settings = running at 250 kbps instead of 500 kbps
- ESP32 sends at 500 kbps, STM32 listens at 250 kbps = garbled messages
- Result: REC counter maxes out at 128 (error-passive state)

**This was the hardest bug to find!** The library assumed 16 MHz, but the hardware had 8 MHz.

---

##  What We Learned
**The hardest problem to solve:** Most MCP2515 libraries and examples assume a 16 MHz crystal, but many cheap modules use 8 MHz crystals. This causes:
- Perfect SPI communication (so you think everything works)
- Successful initialization (MCP2515 responds correctly)
- **Complete CAN failure** (no messages received, REC = 128)

The symptoms don't point to the real cause - it looks like a wiring or termination problem, but it's actually bit timing!
### Key Concepts

1. **Crystal Frequency is CRITICAL**: MCP2515 bit timing calculations depend entirely on crystal frequency
2. **Termination is Critical**: 120Ω resistors at both ends prevent signal reflections
3. **Common Ground Required**: CAN is not fully isolated - needs common ground reference
4. **Filters Can Be Disabled**: Setting RXBnCTRL to `0x60` accepts all messages

### Technical Details

#### CAN Bit Timing (500 kbps @ 8 MHz)
- Time Quantum (TQ) = 0.25 µs
- Bit time = 8 TQ = 2 µs
- Segments: SYNC(1) + PROP(1) + PS1(3) + PS2(3) = 8 TQ

#### MCP2515 Operating Modes
- **Configuration Mode (0x80)**: Set registers, cannot TX/RX
- **Normal Mode (0x00)**: Active communication
- Switch modes via CANCTRL register, verify mode via CANSTAT

#### Error Detection
- **TEC (Transmit Error Counter)**: Increases when TX fails
- **REC (Receive Error Counter)**: Increases when RX corrupted
- **EFLG Register**: Shows error flags

### Note on Message Filtering

We disabled CAN filters in this project (accept all messages), which is fine for learning. In real automotive applications, ECUs use strict filtering to only process relevant messages - essential when the bus has hundreds of messages per second!
---

##  References

- [MCP2515 Datasheet](https://www.microchip.com/en-us/product/MCP2515) - Microchip DS21801E
- [ESP32 TWAI Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html)
- [STM32F401RE Datasheet](https://www.st.com/en/microcontrollers-microprocessors/stm32f401re.html)
- [STM32 MCP2515 Library](https://github.com/ProjectoOfficial/STM32/tree/main/STM32_MCP2515) - Reference implementation by ProjectoOfficial

---
##  License

This project is open source and available under the [MIT License](LICENSE).

Feel free to use, modify, and distribute with proper attribution.

---

##  Contributing

Found a bug? Have a suggestion? Feel free to open an issue or submit a pull request!

---

**Happy CAN-ing!** 🚗💨