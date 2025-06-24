# Quick Start Guide

## 📋 Table of Contents
- [What's in the Box](#whats-in-the-box)
- [Safety Information](#safety-information)
- [Initial Setup](#initial-setup)
- [First Power-On](#first-power-on)
- [Basic Operation](#basic-operation)
- [Making Your First Measurement](#making-your-first-measurement)
- [Understanding the Display](#understanding-the-display)
- [Troubleshooting](#troubleshooting)

## 📦 What's in the Box

### Package Contents
- ✅ STM32L052K6T6 Power Meter (main unit)
- ✅ OLED Display (SSD1306 128x64)
- ✅ User Interface (Rotary encoder + button)
- ✅ Test leads or connectors for voltage/current measurement
- ✅ USB cable for power (if applicable)
- ✅ Quick reference card
- ✅ This documentation

### What You'll Need
- 🔌 **Power Source**: 3.3V DC power supply or USB power
- 📏 **Test Circuit**: Load to measure (LED, resistor, motor, etc.)
- 🔧 **Optional**: Multimeter for calibration verification
- 💻 **Optional**: PC with STM32CubeIDE for firmware updates

## ⚠️ Safety Information

### ⚡ Electrical Safety
```
⚠️  MAXIMUM RATINGS - DO NOT EXCEED:
    Voltage Input:    30V DC maximum
    Current Input:    5A DC maximum
    Power Supply:     3.6V maximum
    
⚠️  POLARITY WARNINGS:
    Check voltage polarity before connection
    Reverse polarity may damage the device
    
⚠️  ISOLATION NOTICE:
    This device is NOT electrically isolated
    Do not use with mains voltage (120V/240V AC)
    Only use with low voltage DC circuits
```

### 🛡️ General Safety
- Always power off the circuit before making connections
- Double-check all connections before applying power
- Start with low voltage/current and work up to rated values
- Do not exceed the specified measurement ranges
- Keep the device away from water and excessive moisture
- Do not operate in explosive or hazardous environments

## 🔧 Initial Setup

### Step 1: Physical Assembly
```
1. Connect the OLED display to the main PCB:
   ├── VCC → 3.3V
   ├── GND → Ground  
   ├── SCL → PB6 (I2C Clock)
   └── SDA → PB7 (I2C Data)

2. Connect the rotary encoder:
   ├── A Channel → PB5
   ├── B Channel → PB4
   ├── Button → PB3
   └── Common → Ground

3. Connect the status LED:
   ├── Anode → PA15 (through current limiting resistor)
   └── Cathode → Ground

4. Install in enclosure (if provided)
```

### Step 2: Power Connection
```
Option A: USB Power (if available)
├── Connect USB cable to power input
├── Device should power on automatically
└── LED should illuminate briefly

Option B: External 3.3V Supply
├── Connect positive to VDD pins
├── Connect negative to GND pins
├── Ensure supply can provide 50-100mA
└── Check polarity before connection
```

### Step 3: Measurement Input Connections
```
Voltage Measurement Setup:
├── Connect voltage input terminals to PA4
├── Ensure voltage divider is properly configured
├── Maximum input: 30V DC
└── Input impedance: >1MΩ

Current Measurement Setup:
├── Connect current sensor output to PA3  
├── Configure current sensor for 0-5A range
├── Ensure sensor output is 0-3.3V
└── Connect current path through sensor
```

## 🚀 First Power-On

### Expected Startup Sequence
```
1. Power Connection:
   └── Apply 3.3V power to device

2. Boot Screen (1-2 seconds):
   ┌─────────────────────────────────┐
   │ Power Meter v1.0               │
   │ Production Ready               │  
   │ STM32L052K6                    │
   │                                │
   └─────────────────────────────────┘

3. Main Display Appears:
   ┌─────────────────────────────────┐
   │ V:0.0V  I:0.00A                │
   │ P:0.0W E:0mWh                  │
   │ ROT:000 BTN:OFF                │
   │                                │
   └─────────────────────────────────┘
```

### Verification Checklist
- ✅ Display illuminates and shows startup message
- ✅ Measurement values appear (should be zero with no load)
- ✅ Button responds when pressed (menu appears)
- ✅ Rotary encoder responds (values change in menu)
- ✅ LED indicator shows appropriate status

### Initial Calibration Check
```
1. Verify Zero Readings:
   ├── Voltage: Should read 0.0V ± 0.1V
   ├── Current: Should read 0.00A ± 0.01A
   └── Power: Should read 0.0W

2. Test Input Response:
   ├── Touch voltage input briefly to 3.3V
   ├── Voltage reading should increase
   └── Remove connection, reading should return to zero

3. Test User Interface:
   ├── Press button → Menu should appear
   ├── Rotate encoder → Selection should change
   └── Press button again → Should enter submenu
```

## 🎮 Basic Operation

### User Interface Overview
```
Physical Controls:
├── Rotary Encoder: Navigate menus, change values
├── Push Button: Select options, enter/exit menus
└── LED Indicator: System status

Control Functions:
├── Short Button Press: Menu selection/navigation
├── Long Button Press (2s): Special functions
├── Very Long Press (4s): Reset all data
├── Rotate Clockwise: Move down/increase values
└── Rotate Counter-clockwise: Move up/decrease values
```

### Navigation Basics
```
From Main Display:
1. Press button once → Enter main menu
2. Rotate encoder → Navigate menu options
3. Press button → Select highlighted option
4. Wait 30 seconds → Auto-return to main display

Menu Structure:
Main Display
├── Main Menu
│   ├── Power Meter (return to main)
│   ├── Peak Values
│   ├── Settings
│   ├── Reset Options
│   └── Graphics Display
├── Peak Values Display
├── Settings Menu
├── Reset Menu
└── Graphics Display
```

### Menu System Tutorial
```
Practice Navigation:

1. Starting from main display, press the button:
   ┌─────────────────────────────────┐
   │ === MAIN MENU ===              │
   │ > Power Meter                  │
   │   Peak Values                  │
   │   Graphics                     │
   └─────────────────────────────────┘

2. Rotate encoder clockwise:
   ┌─────────────────────────────────┐
   │ === MAIN MENU ===              │
   │   Power Meter                  │
   │ > Peak Values                  │
   │   Graphics                     │
   └─────────────────────────────────┘

3. Press button to select Peak Values:
   ┌─────────────────────────────────┐
   │ === PEAK VALUES ===            │
   │ V: 0.0V                        │
   │ I: 0.00A    P: 0.0W            │
   │                                │
   └─────────────────────────────────┘

4. Wait or press button to return to menu
```

## 📊 Making Your First Measurement

### Simple LED Test Circuit
```
Test Setup:
    +3.3V ──[330Ω]──[LED]── GND
                │      │
                │      └── Current sensor input
                └── Voltage measurement point

Expected Results:
├── Voltage: ~1.8-2.2V (LED forward voltage)
├── Current: ~5-10mA (depending on LED and resistor)
├── Power: ~10-20mW
└── Energy: Accumulates over time
```

### Step-by-Step Measurement
```
1. Prepare Test Circuit:
   ├── Build simple LED circuit as shown above
   ├── Do not apply power yet
   └── Verify all connections

2. Connect Power Meter:
   ├── Voltage sense: Connect across LED
   ├── Current sense: Insert in series with circuit
   └── Ensure proper polarity

3. Power On Sequence:
   ├── Power on the meter first
   ├── Verify zero readings
   ├── Apply power to test circuit
   └── Observe measurements appear

4. Verify Measurements:
   ┌─────────────────────────────────┐
   │ V:2.1V  I:0.008A               │
   │ P:0.017W E:1mWh                │
   │ ROT:000 BTN:OFF                │
   │                                │
   └─────────────────────────────────┘

5. Check Menu Functions:
   ├── Press button to access menu
   ├── View peak values
   ├── Check graphics display
   └── Return to main display
```

### Measurement Verification
```
Cross-check with Multimeter (if available):
├── Voltage reading should match DMM within ±5%
├── Current reading should match DMM within ±5%
├── Power = Voltage × Current (verify calculation)
└── Energy should increase over time

Troubleshooting Readings:
├── No voltage: Check connections and polarity
├── No current: Verify series connection through sensor
├── Unstable readings: Check for loose connections
└── Wrong values: Verify calibration factors
```

## 📱 Understanding the Display

### Main Display Layout
```
Line 1: Real-time Measurements
┌─────────────────────────────────┐
│ V:12.3V  I:1.25A               │ ← Voltage and Current
│ P:15.4W E:123mWh               │ ← Power and Energy
│ ROT:001 BTN:OFF                │ ← Encoder position and Button state
│                                │ ← Available for status
└─────────────────────────────────┘

Value Formats:
├── Voltage: XX.X V (0.1V resolution)
├── Current: X.XX A (0.01A resolution)  
├── Power: XX.X W (0.1W resolution)
├── Energy: XXX mWh or X.XXX kWh
├── Encoder: 000-255 (position counter)
└── Button: ON/OFF (current state)
```

### Energy Display Modes
```
Small Energy Values (< 1 Wh):
Display: "E:123mWh"
├── Range: 0-999 mWh
├── Resolution: 1 mWh
└── Precision: ±1mWh

Large Energy Values (≥ 1 Wh):
Display: "E:1.234kWh"  
├── Range: 1.000-99.999 kWh
├── Resolution: 1 Wh (0.001 kWh)
└── Precision: ±0.1%
```

### Status Indicators
```
LED Indicator:
├── Solid On: Normal operation
├── Slow Blink: Warning condition
├── Fast Blink: Error condition
└── Off: System not powered

Display Status:
├── Normal: Values updating every 100ms
├── Menu: Menu system active
├── Graphics: Waveform display mode
└── Blank: Display error or sleep mode
```

## 🔧 Troubleshooting

### Common Issues and Solutions

#### Display Problems
```
Problem: Blank display
Solutions:
├── Check power supply voltage (should be 3.3V ±10%)
├── Verify I2C connections (PB6=SCL, PB7=SDA)
├── Check for loose connections
└── Power cycle the device

Problem: Garbled display
Solutions:
├── Check I2C pull-up resistors (2.2kΩ recommended)
├── Verify display address (typically 0x3C or 0x3D)
├── Check for electromagnetic interference
└── Try different display module
```

#### Measurement Problems
```
Problem: No voltage reading
Solutions:
├── Verify voltage divider configuration
├── Check ADC input connections (PA4)
├── Confirm input voltage is within 0-30V range
├── Test with known voltage source (e.g., 3.3V supply)
└── Check calibration factor (VOLTAGE_SCALE_FACTOR)

Problem: No current reading  
Solutions:
├── Verify current sensor connections
├── Check series connection through current path
├── Confirm sensor output is 0-3.3V for 0-5A
├── Test sensor independently
└── Check calibration factor (CURRENT_SCALE_FACTOR)

Problem: Incorrect readings
Solutions:
├── Calibrate with known accurate meter
├── Adjust scale factors in firmware
├── Check for ground loops or noise
├── Verify sensor specifications
└── Consider temperature effects
```

#### User Interface Problems
```
Problem: Button not responding
Solutions:
├── Check button connections (PB3)
├── Verify internal pull-up is enabled
├── Test button continuity with multimeter
├── Check for mechanical damage
└── Try different button

Problem: Encoder not working
Solutions:  
├── Check encoder connections (PB4, PB5)
├── Verify encoder type (incremental rotary)
├── Test encoder signals with oscilloscope
├── Check for mechanical binding
└── Verify interrupt configuration
```

### Getting Help
```
If problems persist:

1. Check Documentation:
   ├── Hardware overview for connection details
   ├── Pin configuration for signal routing
   ├── Software architecture for behavior
   └── API reference for configuration options

2. Verify Calibration:
   ├── Use known voltage/current sources
   ├── Compare with calibrated multimeter
   ├── Adjust scale factors if needed
   └── Document any changes made

3. Contact Support:
   ├── Gather system information (firmware version, etc.)
   ├── Document symptoms and attempted solutions
   ├── Provide measurement data if possible
   └── Include photos of setup if helpful
```

## 🎯 Next Steps

### Advanced Features to Explore
```
1. Peak Value Tracking:
   ├── Access via Main Menu → Peak Values
   ├── View maximum voltage, current, and power
   ├── Reset peaks via Reset Menu
   └── Use for load characterization

2. Graphics Display:
   ├── Access via Main Menu → Graphics
   ├── View real-time waveforms
   ├── Switch between voltage, current, power
   └── Analyze trends and patterns

3. Reset Functions:
   ├── Reset peaks only (preserves energy)
   ├── Reset energy only (preserves peaks)
   ├── Reset all (4-second button hold)
   └── Useful for different test phases
```

### Measurement Best Practices
```
1. Warm-up Time:
   ├── Allow 5 minutes for stabilization
   ├── Check zero readings before use
   └── Note any drift over time

2. Connection Quality:
   ├── Use twisted pair for differential signals
   ├── Keep measurement leads short
   ├── Ensure secure connections
   └── Shield from electromagnetic interference

3. Calibration Maintenance:
   ├── Verify calibration monthly
   ├── Use NIST-traceable references when possible
   ├── Document calibration results
   └── Adjust software constants as needed
```

### Further Documentation
```
Continue with these guides:
├── User Interface Guide: Detailed menu navigation
├── Measurement Guide: Advanced measurement techniques  
├── Calibration Guide: Precision calibration procedures
├── Hardware Guide: Detailed hardware specifications
└── API Reference: Software configuration options
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*Target Users: First-time users and quick setup*