# EnviroTime

![LPC2148](https://img.shields.io/badge/MCU-LPC2138-blue) ![Proteus](https://img.shields.io/badge/Simulation-Proteus%208-green) ![Build](https://img.shields.io/badge/Build-0%20Errors%200%20Warnings-brightgreen) ![Keil](https://img.shields.io/badge/IDE-Keil%20µVision4-orange)

An LPC2148-based embedded system that tracks real-time clock data, reads ambient temperature via an LM35 sensor, and lets you configure two independent alarms — all through a 4×4 keypad and 16×2 LCD, with every settings screen guarded by a 4-digit PIN.

---

## LCD Screens — What You See at Every Stage

### Boot Splash
Shown once on power-up while all modules initialize:
```
┌────────────────┐
│ EnviroTime     │
│ Initializing...│
└────────────────┘
```

### Home Screen (default)
Live RTC clock + LM35 temperature, updates every second:
```
┌────────────────┐
│ 01:49:38 26.1°C│
│ 04/06/2026 THU │
└────────────────┘
```

### Press P0.7 (Menu Switch)
Main menu appears. Auto-exits back to home after **10 seconds** of no keypress:
```
┌────────────────┐
│ 1:RTC 2:ALARM  │
│ 3:PWD 4:EXIT   │
└────────────────┘
```

---

## Menu Options — Full Walkthrough

### Key `1` → RTC Edit

Password is requested first. On success, the current time and date load into an editable screen:

```
┌────────────────┐     ┌────────────────┐
│ ENTER PASS     │ --> │ 01:49:38       │
│ ****_          │     │ 04/06/2026 D:4 │
└────────────────┘     └────────────────┘
```

- Navigate digits using `+` (forward) and `-` (backward)
- Type digits `0–9` to overwrite
- `C` clears all fields to zero
- `=` validates and saves — invalid ranges show `INVALID DATA` with a buzzer beep
- On success:
```
┌────────────────┐
│ RTC UPDATED    │
│                │
└────────────────┘
```

---

### Key `2` → Alarm Configuration

Password required first. Alarm menu shows current saved times for both alarms:

```
┌────────────────┐
│ A106:30 A207:00│
│ 1:A1 2:A2 3:BCK│
└────────────────┘
```

Select `1` or `2`, then enter alarm time in HH:MM format:

```
┌────────────────┐
│ SET ALARM1     │
│ 06:30_         │
└────────────────┘
```

- `=` confirms → `ALARM SAVED`
- Invalid time (e.g. 25:70) → `INVALID TIME`, re-enter

**When alarm fires** — buzzer activates on `P0.25`, display switches to:
```
┌────────────────┐
│ **** ALARM **** │
│ ALARM 06:30    │
└────────────────┘
```

Press **alarm stop switch on `P0.29`** to silence and return to home.

**Auto-Snooze** — if not dismissed within 10 seconds:
```
┌────────────────┐
│ AUTO SNOOZE    │
│                │
└────────────────┘
```
Alarm reschedules for 1 minute later automatically.

---

### Key `3` → Change Password

Verify current PIN first (default: `1111`). Then enter and confirm new PIN:

```
┌────────────────┐     ┌────────────────┐
│ NEW PASS:      │ --> │ CONFIRM:       │
│ ****_          │     │ ****_          │
└────────────────┘     └────────────────┘
```

Mismatch → `NOT MATCH` with buzzer.  
Success → `PASS UPDATED`.

---

### Wrong PIN Behaviour

| Attempt | LCD message |
|---------|-------------|
| 1st wrong | `WRONG PASS` / `LEFT: 2` |
| 2nd wrong | `WRONG PASS` / `LEFT: 1` |
| 3rd wrong | `SYSTEM LOCKED` / `WAIT 10 SEC` (buzzer held) |

After lockout timer: `TRY AGAIN` — counter resets to 0.

---

### Key `4` → Exit

Returns directly to the home RTC screen with no confirmation.

---

## Pin Configuration

### LCD — 16×2 Display (8-bit mode)

| Signal | LPC2138 Pin |
|--------|-------------|
| Data D0–D7 | P0.8 – P0.15 |
| Register Select (RS) | P0.16 |
| Enable (EN) | P0.17 |
| R/W | GND (write-only) |

### 4×4 Matrix Keypad

| Line | LPC2138 Pin |
|------|-------------|
| Row 1–4 | P1.16 – P1.19 |
| Column 1–4 | P1.20 – P1.23 |

### LM35 + Control Signals

| Component | Pin | Direction |
|-----------|-----|-----------|
| LM35 Vout | P0.28 — ADC AD0.1 | Analog input |
| Buzzer | P0.25 | Output |
| Menu switch (SW1) | P0.7 | Input |
| Alarm stop switch | P0.29 | Input |

---

## Temperature Conversion

LM35 produces 10 mV/°C. Firmware reads the ADC and converts:

```c
current_temp = voltage * 100.0;
```

Displayed as `XX.X°C` at LCD line 1, column 9 — updated every second alongside the clock.

---

## Keypad Reference

| Key | Context | Action |
|-----|---------|--------|
| `1` | Menu | Open RTC edit (password first) |
| `2` | Menu | Open alarm config (password first) |
| `3` | Menu | Change password |
| `4` | Menu | Exit to home |
| `+` | Edit screens | Move cursor forward |
| `-` | Edit screens | Move cursor backward |
| `=` | Edit screens | Confirm / save |
| `C` | Edit screens | Clear all fields |
| `0–9` | Edit / password | Enter digits |

---

## Build Output (Keil µVision)

```
Program Size: Code=13388  RO-data=52  RW-data=152  ZI-data=1256
FromELF: creating hex file...
".\miniproj.axf" - 0 Error(s), 0 Warning(s).
```

---

## Source File Structure

```
EnviroTime/
├── main.c         ← mode state machine, RTC display loop, switch edge detection
├── rtc.c          ← RTC init, get/set time/date/day, RTC_Edit()
├── alarm.c        ← Alarm_Menu(), Alarm_Task(), auto-snooze logic
├── keypad.c       ← 4×4 matrix scan, debounce, GetKey()
├── password.c     ← Password_Verify(), Password_Change(), lockout
├── input.c        ← generic numeric digit input handler
├── adc.c          ← ADC init, Read_ADC(), voltage-to-temperature
├── lcd.c          ← LCD driver: StrLCD, CharLCD, F32LCD, CmdLCD
├── delay.c        ← delay_ms()
├── types.h        ← u8, s32, f32 typedefs
├── defines.h      ← BUZZER, SW_ALARM, pin macros
└── Startup.s      ← ARM7TDMI-S startup assembly
```

---

## Tools

| Tool | Purpose |
|------|---------|
| Keil µVision 4 | Firmware development, compilation, .hex generation |
| Proteus 8 Professional | Full schematic and LCD simulation |
| Flash Magic | ISP flashing of .hex to LPC2138 |

---

## Author

**SURAJ POGUL**  
