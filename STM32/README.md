# STM32 CAN Receiver

## Overview
This code receives CAN messages with ID `0x36` and toggles an LED based on the received data.

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

1. **Initialization:**
   - Tests SPI communication with MCP2515
   - Configures bit timing for 8 MHz crystal @ 500 kbps
   - Disables message filtering (accepts all CAN IDs)
   - Switches to Normal mode

2. **Main Loop:**
   - Continuously checks for incoming CAN messages
   - When message with ID `0x36` arrives:
     - `data[0] = 1` → Turn LED ON
     - `data[0] = 0` → Turn LED OFF
   - Prints debug info to UART

---

## Serial Monitor Output

Connect to UART2 at **115200 baud** to see:
```
=== MCP2515 Diagnostic ===
Sending RESET command...
CANSTAT after reset: 0x80 (expected: 0x80)
CANCTRL after reset: 0x87 (expected: 0x87)
SPI communication OK!
MCP2515 Init SUCCESS!
CANSTAT: 0x00, CANCTRL: 0x00
RX: ID=0x36, Data[0]=1
LED ON
RX: ID=0x36, Data[0]=0
LED OFF
```

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
│   │   ├── main.c          # Main application with diagnostic code
│   │   ├── CANSPI.c        # CAN SPI high-level driver
│   │   └── MCP2515.c       # MCP2515 low-level SPI driver
│   └── Inc/
│       ├── main.h          # Main header
│       ├── CANSPI.h        # CAN SPI driver header
│       └── MCP2515.h       # MCP2515 registers and definitions
└── README.md
```

---

## Notes

- MCP2515 communicates via **SPI**, not native CAN
- STM32F401RE does **not** have built-in CAN controller
- TJA1050 transceiver is usually **built into** MCP2515 modules
- Diagnostic code helps verify SPI communication before CAN initialization