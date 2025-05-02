# 🔌 TRIAC Timer Controller

This Arduino project controls an AC-powered device (like a lamp or motor) using a timer.  
You can adjust the ON and OFF times using a single button and see all the settings on an OLED display.

## 🧰 Components Used

| Component          | Description                                |
|-------------------|--------------------------------------------|
| Arduino UNO/Nano  | Microcontroller that runs the code         |
| TRIAC Relay Module| Controls AC device (via opto-isolated driver like MOC3021) |
| Push Button       | For changing the ON/OFF time settings      |
| OLED Display (I2C)| 128x64 screen for showing time & status    |
| EEPROM (internal) | Stores time settings between reboots       |

## 💡 Features

- Automatically switches the device ON and OFF based on timer
- Editable ON and OFF durations (1 min to 60 min)
- Timer settings saved to EEPROM
- Edit mode with:
  - Long press to enter
  - Short press to increase hours/minutes
  - Long press again to switch between fields
- Display shows:
  - ON and OFF times
  - Countdown timer
  - Edit mode notification
  - ON/OFF status icon

## 🔌 Pin Connections

| Arduino Pin | Connected To             |
|-------------|--------------------------|
| D3          | TRIAC Relay IN pin       |
| D2          | Push Button (active LOW) |
| A4 (SDA)    | OLED SDA                 |
| A5 (SCL)    | OLED SCL                 |
| 5V / GND    | OLED + TRIAC Module      |

## 🧠 How It Works

1. Relay turns ON for a user-set time (e.g. 2:30 minutes)
2. Then turns OFF for another time (e.g. 5:00 minutes)
3. This cycle repeats
4. You can change both ON and OFF times via the push button

## 🛠️ Button Actions

| Action           | Result                             |
|------------------|------------------------------------|
| Long Press       | Enter Edit Mode                    |
| Short Press      | Increase hours or minutes          |
| Long Press Again | Switch between HH / MM / ON / OFF  |
| Inactivity (10s) | Auto exit Edit Mode                |

## ⚠️ Safety Tips

- Be cautious with AC wiring
- Ensure the TRIAC relay module is properly rated and isolated
- Disconnect power while wiring the circuit

---

Built with ❤️ using Arduino, U8g2, EEPROM, and OneButton libraries.
