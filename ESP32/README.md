# ESP32 CAN Transmitter

## Overview
This code sends CAN messages every second to control an LED on the STM32 receiver.

---

## Hardware Requirements

### ESP32 Board
- **ESP32-S3** (or any ESP32 with TWAI/CAN support)
- Built-in CAN controller (TWAI)

### CAN Transceiver
- **MCP2551** CAN transceiver module

### Connections
| ESP32 Pin | MCP2551 Pin | Description |
|-----------|-------------|-------------|
| GPIO 21   | TX          | CAN TX      |
| GPIO 20   | RX          | CAN RX      |
| 3.3V      | VCC         | Power       |
| GND       | GND         | Ground      |

---

## Software Requirements

### Option 1: Arduino IDE
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install ESP32 board support:
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs":
```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
   - Go to **Tools → Board → Boards Manager**
   - Search "ESP32" and install "esp32 by Espressif Systems"

### Option 2: PlatformIO
1. Install [VS Code](https://code.visualstudio.com/)
2. Install PlatformIO extension
3. Use framework: `arduino`

---

## CAN Bus Configuration

| Parameter | Value |
|-----------|-------|
| Bit Rate  | 500 kbps |
| Mode      | Normal |
| Frame Type| Standard (11-bit ID) |
| Message ID| 0x36 |
| Data Length | 8 bytes |

---

## How It Works

1. **Every 1 second**, the code toggles a boolean variable
2. Sends a CAN message with ID `0x36`:
   - `data[0] = 1` → Turn LED ON
   - `data[0] = 0` → Turn LED OFF
3. STM32 receives this message and controls the LED

---

## Upload Instructions

### Arduino IDE:
1. Open `esp32_can_transmitter.ino`
2. Select board: **Tools → Board → ESP32S3 Dev Module** (or your board)
3. Select port: **Tools → Port → COMX**
4. Click **Upload** button

### PlatformIO:
1. Open folder in VS Code
2. Click **Upload** button in bottom toolbar

---

## Serial Monitor Output

Open Serial Monitor at **115200 baud** to see:
```
=== ESP32 CAN TX Demo ===
[TX] CAN ID: 0x36 Data: 01 00 00 00 00 00 00 00 
[TX] CAN ID: 0x36 Data: 00 00 00 00 00 00 00 00 
[TX] CAN ID: 0x36 Data: 01 00 00 00 00 00 00 00 
```

---


## Pin Configuration (Customizable)

You can change the pins in the code:
```cpp
#define TWAI_TX_PIN GPIO_NUM_21  // Change to your TX pin
#define TWAI_RX_PIN GPIO_NUM_20  // Change to your RX pin
```

---

## Notes

- ESP32's TWAI is hardware CAN controller (no external chip needed)
- MCP2551 is just a **transceiver** (converts logic level to CAN bus levels)
- ESP32 handles all CAN protocol timing automatically