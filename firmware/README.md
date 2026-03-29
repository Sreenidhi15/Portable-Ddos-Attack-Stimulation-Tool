# Firmware

## File

| File | Description |
|---|---|
| `esp8266_deauther_1mb.bin` | SpacehuhnTech ESP8266 Deauther firmware, 1MB flash build |

---

## About This Firmware

This project uses the **SpacehuhnTech ESP8266 Deauther** firmware — an open-source Wi-Fi
attack and testing tool for the ESP8266 microcontroller.

- **Original Repository:** [github.com/SpacehuhnTech/esp8266_deauther](https://github.com/SpacehuhnTech/esp8266_deauther)
- **Build variant:** 1MB (suitable for NodeMCU ESP8266 boards with 1MB flash)
- **Interface:** Web-based UI accessible via Wi-Fi at `http://192.168.4.1`
- **Default AP SSID:** `pwned`
- **Default AP Password:** *(none by default — set one via Settings after first flash)*

---

## Supported Attack Modes

| Mode | What It Does |
|---|---|
| **Deauthentication** | Sends IEEE 802.11 deauth frames to forcibly disconnect devices from a target AP |
| **Beacon Flooding** | Broadcasts hundreds of fake SSIDs per minute to flood nearby network lists |
| **Probe Request** | Sends rapid probe requests to APs to gather network availability information |

---

## Compatible Hardware

| Board | Flash Size | Compatible |
|---|---|---|
| NodeMCU ESP8266 (ESP-12E) | 4MB | ✅ Use this `.bin` |
| NodeMCU ESP8266 (ESP-12F) | 4MB | ✅ Use this `.bin` |
| Wemos D1 Mini | 4MB | ✅ Use this `.bin` |
| ESP-01 | 1MB | ✅ Use this `.bin` |
| ESP-01 (512KB variant) | 512KB | ❌ Too small |

> For boards with more than 1MB flash, this 1MB build still works fine.

---

## Flash Settings

| Parameter | Value |
|---|---|
| Flash address | `0x00000` |
| Baud rate | `115200` |
| Flash mode | `DIO` |
| Flash size | `1MB` |

---

## How to Flash

See [`../flasher/`](../flasher/) for the NodeMCU Flasher tool and full step-by-step instructions.

---

## After Flashing

1. The NodeMCU will reboot and broadcast a Wi-Fi AP named **`pwned`**
2. Connect to it from any phone or laptop (no password by default)
3. Open a browser and navigate to **`http://192.168.4.1`**
4. Use the **APs**, **Stations**, and **Attacks** tabs to configure and run simulations

---

## ⚠️ Legal & Ethical Notice

This firmware is provided strictly for **educational and research use** in controlled environments.
Do **not** use on networks or devices you do not own or have explicit written permission to test.
Misuse may violate the CFAA, GDPR, and local cybersecurity laws.

**USE IT ONLY ON YOUR OWN NETWORKS AND DEVICES.**
