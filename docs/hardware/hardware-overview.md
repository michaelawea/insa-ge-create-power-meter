# Hardware Overview

## 📋 Table of Contents
- [System Overview](#system-overview)
- [Main Components](#main-components)
- [Electrical Specifications](#electrical-specifications)
- [Physical Specifications](#physical-specifications)
- [Block Diagram](#block-diagram)
- [Power Requirements](#power-requirements)
- [Environmental Specifications](#environmental-specifications)

## 🎯 System Overview

The STM32L052K6T6 Power Meter is a compact, real-time power measurement device designed for accurate monitoring of DC voltage, current, and power consumption. The system features a high-resolution OLED display and intuitive user interface for comprehensive power analysis.

### Key Capabilities
- **Voltage Range**: 0-30V DC with 0.1V resolution
- **Current Range**: 0-5A DC with 0.01A resolution  
- **Power Calculation**: Real-time P = V × I computation
- **Energy Integration**: Continuous Wh accumulation
- **Peak Detection**: Maximum value tracking
- **Data Visualization**: Real-time graphical display

## 🔧 Main Components

### Primary Components

| Component | Part Number | Function | Interface |
|-----------|-------------|----------|-----------|
| **Microcontroller** | STM32L052K6T6 | Main processor | - |
| **Display** | SSD1306 | 128x64 OLED | I2C |
| **Voltage Sensor** | Voltage Divider | 0-30V input conditioning | ADC |
| **Current Sensor** | Hall Effect/Shunt | 0-5A measurement | ADC |
| **User Interface** | Rotary Encoder + Button | Navigation and control | GPIO |
| **Status LED** | Standard LED | System status indication | GPIO |

### Microcontroller Specifications

```
Part Number:    STM32L052K6T6
Package:        LQFP32 (7x7mm, 0.8mm pitch)
Architecture:   ARM Cortex-M0+
Core Clock:     32 MHz (HSI + PLL)
Flash Memory:   32 KB
RAM Memory:     8 KB  
EEPROM:         2 KB
ADC:            12-bit, 2 channels used
Timers:         Multiple, TIM6 used for 10Hz sampling
Communication:  I2C1, UART1
GPIO:           Available pins for control
Power:          Ultra-low power design
```

### Display Module

```
Model:          SSD1306 Compatible
Size:           128x64 pixels (0.96" typical)
Interface:      I2C (400kHz max, 100kHz used)
Colors:         Monochrome (White/Blue)
Voltage:        3.3V
Current:        ~20mA active, <1µA standby
Fonts:          6x8, 7x10 pixel fonts supported
Graphics:       Pixel-level control, line drawing
Refresh Rate:   Variable, ~10Hz for real-time data
```

### Sensor Interfaces

#### Voltage Measurement Circuit
```
Input Range:    0-30V DC
Conditioning:   Resistive voltage divider
Ratio:          7.32:1 (configurable via VOLTAGE_SCALE_FACTOR)
ADC Input:      0-3.3V (corresponding to 0-30V)
Resolution:     30V / 4096 = 7.3mV per count
Accuracy:       ±1% (with proper calibration)
Protection:     Over-voltage protection recommended
```

#### Current Measurement Circuit  
```
Input Range:    0-5A DC
Method:         Hall effect sensor or shunt resistor
Conditioning:   Signal conditioning to 0-3.3V
Ratio:          1.22:1 (configurable via CURRENT_SCALE_FACTOR)
ADC Input:      0-3.3V (corresponding to 0-5A)
Resolution:     5A / 4096 = 1.22mA per count
Accuracy:       ±1% (with proper calibration)
Isolation:      Depends on sensor type
```

### User Interface Components

#### Rotary Encoder
```
Type:           Incremental rotary encoder
Phases:         2 (A and B channels)
Resolution:     Configurable (software debounced)
Connections:    PB4 (Channel B), PB5 (Channel A)
Pull-ups:       Internal pull-ups enabled
Interrupts:     EXTI4_15 for both channels
Debouncing:     Software debouncing in interrupt handler
```

#### Push Button
```
Type:           Momentary push button
Function:       Menu navigation and selection
Connection:     PB3 (USER_BUTTON)
Pull-up:        Internal pull-up enabled
Interrupt:      EXTI2_3
Debouncing:     Software debouncing (20ms)
Long Press:     2s for special functions, 4s for reset
```

## ⚡ Electrical Specifications

### Power Supply Requirements

| Parameter | Min | Typical | Max | Unit | Notes |
|-----------|-----|---------|-----|------|-------|
| Supply Voltage | 3.0 | 3.3 | 3.6 | V | Clean, regulated supply |
| Supply Current (Active) | 15 | 25 | 40 | mA | With display on |
| Supply Current (Sleep) | - | 1 | 5 | mA | Display off, minimal activity |
| Peak Current | - | 50 | 80 | mA | During display updates |

### ADC Specifications

| Parameter | Value | Unit | Notes |
|-----------|-------|------|-------|
| Resolution | 12 | bits | 4096 counts full scale |
| Reference Voltage | 3.3 | V | Internal VDDA reference |
| Sampling Rate | 10 | Hz | Timer-triggered conversion |
| Conversion Time | <100 | µs | Per channel |
| Input Impedance | >1 | MΩ | High impedance inputs |
| Accuracy | ±1 | LSB | With proper PCB layout |

### Digital I/O Specifications

| Parameter | Min | Typical | Max | Unit |
|-----------|-----|---------|-----|------|
| Input High Voltage | 2.0 | - | 3.6 | V |
| Input Low Voltage | -0.3 | - | 0.8 | V |
| Output High Voltage | 2.4 | 3.3 | - | V |
| Output Low Voltage | - | 0 | 0.4 | V |
| Input Current | - | ±1 | - | µA |
| Output Current | - | ±8 | ±20 | mA |

## 📐 Physical Specifications

### Mechanical Constraints

```
Target Form Factor:   Compact handheld device
Display Size:         0.96" OLED (typical)
Encoder Size:         12mm rotary encoder (typical)
Button Size:          6mm tactile switch (typical)
Connector Types:      Terminal blocks or banana jacks for V/I inputs
Enclosure:           Plastic enclosure with access to controls
Mounting:            Desktop or panel mount options
```

### PCB Considerations

```
Layer Count:         2-4 layers (depending on complexity)
Board Size:          Approximately 50x70mm (estimated)
Trace Width:         
  - Power traces:    0.5mm minimum for main supply
  - Signal traces:   0.2mm for digital signals
  - Analog traces:   0.3mm for ADC inputs
Ground Plane:        Solid ground plane for analog performance
Via Size:            0.2mm drill, 0.4mm pad (standard)
Clearances:          0.2mm minimum for production
```

## 🔄 Block Diagram

```
                    STM32L052K6T6 Power Meter Block Diagram
                                        
    Voltage Input     Current Input           User Interface
    (0-30V DC)        (0-5A DC)              ┌─────────────┐
         │                 │                  │   Rotary    │
         │                 │                  │   Encoder   │
         ▼                 ▼                  │   (PB4/PB5) │
    ┌─────────┐      ┌─────────┐             └──────┬──────┘
    │Voltage  │      │Current  │                    │
    │Divider  │      │Sensor   │             ┌──────▼──────┐
    │7.32:1   │      │1.22:1   │             │Push Button │
    └────┬────┘      └────┬────┘             │   (PB3)     │
         │                │                  └──────┬──────┘
         │ 0-3.3V         │ 0-3.3V                  │
         ▼                ▼                         ▼
    ┌─────────────────────────────────────────────────────────┐
    │                STM32L052K6T6                           │
    │                                                        │
    │  ADC_IN4 (PA4)     ADC_IN3 (PA3)      GPIO (PB3/4/5)  │
    │      │                 │                    │          │
    │      ▼                 ▼                    ▼          │
    │  ┌────────┐       ┌────────┐         ┌──────────┐     │
    │  │12-bit  │       │12-bit  │         │  GPIO    │     │
    │  │ADC     │       │ADC     │         │ Handler  │     │
    │  └───┬────┘       └───┬────┘         └─────┬────┘     │
    │      │                │                    │          │
    │      └────────┬───────┘                    │          │
    │               ▼                            │          │
    │        ┌─────────────┐                     │          │
    │        │   Power     │                     │          │
    │        │ Calculator  │                     │          │
    │        │  P = V×I    │                     │          │
    │        └──────┬──────┘                     │          │
    │               │                            │          │
    │               ▼                            │          │
    │        ┌─────────────┐                     │          │
    │        │   Energy    │                     │          │
    │        │ Integrator  │                     │          │
    │        │   ∫P dt     │                     │          │
    │        └──────┬──────┘                     │          │
    │               │                            │          │
    │               └────────┬───────────────────┘          │
    │                        ▼                              │
    │                 ┌─────────────┐                       │
    │                 │   Display   │                       │
    │                 │  Controller │                       │
    │                 │   & Menu    │                       │
    │                 └──────┬──────┘                       │
    │                        │                              │
    │                  I2C1 (PB6/PB7)   UART1(PA9/PA10)    │
    └────────────────────────┼─────────────────┼────────────┘
                             │                 │
                             ▼                 ▼
                    ┌─────────────────┐  ┌──────────┐
                    │   SSD1306 OLED  │  │  Debug   │
                    │   128x64 Display│  │Interface │
                    │                 │  │(Optional)│
                    └─────────────────┘  └──────────┘
```

## 🔌 Power Requirements

### Power Budget Analysis

| Component | Typical Current | Max Current | Notes |
|-----------|----------------|-------------|-------|
| STM32L052K6T6 | 8mA | 15mA | At 32MHz, active mode |
| SSD1306 OLED | 20mA | 25mA | Full display on |
| Voltage Divider | 1mA | 2mA | Depends on resistor values |
| Current Sensor | 5mA | 10mA | Depends on sensor type |
| LED Indicator | 2mA | 5mA | At reduced brightness |
| **Total System** | **36mA** | **57mA** | Typical/Maximum |

### Power Supply Design

```
Input Voltage:       5V DC (via USB or external adapter)
Regulation:          Linear regulator to 3.3V
Efficiency:          ~65% (linear regulator)
Input Current:       ~55mA at 5V input
Power Consumption:   ~275mW total system
Battery Option:      2x AA batteries with boost converter
Battery Life:        ~50 hours continuous operation (2500mAh)
```

### Power Management Features

```
Sleep Mode:          STM32 STOP mode when inactive
Display Power:       Can be turned off to save power
Measurement Rate:    Configurable (1Hz to 100Hz)
Auto Shutdown:       Configurable timeout
Power Monitoring:    Battery voltage monitoring (if applicable)
Low Power Indicators: LED dimming, display timeout
```

## 🌡️ Environmental Specifications

### Operating Conditions

| Parameter | Min | Typical | Max | Unit | Notes |
|-----------|-----|---------|-----|------|-------|
| Temperature | 0 | 25 | 70 | °C | Commercial grade |
| Humidity | 10 | 50 | 85 | %RH | Non-condensing |
| Altitude | 0 | - | 2000 | m | Standard atmospheric |
| Vibration | - | - | 5 | g | Light mechanical stress |

### Storage Conditions

| Parameter | Min | Max | Unit | Notes |
|-----------|-----|-----|------|-------|
| Temperature | -20 | 85 | °C | Non-operating |
| Humidity | 5 | 95 | %RH | Non-condensing |

### EMC Considerations

```
EMI:                  Minimize switching noise from display
ESD Protection:       Input protection on user interfaces
EMC Compliance:       Follow good PCB layout practices
Cable Shielding:      Shielded cables for analog inputs
Filtering:            RC filters on ADC inputs
Grounding:            Single-point ground system
```

## 🔒 Safety Considerations

### Electrical Safety

```
Input Protection:     Over-voltage protection on measurement inputs
Current Limiting:     Fuse protection for current measurement
Isolation:            Consider isolation for high-voltage measurements  
Enclosure:            Non-conductive enclosure for user safety
Marking:              Proper voltage/current ratings marked
Standards:            Follow IEC 61010 for measurement equipment
```

### Operational Safety

```
Maximum Ratings:      Do not exceed 30V/5A input limits
Polarity:             Reverse polarity protection recommended
Short Circuit:        Protection against measurement circuit shorts
User Interface:       Clear marking of input connections
Documentation:        Comprehensive user safety instructions
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*Hardware Version: Production v1.0*