#include <Arduino.h>
#include "driver/twai.h"

// ================= CONFIG =================
#define TWAI_TX_PIN GPIO_NUM_21
#define TWAI_RX_PIN GPIO_NUM_20

#define CAN_ID_CMD 0x36   // CAN ID

// ================= GLOBALS =================
uint32_t lastTxTime = 0;
bool led_cmd = false;

// ================= INIT =================
void setupTWAI() {
  twai_general_config_t g_config =
      TWAI_GENERAL_CONFIG_DEFAULT(TWAI_TX_PIN, TWAI_RX_PIN, TWAI_MODE_NORMAL);

  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
      Serial.println("TWAI install failed!");
      while(1);
  }

  if (twai_start() != ESP_OK) {
      Serial.println("TWAI start failed!");
      while(1);
  }
}

// ================= SEND =================
void sendCommand(bool state) {
  twai_message_t msg = {0};

  msg.identifier = CAN_ID_CMD;   // 0x36
  msg.extd = 0;                  // standard frame
  msg.rtr  = 0;
  msg.data_length_code = 8;

  msg.data[0] = state ? 1 : 0;   // STM32 reads ONLY this
  for (int i = 1; i < 8; i++) {
    msg.data[i] = 0;
  }

  esp_err_t result = twai_transmit(&msg, pdMS_TO_TICKS(100));
  
  // ==== Print to Serial ====
  if (result == ESP_OK) {
    Serial.print("[TX] CAN ID: 0x");
    Serial.print(msg.identifier, HEX);
    Serial.print(" Data: ");
    for (int i = 0; i < msg.data_length_code; i++) {
      Serial.printf("%02X ", msg.data[i]);
    }
    Serial.println();
  } else if (result == 263) { 
    Serial.println("BUS-OFF! Recovery...");
    twai_stop();
    delay(100);
    twai_start();
  } else {
    Serial.printf("[TX] ERROR: %d\n", result);// if the is an issue the error number appears 
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("=== ESP32 CAN TX Demo ===");
  setupTWAI();
}

// ================= LOOP =================
void loop() {
  if (millis() - lastTxTime > 1000) {
    led_cmd = !led_cmd;          // toggle command
    sendCommand(led_cmd);
    lastTxTime = millis();
  }
}
