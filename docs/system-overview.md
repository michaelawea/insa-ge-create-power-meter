# STM32 Power Meter - System Overview

## 🏗️ Code Logic Architecture

The STM32 Power Meter operates on a timer-driven interrupt system with modular design:

### Interrupt System (Core Control)
```
TIM6 (100ms) ──► Timer ISR ──► Measurement & Display Update
GPIO EXTI    ──► GPIO ISR  ──► User Input Processing  
SysTick (1ms)──► SysTick   ──► HAL Tick Update
```

### System Flow Overview

1. **Timer-Driven Measurement (100ms cycle)**
   - TIM6 triggers ADC conversion every 100ms
   - Read voltage (PA4) and current (PA3) channels
   - Convert raw ADC values using calibration constants
   - Calculate power (P = V × I) and update energy accumulation
   - Update peak value tracking
   - Refresh display if menu state changed

2. **User Input Processing**
   - Button (PB3): Debounced 20ms, detects short/long press
   - Encoder (PB4/PB5): Debounced 5ms, detects rotation direction
   - Input events trigger menu navigation functions
   - Menu system updates current state and selection

3. **Menu State Management**
   - 7 menu states: POWER_METER, MAIN, PEAKS, GRAPHICS, SETTINGS, RESET, ABOUT
   - Navigation controlled by encoder rotation
   - Selection confirmed by button press
   - 30-second timeout returns to POWER_METER display

4. **Display System**
   - Updates triggered by menu state changes
   - Three main display modes:
     - Power Meter: Real-time V, I, P, E values
     - Graphics: Historical data plots (32 data points)
     - Menu: Navigation interface
   - SSD1306 OLED driver handles all screen operations

5. **Data Processing Pipeline**
   ```
   ADC Raw Values → Calibration → Power Calculation → Peak Tracking
                                      ↓
   Graphics History ← Menu System ← Display Functions
   ```

### Key Constants
- **Voltage Scale Factor**: 7.32f (for 0-30V range)
- **Current Scale Factor**: 1.22f (for 0-5A range)  
- **ADC Reference**: 3.3V, 12-bit resolution (0-4095)

## 🎮 User Testing Flow

### Basic Operation Test
1. **Power On**
   - Device starts in POWER_METER mode
   - Display shows: `V:0.0V  I:0.00A  P:0.0W  E:0mWh`

2. **Enter Menu System**
   - **Long press button** (>2.5 seconds)
   - Display changes to MAIN menu
   - Shows: `=== MAIN MENU ===`

3. **Navigate Menu Options**
   - **Rotate encoder clockwise** → Move down menu list
   - **Rotate encoder counter-clockwise** → Move up menu list
   - Available options: Power Meter, Peak Values, Graphics, Settings, Reset Options

4. **Select Menu Items**
   - **Short press button** → Enter selected menu
   - Each menu has specific functions:
     - **Peak Values**: View maximum recorded V, I, P
     - **Graphics**: Select parameter (voltage/current/power) for real-time plotting
     - **Settings**: System information and about screen
     - **Reset Options**: Clear energy, peaks, or all data

5. **Return Navigation**
   - **Press button** in sub-menus → Return to MAIN menu
   - **Wait 30 seconds** → Auto-return to POWER_METER display
   - **Navigate to "Power Meter"** → Manual return to main display

### Complete Test Sequence
```
Power On → POWER_METER Display
    ↓ (Long Press)
MAIN Menu → Navigate with Encoder
    ↓ (Short Press)
PEAKS Menu → View max values → Return
    ↓ (Navigate & Select)
GRAPHICS Menu → Select parameter → View plot → Return
    ↓ (Navigate & Select) 
SETTINGS Menu → View info → Return
    ↓ (Navigate & Select)
RESET Menu → Reset data → Return
    ↓ (30s timeout or select Power Meter)
POWER_METER Display (Complete cycle)
```

## 🔧 Hardware Integration

- **MCU**: STM32L052K6T6 (32KB Flash, 8KB RAM)
- **Display**: SSD1306 128×64 OLED via I2C1
- **Inputs**: 12-bit ADC on PA3 (current), PA4 (voltage)
- **User Interface**: Rotary encoder (PB4/PB5) + button (PB3)
- **Update Rate**: 100ms measurement cycle, 200ms graphics update

## 📊 Key Features

- **Real-time measurement** with 100ms refresh rate
- **Energy accumulation** with mWh/kWh automatic scaling
- **Peak value tracking** for load characterization  
- **Historical plotting** with 32-point circular buffer
- **Menu-driven interface** with encoder navigation
- **Auto-timeout protection** prevents menu lock-up
- **Calibrated measurements** with adjustable scale factors

---
*System Overview v1.0 | STM32L052K6T6 Power Meter*