# STM32 CAN Receiver

## Overview
This code receives CAN messages with ID `0x36` and toggles an LED based on the received data. Includes diagnostic output and periodic status monitoring.

---
##  CRITICAL: Crystal Frequency Issue

**Before you start**, be aware of this common problem:

Most MCP2515 example code and libraries are configured for **16 MHz crystals**, but many cheap MCP2515 modules sold online come with **8 MHz crystals**. This mismatch causes **complete communication failure** with symptoms that are hard to diagnose:

- ✅ SPI communication works fine
- ✅ MCP2515 initializes successfully  
- ❌ **But NO CAN messages are received!**
- ❌ REC (Receive Error Counter) climbs to 128
- ❌ EFLG shows error flags (0x0B)

**The code in this repository is configured for 8 MHz crystals** (the most common).

**How to check your crystal:**
Look at the silver rectangular component on your MCP2515 module. It's labeled either "8.000" (8 MHz) or "16.000" (16 MHz).

If you have 16 MHz, see the configuration section below for the fix.

---
## Hardware Requirements

### STM32 Board
- **STM32F401RE** (Nucleo-64)
- No built-in CAN controller

### CAN Controller
- **MCP2515** module with **8 MHz crystal**
- Includes **TJA1050** CAN transceiver (built into module)

### LED
- **Pin:** PB8 (onboard LED or external)

---

## Pin Configuration

### SPI1 (MCP2515 Communication):
| STM32 Pin | Function | MCP2515 Pin |
|-----------|----------|-------------|
| PA5       | SCK      | SCK         |
| PA6       | MISO     | SO          |
| PA7       | MOSI     | SI          |
| PB6       | CS       | CS          |

### UART2 (Debug Output):
| STM32 Pin | Function | Description |
|-----------|----------|-------------|
| PA2       | TX       | Serial debug output |
| PA3       | RX       | Serial input (unused) |
| Baud Rate | 115200   | -           |

### GPIO:
| STM32 Pin | Function | Description |
|-----------|----------|-------------|
| PB8       | LED      | Toggles based on CAN message |

---

## Software Requirements

- **STM32CubeIDE** (recommended) or any STM32 development environment
- **STM32 HAL Library** (included with CubeIDE)

---

## CAN Bus Configuration

| Parameter | Value |
|-----------|-------|
| Bit Rate  | 500 kbps |
| Crystal   | **8 MHz** (MCP2515 module) |
| Mode      | Normal (for receiving) |
| Frame Type| Standard (11-bit ID) |
| Message ID| 0x36 |
| Filters   | Disabled (accept all messages) |

---

## Important: MCP2515 Crystal Frequency

**CRITICAL:** Your MCP2515 module **MUST** have an **8 MHz crystal**.

- ✅ **8 MHz crystal** → Use provided code as-is
- ❌ **16 MHz crystal** → Modify `CANSPI.c` CNF registers:
```c
  // In CANSPI_Initialize(), change these lines:
  MCP2515_WriteByte(MCP2515_CNF1, 0x00);
  MCP2515_WriteByte(MCP2515_CNF2, 0xE5);  // For 16MHz
  MCP2515_WriteByte(MCP2515_CNF3, 0x83);  // For 16MHz
```

To check your crystal frequency, look at the silver component on the MCP2515 module (labeled "8.000" or "16.000").

---

## How It Works

### 1. Startup Diagnostics
On power-up, the system performs comprehensive diagnostics:
- Sends RESET command to MCP2515
- Reads CANSTAT register (expects 0x80 = Configuration mode)
- Reads CANCTRL register (expects 0x87 = default reset value)
- Verifies SPI communication is working
- Initializes CAN controller with 500kbps @ 8MHz settings
- Switches to Normal mode
- Displays final CANSTAT and CANCTRL values

### 2. Main Loop
The system continuously:
- **Receives CAN messages**: Checks for incoming messages with ID `0x36`
  - `data[0] = 1` → Turn LED ON (PB8 = HIGH)
  - `data[0] = 0` → Turn LED OFF (PB8 = LOW)
- **Monitors status**: Every 2 seconds, displays:
  - Messages in buffer
  - Error flags (EFLG register)
  - Transmit Error Counter (TEC)
  - Receive Error Counter (REC)

---

## Serial Monitor Output

Connect to UART2 at **115200 baud** to see:

### Successful Startup:
```
=== MCP2515 Diagnostic ===
Sending RESET command...
CANSTAT after reset: 0x80 (expected: 0x80)
CANCTRL after reset: 0x87 (expected: 0x87)
SPI communication OK!
MCP2515 Init SUCCESS!
CANSTAT: 0x00, CANCTRL: 0x00
```

### During Operation:
```
RX: ID=0x36, Data[0]=1
LED ON
Status - Messages: 0, EFLG: 0x00, TEC: 0, REC: 0
RX: ID=0x36, Data[0]=0
LED OFF
Status - Messages: 0, EFLG: 0x00, TEC: 0, REC: 0
```

### Status Indicators:
- **Messages**: Number of unread messages in receive buffers
- **EFLG**: Error flags (0x00 = no errors)
  - 0x01 = Error warning
  - 0x08 = RX error-passive
  - 0x10 = TX error-passive  
  - 0x20 = Bus-off
- **TEC**: Transmit Error Counter (should be 0 - not transmitting)
- **REC**: Receive Error Counter (should be 0 - no receive errors)

---

## Build and Flash

### Using STM32CubeIDE:
1. **Import Project:**
   - File → Open Projects from File System
   - Select the STM32 folder

2. **Build:**
   - Project → Build All (Ctrl+B)

3. **Flash:**
   - Run → Debug (F11) or Run (Ctrl+F11)

---

## File Structure
```
STM32/
├── Core/
│   ├── Src/
│   │   ├── main.c          # Main application with diagnostics & status monitoring
│   │   ├── CANSPI.c        # CAN SPI high-level driver
│   │   └── MCP2515.c       # MCP2515 low-level SPI driver
│   └── Inc/
│       ├── main.h          # Main header
│       ├── CANSPI.h        # CAN SPI driver header
│       └── MCP2515.h       # MCP2515 registers and definitions
└── README.md
```

---

## Diagnostic Features

### SPI Communication Test
Before initializing CAN, the code verifies SPI is working by:
1. Sending RESET command
2. Reading known register values
3. Confirming expected responses

**If SPI fails**, you'll see:
```
SPI communication FAILED! Check wiring.
Possible issues:
- CS pin not connected or wrong GPIO
- MOSI/MISO/SCK pins wrong
- MCP2515 not powered
```

### Periodic Health Monitoring
Every 2 seconds, the system reports:
- Buffer status (messages waiting)
- Error flags (communication issues)
- Error counters (TX/RX quality)

This helps diagnose problems like:
- Missing termination resistors (REC increases)
- Bit timing mismatch (REC = 128, EFLG = 0x0B)
- Bus-off conditions (EFLG = 0x20)

---

## Notes

- MCP2515 communicates via **SPI**, not native CAN
- STM32F401RE does **not** have built-in CAN controller
- TJA1050 transceiver is usually **built into** MCP2515 modules
- Diagnostic code verifies SPI communication before CAN initialization
- Status monitoring helps debug CAN bus issues in real-time
- 10ms delay in main loop prevents excessive CPU usage