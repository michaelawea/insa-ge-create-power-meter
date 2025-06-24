# Pin Configuration

## 📋 Table of Contents
- [Pin Assignment Overview](#pin-assignment-overview)
- [Detailed Pin Mapping](#detailed-pin-mapping)
- [Peripheral Configuration](#peripheral-configuration)
- [GPIO Configuration](#gpio-configuration)
- [Alternate Function Mapping](#alternate-function-mapping)
- [Pin Constraints](#pin-constraints)
- [Layout Considerations](#layout-considerations)

## 🎯 Pin Assignment Overview

The STM32L052K6T6 uses a carefully optimized pin assignment to maximize functionality while minimizing PCB complexity. The 32-pin LQFP package provides sufficient I/O for the power meter application.

### Pin Utilization Summary

| Function Category | Pins Used | Total Available | Utilization |
|-------------------|-----------|-----------------|-------------|
| **ADC Inputs** | 2 | 16 | 12.5% |
| **I2C Communication** | 2 | 2 | 100% |
| **UART Debug** | 2 | 2 | 100% |
| **GPIO (User Interface)** | 3 | ~20 | 15% |
| **GPIO (Status LED)** | 1 | ~20 | 5% |
| **Power & Clock** | 4 | 4 | 100% |
| **Unused/Reserved** | 18 | - | - |

## 📌 Detailed Pin Mapping

### Complete Pin Assignment Table

| Pin # | Pin Name | Function | Type | Signal | Notes |
|-------|----------|----------|------|--------|-------|
| 1 | VDD | Power | PWR | +3.3V | Main power supply |
| 2 | PC14-OSC32_IN | Clock | CLK | OSC32_IN | 32.768kHz crystal (optional) |
| 3 | PC15-OSC32_OUT | Clock | CLK | OSC32_OUT | 32.768kHz crystal (optional) |
| 4 | NRST | Reset | RST | RESET | System reset (pull-up) |
| 5 | VDDA | Power | PWR | +3.3V | Analog supply |
| 6 | PA0 | GPIO | I/O | - | Available for expansion |
| 7 | PA1 | GPIO | I/O | - | Available for expansion |
| 8 | PA2 | GPIO | I/O | - | Available for expansion |
| 9 | **PA3** | **ADC** | **AI** | **CURRENT_IN** | **Current measurement input** |
| 10 | **PA4** | **ADC** | **AI** | **VOLTAGE_IN** | **Voltage measurement input** |
| 11 | PA5 | GPIO | I/O | - | Available for expansion |
| 12 | PA6 | GPIO | I/O | - | Available for expansion |
| 13 | PA7 | GPIO | I/O | - | Available for expansion |
| 14 | PB0 | GPIO | I/O | - | Available for expansion |
| 15 | PB1 | GPIO | I/O | - | Available for expansion |
| 16 | VSS | Power | PWR | GND | Ground |
| 17 | VDD | Power | PWR | +3.3V | Power supply |
| 18 | **PA9** | **UART** | **AF** | **UART1_TX** | **Debug output** |
| 19 | **PA10** | **UART** | **AF** | **UART1_RX** | **Debug input** |
| 20 | PA11 | GPIO | I/O | - | Available for expansion |
| 21 | PA12 | GPIO | I/O | - | Available for expansion |
| 22 | PA13 | Debug | DBG | SWDIO | SWD debug interface |
| 23 | PA14 | Debug | DBG | SWCLK | SWD debug interface |
| 24 | **PA15** | **GPIO** | **DO** | **LED_RED** | **Status LED output** |
| 25 | **PB3** | **GPIO** | **DI** | **USER_BUTTON** | **User button input** |
| 26 | **PB4** | **GPIO** | **DI** | **ROT_CHB** | **Rotary encoder B** |
| 27 | **PB5** | **GPIO** | **DI** | **ROT_CHA** | **Rotary encoder A** |
| 28 | **PB6** | **I2C** | **AF** | **I2C1_SCL** | **Display clock** |
| 29 | **PB7** | **I2C** | **AF** | **I2C1_SDA** | **Display data** |
| 30 | BOOT0 | Config | CFG | BOOT | Boot configuration (GND) |
| 31 | VSS | Power | PWR | GND | Ground |
| 32 | VDD | Power | PWR | +3.3V | Power supply |

**Legend:**
- **Bold**: Actively used pins
- AI: Analog Input, DI: Digital Input, DO: Digital Output, AF: Alternate Function
- PWR: Power, CLK: Clock, RST: Reset, DBG: Debug, CFG: Configuration

## 🔧 Peripheral Configuration

### ADC Configuration (ADC1)

```c
// ADC Channel Assignment
PA3 → ADC1_IN3  (CURRENT_IN)
PA4 → ADC1_IN4  (VOLTAGE_IN)

// ADC Configuration Parameters
Resolution:         12-bit (4096 counts)
Reference:          VDDA (3.3V)
Sampling Time:      Default (minimum for 12-bit)
Conversion Mode:    Single conversion
Trigger Source:     Software trigger
DMA:               Not used (polling mode)
Interrupt:         Conversion complete (optional)

// Pin Configuration
GPIO Mode:          Analog
Pull-up/Pull-down:  None (floating)
Speed:              Not applicable for analog
Drive Strength:     Not applicable for analog
```

### I2C1 Configuration

```c
// I2C Pin Assignment
PB6 → I2C1_SCL  (Display Clock)
PB7 → I2C1_SDA  (Display Data)

// I2C Configuration Parameters
Speed:              Standard mode (100 kHz)
Addressing:         7-bit addressing
Master Mode:        Yes (MCU is master)
Clock Stretching:   Enabled
General Call:       Disabled
Timing Register:    0x00B07CB4 (for 100kHz at 32MHz)

// Pin Configuration
GPIO Mode:          Alternate Function (AF1)
Output Type:        Open-drain
Pull-up:            Internal pull-up enabled
Speed:              Medium speed
Drive Strength:     Standard
```

### UART1 Configuration

```c
// UART Pin Assignment
PA9  → UART1_TX  (Debug Transmit)
PA10 → UART1_RX  (Debug Receive)

// UART Configuration Parameters
Baud Rate:          115200 bps
Data Bits:          8
Parity:             None
Stop Bits:          1
Flow Control:       None
Mode:               TX/RX enabled

// Pin Configuration
GPIO Mode:          Alternate Function (AF4)
Output Type:        Push-pull
Pull-up:            None (TX), Internal pull-up (RX)
Speed:              High speed
Drive Strength:     Standard
```

### Timer Configuration (TIM6)

```c
// Timer Configuration for 10Hz Interrupt
Timer:              TIM6 (basic timer)
Clock Source:       Internal clock (32 MHz)
Prescaler:          31999 (32MHz → 1kHz)
Period:             99 (1kHz → 10Hz)
Interrupt:          Update interrupt enabled
Priority:           Medium priority (3)

// No external pins required for TIM6
```

## 🔲 GPIO Configuration

### Input Pins Configuration

#### User Button (PB3)
```c
Pin:                PB3
Function:           USER_BUTTON
GPIO Mode:          Input
Pull-up/Pull-down:  Internal pull-up
Trigger:            Rising and falling edge
Interrupt:          EXTI2_3_IRQn
Priority:           High priority (2)
Debouncing:         Software (20ms)

// External circuit requirements
External Pull-up:   Not required (internal used)
Switch Type:        Normally open momentary
Switch Rating:      Low voltage (3.3V, <1mA)
```

#### Rotary Encoder Channel A (PB5)
```c
Pin:                PB5
Function:           ROT_CHA (Encoder Channel A)
GPIO Mode:          Input
Pull-up/Pull-down:  Internal pull-up
Trigger:            Rising and falling edge
Interrupt:          EXTI4_15_IRQn
Priority:           Medium priority (1)
Debouncing:         Software

// External circuit requirements
Encoder Type:       Incremental rotary encoder
Output Type:        Open collector or mechanical contacts
Resolution:         Any (software configurable)
```

#### Rotary Encoder Channel B (PB4)
```c
Pin:                PB4
Function:           ROT_CHB (Encoder Channel B)
GPIO Mode:          Input
Pull-up/Pull-down:  Internal pull-up
Trigger:            Rising and falling edge
Interrupt:          EXTI4_15_IRQn
Priority:           Medium priority (1)
Debouncing:         Software

// Configuration identical to Channel A
```

### Output Pins Configuration

#### Status LED (PA15)
```c
Pin:                PA15
Function:           LED_RED (Status indicator)
GPIO Mode:          Output
Output Type:        Push-pull
Pull-up/Pull-down:  None
Speed:              Low speed
Drive Strength:     Standard (up to 8mA)

// External circuit requirements
LED Type:           Standard LED (red)
Current Limiting:   External resistor required
Typical Values:     220Ω - 1kΩ resistor
Forward Voltage:    1.8V - 2.2V (red LED)
Forward Current:    2mA - 10mA
```

### Analog Input Pins Configuration

#### Voltage Measurement (PA4)
```c
Pin:                PA4
Function:           VOLTAGE_IN (ADC1_IN4)
GPIO Mode:          Analog
Pull-up/Pull-down:  None (floating)
ADC Channel:        ADC1_IN4
Input Range:        0V - 3.3V (at MCU pin)
Measurement Range:  0V - 30V (after voltage divider)

// External circuit requirements
Signal Conditioning: Voltage divider (R1:R2 = 7.32:1)
Input Protection:   Optional TVS diode
Filtering:          Optional RC low-pass filter
Maximum Input:      30V DC (with voltage divider)
```

#### Current Measurement (PA3)
```c
Pin:                PA3
Function:           CURRENT_IN (ADC1_IN3)
GPIO Mode:          Analog
Pull-up/Pull-down:  None (floating)
ADC Channel:        ADC1_IN3
Input Range:        0V - 3.3V (at MCU pin)
Measurement Range:  0A - 5A (after current sensor)

// External circuit requirements
Signal Conditioning: Current sensor output conditioning
Sensor Type:        Hall effect or shunt-based
Output Range:       0V - 3.3V for 0A - 5A
Filtering:          Optional RC low-pass filter
Isolation:          Recommended for high current measurements
```

## 🔄 Alternate Function Mapping

### STM32L052K6T6 Alternate Functions Used

| Pin | AF0 | AF1 | AF2 | AF3 | AF4 | AF5 | AF6 | AF7 | Used |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|------|
| PA9 | - | - | - | - | **USART1_TX** | - | - | - | **AF4** |
| PA10| - | - | - | - | **USART1_RX** | - | - | - | **AF4** |
| PB6 | - | **I2C1_SCL** | - | - | - | - | - | - | **AF1** |
| PB7 | - | **I2C1_SDA** | - | - | - | - | - | - | **AF1** |

### Unused Alternate Functions (Available for Expansion)

```
SPI1:     Available on PA5, PA6, PA7, PB3, PB4, PB5
SPI2:     Available on PB12, PB13, PB14, PB15 (not present on LQFP32)
USART2:   Available on PA2, PA3, PA14, PA15
I2C2:     Available on PB10, PB11 (not present on LQFP32)
TIM2:     Available on PA0, PA1, PA2, PA3, PA5, PA15, PB3
TIM21:    Available on PA2, PA3, PB13, PB14 (some not on LQFP32)
TIM22:    Available on PA6, PA7, PB4, PB5
```

## ⚠️ Pin Constraints

### Hardware Limitations

#### ADC Input Constraints
```
Maximum Input Voltage:  3.6V (absolute maximum)
Recommended Range:      0V - 3.3V (rail-to-rail)
Input Impedance:        Very high (>1MΩ)
Leakage Current:        <1µA typical
Temperature Coefficient: <100ppm/°C
Settlement Time:        <1µs for full scale
```

#### I2C Constraints
```
Maximum Frequency:      400kHz (Fast mode)
Used Frequency:         100kHz (Standard mode)
Pull-up Requirements:   2.2kΩ - 10kΩ typical
Bus Capacitance:        <400pF for 100kHz
Rise Time:              <1µs for 100kHz
Fall Time:              <300ns for 100kHz
```

#### GPIO Current Limitations
```
Per Pin Source:         8mA (typical), 20mA (absolute max)
Per Pin Sink:           8mA (typical), 20mA (absolute max)
Total Port Source:      80mA maximum
Total Port Sink:        80mA maximum
Total Device Current:   120mA maximum
```

### Software Constraints

#### Interrupt Priority Conflicts
```
Highest Priority (0):   Not used (reserved for critical)
High Priority (1):      EXTI4_15 (Rotary encoder)
Medium Priority (2):    EXTI2_3 (User button)
Low Priority (3):       TIM6 (Measurement timer)
Lowest Priority (4+):   Available for expansion
```

#### Resource Conflicts
```
ADC1:           Shared between PA3 and PA4 (no conflict, sequential sampling)
EXTI4_15:       Shared between PB4 and PB5 (handled in same ISR)
I2C1:           Dedicated to display (no sharing)
UART1:          Dedicated to debug (optional, can be freed)
TIM6:           Dedicated to measurement timing
```

## 📐 Layout Considerations

### PCB Routing Guidelines

#### Analog Signal Routing
```
Voltage Input (PA4):
- Keep traces short and direct
- Use wide traces for better noise immunity
- Avoid routing near digital switching signals
- Consider guard traces or ground plane isolation
- Add RC filtering near the MCU pin

Current Input (PA3):
- Minimize trace length to reduce noise pickup
- Use dedicated analog ground return
- Route away from high-frequency digital signals
- Consider shielded cable connection points
- Add ferrite bead for EMI suppression
```

#### Digital Signal Routing
```
I2C Bus (PB6/PB7):
- Keep trace lengths matched between SCL and SDA
- Use 50Ω characteristic impedance (not critical at 100kHz)
- Add 2.2kΩ pull-up resistors close to MCU
- Minimize stub lengths to display connector
- Consider ESD protection at connector

UART Debug (PA9/PA10):
- Standard digital routing practices
- Can use longer traces if needed
- Add ESD protection if connector is exposed
- Consider using 3.3V levels (no level translation needed)

GPIO Controls (PB3/PB4/PB5):
- Short traces to minimize noise pickup
- Use internal pull-ups (no external components needed)
- Route away from analog signals
- Consider RC filtering for very noisy environments
```

#### Power Distribution
```
Power Planes:
- Use solid 3.3V and ground planes where possible
- Minimize via count in power paths
- Place decoupling capacitors close to MCU power pins
- Use separate analog and digital ground areas if needed

Decoupling Strategy:
- 100nF ceramic capacitor per power pin pair
- 10µF tantalum or ceramic for bulk decoupling
- Place decoupling within 5mm of power pins
- Use multiple vias for ground connections
```

### Connector Pinouts

#### Measurement Input Connector
```
Pin 1:  Voltage Input Positive (to voltage divider)
Pin 2:  Voltage Input Ground (system ground)
Pin 3:  Current Sensor Positive (to PA3)
Pin 4:  Current Sensor Ground (system ground)
Pin 5:  Shield/Chassis Ground (optional)

Connector Type: Terminal block or banana jack
Rating:         30V, 5A minimum
Spacing:        5mm pitch for terminal blocks
```

#### Debug/Programming Connector
```
Pin 1:  VDD (+3.3V)
Pin 2:  SWDIO (PA13)
Pin 3:  SWCLK (PA14)
Pin 4:  NRST (Reset)
Pin 5:  GND
Pin 6:  UART_TX (PA9) - optional
Pin 7:  UART_RX (PA10) - optional

Connector Type: 2.54mm header (standard SWD)
Standard:       ARM 10-pin or 7-pin SWD connector
```

## 🔧 Configuration Code Example

### GPIO Initialization Code
```c
// GPIO Initialization Function
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable GPIO clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Configure LED output (PA15)
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

  // Configure user button (PB3)
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  // Configure rotary encoder (PB4, PB5)
  GPIO_InitStruct.Pin = ROT_CHB_Pin|ROT_CHA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure EXTI interrupts
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*Hardware Revision: Production v1.0*