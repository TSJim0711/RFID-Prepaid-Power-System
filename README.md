A "Pay-as-you-go" asset controller powered by STM32 & LVGL.
May suitable for bussiness like: Sharing Launduary, EV charging station, Party room rental, High value instrument Auth and Rent, ect...

### **KEY:**
  - Multi-Display UI: Dual LCD (LVGL v9), which is [SPI Screen](https://github.com/TSJim0711/RFID-Prepaid-Power-System/tree/main/hardware/lcd_small) and [TFT screen](https://github.com/TSJim0711/RFID-Prepaid-Power-System/tree/main/hardware/lcd).
  - Smart Billing: Time-based dynamic deduction (Pay-as-you-use) with relay interlocking.
  - Task State Machine: Supports Normal/Registration/Top-up modes via MFRC522.
  - Data Integrity: 24-bit signed balance persistence with anti-fraud protection.
  - Generic Framework: Adaptable for EV kiosks, massage chairs, or shared PC stations.

### **TECH:**
  - Graphics: LVGL v9 (Multi-screen & System Layer Events)
  - Storage: MFRC522 Sector/ Block level data management
  - Execution: GPIO Relay Interlock & Bitwise Balance Encoding

### **Using:**
1. Tap card for Registration (No need if done last time)
2. Top-up balance 
3. Relay outputs and spend credit.
---
Note: UI was developed with LLM collaboration to ensure rapid prototyping.
