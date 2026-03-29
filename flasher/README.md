# Flasher

## Contents

| File | Description |
|---|---|
| `nodemcu-flasher.zip` | NodeMCU Flasher tool for Windows — used to flash the deauther firmware onto the ESP8266 |

---

## What is NodeMCU Flasher?

NodeMCU Flasher is a Windows GUI tool that writes `.bin` firmware files directly onto ESP8266-based boards over USB. No command line or Arduino IDE required.

- **Original Repository:** [github.com/nodemcu/nodemcu-flasher](https://github.com/nodemcu/nodemcu-flasher)

---

## Requirements

| Item | Details |
|---|---|
| OS | Windows 7 / 8 / 10 / 11 |
| Cable | Micro-USB data cable (not charge-only) |
| Driver | CP2102 or CH340 USB-to-Serial driver |
| Firmware | `../firmware/esp8266_deauther_1mb.bin` |

> **Driver Note:** If your NodeMCU is not detected, install the correct driver:
> - **CP2102 chip:** [Silicon Labs CP210x driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> - **CH340 chip:** [CH340 driver](https://www.wch-ic.com/downloads/CH341SER_EXE.html)
>
> To check which chip your board uses, look at the small IC near the USB port.

---

## Step-by-Step Flashing Guide

### Step 1 — Extract the Flasher
```
Unzip nodemcu-flasher.zip
Open the extracted folder
Run ESP8266Flasher.exe
```

### Step 2 — Connect the NodeMCU
- Plug the NodeMCU into your PC via Micro-USB
- Open **Device Manager** (Win+X → Device Manager)
- Look under **Ports (COM & LPT)** for a new COM port (e.g. `COM3`)
- Note this COM port number

### Step 3 — Select COM Port
- In NodeMCU Flasher, open the **Operation** tab
- Select your COM port from the dropdown (e.g. `COM3`)

### Step 4 — Load the Firmware
- Click the **Config** tab
- Click the gear icon next to the first row
- Browse to and select:
  ```
  ../firmware/esp8266_deauther_1mb.bin
  ```
- Set the flash address to `0x00000`

### Step 5 — Flash Settings
- Click the **Advanced** tab and verify:

| Setting | Value |
|---|---|
| Baudrate | 115200 |
| Flash size | 1 MByte |
| Flash speed | 40 MHz |
| SPI Mode | DIO |

### Step 6 — Flash the Firmware
- Go back to the **Operation** tab
- Click **Flash**
- A progress bar will appear — wait for it to complete (~1–2 minutes)
- The LED on the NodeMCU will blink rapidly during flashing
- You will see a green checkmark ✅ when done

### Step 7 — Verify
- Unplug and re-plug the NodeMCU
- On your phone or laptop, open Wi-Fi settings
- You should see a new network named **`pwned`**
- Connect to it (no password)
- Open a browser and go to `http://192.168.4.1`
- The deauther web interface should load ✅

---

## Troubleshooting

| Problem | Fix |
|---|---|
| COM port not showing up | Install CP2102 or CH340 driver, try a different USB cable |
| Flash fails immediately | Hold the FLASH button on the NodeMCU while clicking Flash |
| Progress bar stuck at 0% | Wrong COM port selected, or cable is charge-only |
| `pwned` network not visible after flash | Re-flash, ensure firmware address is `0x00000` |
| Web UI not loading at 192.168.4.1 | Disable mobile data — phone may route traffic through cellular |

---

## Alternative: Flash via Arduino IDE

If NodeMCU Flasher doesn't work on your system, you can flash using Arduino IDE:

```
1. Install Arduino IDE
2. Add ESP8266 board support URL:
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
3. Install ESP8266 boards via Boards Manager
4. Select: Tools → Board → NodeMCU 1.0 (ESP-12E Module)
5. Select: Tools → Port → your COM port
6. Use: Sketch → Upload (with the .ino from src/)
```

---

## ⚠️ Legal & Ethical Notice

Flash and use this firmware **only on your own devices**.
Unauthorized use on other networks violates the CFAA, GDPR, and local laws.
