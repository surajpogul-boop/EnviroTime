# EnviroTime 

> A compact embedded solution built around the LPC2138 microcontroller that brings together real-time timekeeping, configurable alarms, temperature sensing, and a secure settings interface — all driven through a keypad and 16×2 LCD.

---

## What This Project Does

EnviroTime continuously tracks the current time, date, and ambient temperature, displaying everything on an LCD screen. Users can set up to two alarms, protect system settings with a password, and navigate all options through a physical keypad — no serial monitor or PC required during operation.

---

## Key Capabilities

| Capability | Description |
|---|---|
| ⏱ Real-Time Clock | Tracks hours, minutes, seconds, date, and day of week |
| 🔔 Dual Alarms | Two independently configurable alarm slots |
| 😴 Auto-Snooze | Unacknowledged alarm snoozes for 1 minute automatically |
| 🔒 Password Lock | Settings access is gated by a 4-digit PIN |
| 🌡 Temperature Display | LM35 sensor reads ambient temperature via ADC |
| 🖥 LCD Interface | All output rendered on a 16×2 character display |
| ⌨️ Keypad Input | Full navigation using a 4×4 matrix keypad |
| 🔊 Buzzer Alerts | Audible feedback for alarms, errors, and lockouts |

---

## Hardware Components

- **Microcontroller** — LPC2138 (ARM7TDMI-S, 64-pin)
- **Temperature Sensor** — LM35 (analog output, connected to ADC)
- **Display** — 16×2 LCD (8-bit parallel interface)
- **Input** — 4×4 Matrix Keypad
- **Output** — Piezo Buzzer
- **Controls** — 2× Push Buttons (Menu, Alarm Stop)

---

## Pin Configuration

### LCD — 16×2 Display (8-bit mode)

| Signal | LPC2138 Pin |
|--------|-------------|
| Data D0 | P0.8 |
| Data D1 | P0.9 |
| Data D2 | P0.10 |
| Data D3 | P0.11 |
| Data D4 | P0.12 |
| Data D5 | P0.13 |
| Data D6 | P0.14 |
| Data D7 | P0.15 |
| Register Select (RS) | P0.16 |
| Enable (EN) | P0.17 |
| Read/Write (RW) | GND (write-only) |

### 4×4 Matrix Keypad

| Line | LPC2138 Pin |
|------|-------------|
| Row 1 | P1.16 |
| Row 2 | P1.17 |
| Row 3 | P1.18 |
| Row 4 | P1.19 |
| Column 1 | P1.20 |
| Column 2 | P1.21 |
| Column 3 | P1.22 |
| Column 4 | P1.23 |

### LM35 Temperature Sensor

| LM35 Terminal | Connects To |
|---------------|-------------|
| VCC | +5V supply |
| Output (Vout) | P0.28 — ADC channel AD0.1 |
| GND | Ground |

### Control & Output Signals

| Component | LPC2138 Pin | Notes |
|-----------|-------------|-------|
| Buzzer | P0.25 | Alarm and error tones |
| Menu Button | P0.7 | Opens main settings menu |
| Alarm Stop / Snooze | P0.29 | Dismisses or snoozes active alarm |

---

## How the System Works

### Boot Sequence

On startup, the firmware initializes the LCD, RTC registers, keypad scanner, ADC peripheral, alarm logic, and password module. Once ready, the home screen is shown immediately.

### Home Screen

The default screen refreshes continuously with live data:

```
HH:MM:SS  XX.X°C
DD/MM/YYYY  DAY
```

Example:
```
10:48:25  29.5°C
04/06/2026  THU
```

---

### Accessing the Menu

A short press on the **Menu button (P0.7)** brings up the main options:

```
1:RTC  2:ALARM
3:PWD  4:EXIT
```

If no key is pressed within **10 seconds**, the display automatically returns to the home screen.

---

### Option 1 — Edit RTC

The system first prompts for password verification. On success, the user can update:

- **Time** — Hours (0–23), Minutes (0–59), Seconds (0–59)
- **Date** — Day (1–31), Month (1–12), Year (2000–2099)
- **Day of Week** — Value 0–6

Any out-of-range value is rejected immediately:
```
INVALID DATA
```
The buzzer sounds a short error tone alongside the LCD message.

---

### Option 2 — Alarm Settings

Two alarm slots are available. For each alarm, the user enters the trigger time (HH:MM) via keypad.

When the RTC matches an active alarm time, the buzzer turns on and the display switches to:
```
**** ALARM ****
ALARM  HH:MM
```

To stop the alarm, press the **Alarm Stop button (P0.29)** — the buzzer goes silent and the system returns to the home screen.

---

### Auto-Snooze Behaviour

If the alarm runs unacknowledged for **10 seconds**, the system automatically snoozes and reschedules the trigger for **1 minute later**. The LCD briefly shows:

```
AUTO SNOOZE
```

---

### Option 3 — Change Password

The current password must be entered first. On success, the user sets a new 4-digit PIN.

- **Default password:** `1111`
- **Maximum wrong attempts:** 3

After 3 failed tries, the system enters a lockout state:
```
SYSTEM LOCKED
WAIT 60 SEC
```
The buzzer stays active for the duration of the lockout.

---

### Temperature Monitoring

The LM35 produces an output voltage proportional to temperature (10 mV/°C). This is sampled by the LPC2138's internal 10-bit ADC on channel **AD0.1 (P0.28)** and converted using:

```
Temperature (°C) = (ADC Voltage) × 100
```

The result is displayed on the home screen alongside the clock, updated every cycle.

---

## Development Tools

| Tool | Purpose |
|------|---------|
| Keil µVision | Firmware development and compilation |
| Proteus 8 Professional | Circuit simulation and testing |
| Flash Magic | Binary flashing to LPC2138 |

---

## Source File Structure

```
EnviroTime/
├── src/
│   ├── main.c          # Entry point, init, main loop
│   ├── rtc.c           # RTC read/write and edit logic
│   ├── alarm.c         # Alarm compare, trigger, snooze
│   ├── keypad.c        # 4×4 keypad scan and debounce
│   ├── password.c      # PIN verification and change flow
│   ├── input.c         # Generic numeric input handler
│   └── adc.c           # ADC init and voltage-to-temp conversion
├── imgs/               # Schematic, simulation screenshots
└── README.md
```

---

## Possible Extensions

- Store the password in **EEPROM** so it survives power cycles
- Add a **temperature threshold alarm** that triggers when the sensor exceeds a set value
- Support more than two alarm slots with individual enable/disable flags
- Log time and temperature data over **UART to a PC**
- Connect to a Wi-Fi module for **remote alarm configuration or IoT logging**

---

## Author

**SURAJ POGUL**  

