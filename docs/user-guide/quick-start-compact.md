# Power Meter Quick Start Guide

## 🚀 Basic Operation

### Power On
1. Connect 3.3V power supply to device
2. Display shows startup screen, then main measurement display:
   ```
   ┌─────────────────────┐
   │ V:0.0V  I:0.00A    │
   │ P:0.0W  E:0mWh     │
   └─────────────────────┘
   ```

### User Controls
- **Rotary Encoder**: Navigate menus up/down
- **Button Short Press**: Select menu items
- **Button Long Press (>2.5s)**: Enter main menu from power display

## 📱 Menu Navigation

### From Power Meter Display
1. **Long press button** (2.5s) → Enter Main Menu
2. **Rotate encoder** → Navigate menu options
3. **Short press button** → Select highlighted option
4. **30 seconds timeout** → Auto-return to power display

### Menu Structure
```
Main Menu
├── 1. Power Meter     (return to main display)
├── 2. Peak Values     (view max voltage/current/power)
├── 3. Graphics        (real-time graphs)
│   ├── Voltage Graph
│   ├── Current Graph
│   └── Power Graph
├── 4. Settings
│   └── About (system info)
└── 5. Reset Options
    ├── Reset Energy
    ├── Reset Peaks
    └── Reset All
```

## 🔧 Quick Setup

### Measurement Connections
- **Voltage Input**: Connect to PA4 (0-30V max)
- **Current Input**: Connect current sensor output to PA3 (0-5A max)
- **Power Supply**: 3.3V to VDD pins

### Safety Limits
- ⚠️ **Max Voltage**: 30V DC
- ⚠️ **Max Current**: 5A DC  
- ⚠️ **No AC or mains voltage**

## 📊 Display Information

### Main Display
- **V**: Voltage (0.1V resolution)
- **I**: Current (0.01A resolution)  
- **P**: Power (calculated as V×I)
- **E**: Energy accumulation (mWh or kWh)

### Peak Values Menu
Shows maximum values recorded since last reset:
- Peak Voltage, Peak Current, Peak Power

### Graphics Menu
Real-time plot of selected parameter (32 data points, 200ms update rate)

### Reset Functions
- **Reset Energy**: Clear energy counter only
- **Reset Peaks**: Clear peak values only  
- **Reset All**: Clear all data (also available via 4s button hold)

## 🔍 Quick Troubleshooting

| Problem | Solution |
|---------|----------|
| Blank display | Check 3.3V power, I2C connections (PB6/PB7) |
| No voltage reading | Verify PA4 connection, check 0-30V range |
| No current reading | Check current sensor output to PA3 |
| Button not working | Verify PB3 connection |
| Encoder not working | Check PB4/PB5 connections |

## ⚡ Quick Test
1. Power on device → Should show zero readings
2. Long press button → Main menu appears
3. Rotate encoder → Selection changes
4. Touch 3.3V to voltage input → Reading should increase
5. Remove connection → Reading returns to zero

---
*Compact Reference | V1.0 | STM32L052K6T6 Power Meter*