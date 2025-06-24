# Software Architecture

## 📋 Table of Contents
- [Architecture Overview](#architecture-overview)
- [System Design](#system-design)
- [Module Organization](#module-organization)
- [Data Flow](#data-flow)
- [Task Scheduling](#task-scheduling)
- [Memory Management](#memory-management)
- [State Machine Design](#state-machine-design)
- [Error Handling](#error-handling)

## 🎯 Architecture Overview

The STM32L052K6T6 Power Meter firmware follows a **bare-metal, event-driven architecture** optimized for real-time power measurement applications. The system uses timer-based scheduling combined with interrupt-driven I/O for responsive user interaction and accurate data acquisition.

### Key Architectural Principles

1. **Real-time Processing**: 10Hz measurement cycle ensures accurate power calculations
2. **Event-driven Design**: Interrupt-based handling for user inputs and timing
3. **Modular Structure**: Clear separation between measurement, display, and UI modules
4. **Memory Efficiency**: Optimized for 32KB Flash and 8KB RAM constraints
5. **Deterministic Behavior**: Predictable timing for measurement accuracy

### System Characteristics

```
Operating System:     Bare metal (no RTOS)
Scheduling:          Timer-driven with interrupt priorities
Memory Model:        Static allocation with stack/heap separation
Communication:       Polled I2C, interrupt-driven GPIO
Real-time Constraints: 100ms measurement intervals, <10ms UI response
```

## 🏗️ System Design

### High-Level System Architecture

```
                    STM32L052K6T6 Power Meter Software Architecture
                                        
                     ┌─────────────────────────────────────────┐
                     │              Application Layer           │
                     │  ┌─────────────┐  ┌─────────────────────┐│
                     │  │    Menu     │  │   Power Meter       ││
                     │  │   System    │  │   Application       ││
                     │  └─────────────┘  └─────────────────────┘│
                     └─────────────────────────────────────────┘
                                          │
                     ┌─────────────────────────────────────────┐
                     │            Service Layer                │
                     │  ┌──────────┐ ┌──────────┐ ┌──────────┐ │
                     │  │ Display  │ │  Power   │ │   User   │ │
                     │  │ Manager  │ │Calculator│ │Interface │ │
                     │  └──────────┘ └──────────┘ └──────────┘ │
                     └─────────────────────────────────────────┘
                                          │
                     ┌─────────────────────────────────────────┐
                     │             Driver Layer                │
                     │  ┌──────────┐ ┌──────────┐ ┌──────────┐ │
                     │  │ SSD1306  │ │   ADC    │ │  GPIO    │ │
                     │  │ Driver   │ │ Handler  │ │ Handler  │ │
                     │  └──────────┘ └──────────┘ └──────────┘ │
                     └─────────────────────────────────────────┘
                                          │
                     ┌─────────────────────────────────────────┐
                     │              HAL Layer                  │
                     │     STM32L0xx Hardware Abstraction     │
                     │  ┌──────────┐ ┌──────────┐ ┌──────────┐ │
                     │  │   I2C    │ │   ADC    │ │  Timer   │ │
                     │  │   HAL    │ │   HAL    │ │   HAL    │ │
                     │  └──────────┘ └──────────┘ └──────────┘ │
                     └─────────────────────────────────────────┘
                                          │
                     ┌─────────────────────────────────────────┐
                     │            Hardware Layer               │
                     │         STM32L052K6T6 MCU              │
                     └─────────────────────────────────────────┘
```

### Layer Responsibilities

#### Application Layer
- **Menu System**: Navigation logic and state management
- **Power Meter Application**: Main application logic and coordination

#### Service Layer
- **Display Manager**: Screen content management and graphics coordination
- **Power Calculator**: Measurement processing and energy integration
- **User Interface**: Input handling and response generation

#### Driver Layer
- **SSD1306 Driver**: OLED display communication and graphics primitives
- **ADC Handler**: Analog-to-digital conversion management
- **GPIO Handler**: Digital I/O and interrupt processing

#### HAL Layer
- **STM32L0xx HAL**: Hardware abstraction provided by ST

## 📦 Module Organization

### Core Modules

#### Main Controller (`main.c`)
```c
Responsibilities:
├── System initialization and configuration
├── Main program loop coordination
├── Inter-module communication
├── Error handling and system recovery
└── Hardware abstraction integration

Key Functions:
├── main()                          // Program entry point
├── SystemClock_Config()           // Clock tree setup
├── Timer_Interrupt_Handler()      // Main processing cycle
└── Error_Handler()                // System error management
```

#### Power Measurement Module
```c
Responsibilities:
├── ADC data acquisition
├── Voltage/current conversion
├── Power calculation (P = V × I)
├── Energy integration (∫P dt)
└── Peak value tracking

Key Functions:
├── Convert_ADC_to_Voltage()       // Voltage scaling
├── Convert_ADC_to_Current()       // Current scaling
├── Calculate_Power()              // Power computation
├── Update_Energy()                // Energy accumulation
└── Update_Peaks()                 // Maximum tracking
```

#### Display System Module (`ssd1306/`)
```c
Responsibilities:
├── SSD1306 OLED driver implementation
├── Graphics primitives (pixel, line, text)
├── Font rendering and text layout
├── Screen buffer management
└── I2C communication with display

Key Functions:
├── ssd1306_Init()                 // Display initialization
├── ssd1306_Fill()                 // Screen clearing
├── ssd1306_WriteString()          // Text rendering
├── ssd1306_DrawPixel()           // Pixel manipulation
└── ssd1306_UpdateScreen()        // Buffer transfer
```

#### User Interface Module
```c
Responsibilities:
├── Menu state management
├── User input processing
├── Navigation logic
├── Display content generation
└── User feedback coordination

Key Functions:
├── Display_Current_Menu()         // Screen content management
├── Handle_Menu_Action()           // Input processing
├── User_Button_Interrupt_Handler() // Button handling
└── Rotary_Encoder_Interrupt_Handler() // Encoder handling
```

### Module Dependencies

```
                     Module Dependency Graph
                            
    ┌─────────────┐    ┌─────────────────────┐    ┌─────────────┐
    │    Menu     │───▶│   Power Meter       │◀───│   Display   │
    │   System    │    │   Application       │    │   Manager   │
    └─────────────┘    └─────────────────────┘    └─────────────┘
           │                      │                       │
           │                      ▼                       │
           │            ┌─────────────────────┐           │
           │            │   Power Calculator  │           │
           │            └─────────────────────┘           │
           │                      │                       │
           ▼                      ▼                       ▼
    ┌─────────────┐    ┌─────────────────────┐    ┌─────────────┐
    │    GPIO     │    │      ADC Handler    │    │   SSD1306   │
    │   Handler   │    └─────────────────────┘    │   Driver    │
    └─────────────┘              │                └─────────────┘
           │                     │                       │
           ▼                     ▼                       ▼
    ┌─────────────┐    ┌─────────────────────┐    ┌─────────────┐
    │ GPIO HAL    │    │      ADC HAL        │    │   I2C HAL   │
    └─────────────┘    └─────────────────────┘    └─────────────┘
```

## 🔄 Data Flow

### Measurement Data Flow

```
                         Real-time Measurement Pipeline
                                    
    Hardware              Driver               Service              Application
                                    
    ┌──────────┐         ┌──────────┐         ┌──────────┐         ┌──────────┐
    │ Voltage  │  ADC    │   ADC    │ Raw     │  Power   │ Values  │   Menu   │
    │ Sensor   │────────▶│ Handler  │────────▶│Calculator│────────▶│  System  │
    └──────────┘         └──────────┘         └──────────┘         └──────────┘
                                    
    ┌──────────┐         ┌──────────┐         ┌──────────┐         ┌──────────┐
    │ Current  │  ADC    │   ADC    │ Raw     │  Power   │ Values  │ Display  │
    │ Sensor   │────────▶│ Handler  │────────▶│Calculator│────────▶│ Manager  │
    └──────────┘         └──────────┘         └──────────┘         └──────────┘
                                    
                         ┌──────────┐         ┌──────────┐
                         │  Timer   │ 100ms   │   Main   │
                         │  TIM6    │────────▶│   Loop   │
                         └──────────┘         └──────────┘

Processing Steps:
1. Timer interrupt triggers ADC conversion (10Hz)
2. ADC handler reads voltage and current channels
3. Power calculator converts raw values to engineering units
4. Power calculation: P = V × I
5. Energy integration: E += P × Δt
6. Peak value tracking: max(V), max(I), max(P)
7. Display manager updates screen content
8. Menu system processes any user inputs
```

### User Interface Data Flow

```
                         User Interface Event Pipeline
                                    
    Hardware              Driver               Service              Application
                                    
    ┌──────────┐         ┌──────────┐         ┌──────────┐         ┌──────────┐
    │  Button  │  GPIO   │  GPIO    │ Event   │   User   │ Action  │   Menu   │
    │  (PB3)   │────────▶│ Handler  │────────▶│Interface │────────▶│  System  │
    └──────────┘         └──────────┘         └──────────┘         └──────────┘
                                    
    ┌──────────┐         ┌──────────┐         ┌──────────┐         ┌──────────┐
    │ Encoder  │  GPIO   │  GPIO    │ Event   │   User   │ Action  │ Display  │
    │(PB4/PB5) │────────▶│ Handler  │────────▶│Interface │────────▶│ Manager  │
    └──────────┘         └──────────┘         └──────────┘         └──────────┘
                                    
                         ┌──────────┐         ┌──────────┐
                         │  EXTI    │ IRQ     │   ISR    │
                         │2_3, 4_15 │────────▶│ Handlers │
                         └──────────┘         └──────────┘

Event Processing:
1. User presses button or rotates encoder
2. GPIO interrupt triggers (EXTI2_3 or EXTI4_15)
3. ISR performs debouncing and basic processing
4. User interface service processes the action
5. Menu system updates state accordingly
6. Display manager refreshes screen content
7. Visual feedback provided to user
```

## ⏰ Task Scheduling

### Timing Architecture

The system uses a hybrid scheduling approach combining timer-driven periodic tasks with interrupt-driven event handling.

#### Primary Timer (TIM6) - 10Hz Measurement Cycle
```c
Timer Configuration:
├── Base Frequency: 32 MHz (system clock)
├── Prescaler: 31999 (32MHz → 1kHz)  
├── Period: 99 (1kHz → 10Hz)
├── Interrupt Priority: 3 (medium)
└── Function: Main measurement cycle

Timing Diagram (100ms intervals):
    0ms     100ms    200ms    300ms    400ms    500ms
     │       │        │        │        │        │
     ▼       ▼        ▼        ▼        ▼        ▼
   ┌─────┐ ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐
   │ ADC │ │ ADC │  │ ADC │  │ ADC │  │ ADC │  │ ADC │
   │Read │ │Read │  │Read │  │Read │  │Read │  │Read │
   └─────┘ └─────┘  └─────┘  └─────┘  └─────┘  └─────┘

Tasks per cycle:
├── ADC voltage/current reading (~500µs)
├── Unit conversion and scaling (~100µs)
├── Power calculation P = V×I (~50µs)
├── Energy integration (~100µs)
├── Peak value updates (~50µs)
├── Display content update (~1-5ms)
└── Total: ~2-6ms per 100ms cycle (2-6% CPU usage)
```

#### Interrupt-Driven Events (Asynchronous)
```c
Button Interrupt (EXTI2_3):
├── Priority: 2 (high)
├── Debouncing: 20ms software filter
├── Response time: <1ms
├── Processing: 10-100µs per event
└── Function: Menu navigation

Encoder Interrupt (EXTI4_15):
├── Priority: 1 (highest)
├── Debouncing: Software filter
├── Response time: <500µs
├── Processing: 5-50µs per event
└── Function: Value selection

SysTick (1ms):
├── Priority: 0 (system)
├── Function: HAL timebase
├── Processing: <10µs per tick
└── Used for: Delays, timeouts
```

### Real-time Constraints

#### Critical Timing Requirements
```
Measurement Accuracy:
├── ADC sampling jitter: <1ms (1% of measurement period)
├── Power calculation delay: <5ms
├── Energy integration error: <0.1%
└── Peak detection latency: <100ms

User Interface Responsiveness:
├── Button response: <10ms perceived delay
├── Encoder response: <5ms for smooth operation
├── Menu transitions: <50ms for fluid experience
└── Display updates: <20ms for smooth animation

System Stability:
├── Interrupt latency: <100µs worst case
├── Stack overflow margin: >50% free space
├── Memory fragmentation: Not applicable (static allocation)
└── Watchdog timeout: Not implemented (bare metal system)
```

#### CPU Load Analysis
```
Normal Operation (per 100ms cycle):
├── Timer interrupt processing: 2-6ms (2-6%)
├── Background main loop: <1ms (1%)
├── User interface events: <1ms average (1%)
├── System overhead: <1ms (1%)
└── Total CPU usage: ~5-9% typical, <15% peak

Memory Usage:
├── Stack usage: ~500-800 bytes peak
├── Global variables: ~2KB
├── Display buffer: 1KB (128×64 ÷ 8)
├── Measurement history: 192 bytes (32 points × 3 channels × 2 bytes)
└── Total RAM usage: ~4KB of 8KB available (50%)
```

## 💾 Memory Management

### Memory Layout Strategy

#### Flash Memory Organization (32KB total)
```
Address Range       Size    Usage                    Notes
0x08000000-0x080000FF  256B   Vector table            Fixed by MCU
0x08000100-0x08006FFF  28KB   Application code        Main program
0x08007000-0x08007BFF   3KB   HAL library code       STM32 drivers
0x08007C00-0x08007FFF   1KB   Reserved/unused        Future expansion

Critical size constraints:
├── Current usage: ~31KB (97% of available)
├── Available space: ~1KB for future features
├── Optimization level: -Os (size optimization)
└── Link-time optimization: Enabled for dead code removal
```

#### RAM Memory Organization (8KB total)
```
Address Range       Size    Usage                    Notes
0x20000000-0x200007FF   2KB   Stack space             Growing downward
0x20000800-0x20000BFF   1KB   Display buffer          SSD1306 frame buffer
0x20000C00-0x20000FFF   1KB   Global variables        Static allocation
0x20001000-0x200017FF   2KB   Measurement data        Circular buffers
0x20001800-0x20001FFF   2KB   Available/heap          Dynamic allocation

Memory allocation strategy:
├── Static allocation for all application data
├── No dynamic memory allocation (no malloc/free)
├── Stack/heap collision detection: Manual monitoring
└── Buffer overflow protection: Array bounds checking
```

### Data Structure Optimization

#### Measurement Data Storage
```c
// Optimized data structures for memory efficiency
typedef struct {
    float voltage_history[GRAPH_DATA_POINTS];  // 32 × 4 = 128 bytes
    float current_history[GRAPH_DATA_POINTS];  // 32 × 4 = 128 bytes  
    float power_history[GRAPH_DATA_POINTS];    // 32 × 4 = 128 bytes
    uint8_t history_index;                     // 1 byte
} MeasurementHistory;  // Total: 385 bytes

// Reduced from 64 to 32 data points to save 192 bytes
// Trade-off: Less historical data vs. memory savings

// Current measurement variables (global)
static float measured_voltage;     // 4 bytes
static float measured_current;     // 4 bytes
static float calculated_power;     // 4 bytes
static float accumulated_energy;   // 4 bytes
static float peak_voltage;         // 4 bytes
static float peak_current;         // 4 bytes
static float peak_power;          // 4 bytes
// Total: 28 bytes for current values
```

#### Display Buffer Management
```c
// SSD1306 display buffer (1024 bytes)
static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
// 128 × 64 ÷ 8 = 1024 bytes

// Buffer organization:
// ├── Page 0 (8 rows): ssd1306_buffer[0-127]
// ├── Page 1 (8 rows): ssd1306_buffer[128-255]
// ├── ...
// └── Page 7 (8 rows): ssd1306_buffer[896-1023]

// Double buffering: Not used due to memory constraints
// Direct buffer manipulation for memory efficiency
```

## 🔄 State Machine Design

### Menu System State Machine

```c
// Menu state enumeration
typedef enum {
    MENU_POWER_METER = 0,    // Default measurement display
    MENU_MAIN,               // Main menu selection
    MENU_PEAKS,              // Peak values display
    MENU_SETTINGS,           // Settings and configuration
    MENU_RESET,              // Reset options
    MENU_GRAPHICS,           // Graphics parameter selection
    MENU_ABOUT               // About information
} MenuState;

// State transition diagram:
//
//    ┌─────────────────┐    Button    ┌─────────────────┐
//    │  POWER_METER    │─────────────▶│   MAIN_MENU     │
//    │  (Default)      │              │                 │
//    │                 │◀─────────────│  ◦ Power Meter  │
//    └─────────────────┘   Timeout    │  ◦ Peak Values  │
//           ▲                         │  ◦ Settings     │
//           │                         │  ◦ Reset        │
//           │ 30s                     │  ◦ Graphics     │
//           │ Timeout                 └─────────────────┘
//           │                                  │
//           │                                  │ Encoder
//           │                                  ▼
//    ┌─────────────────┐              ┌─────────────────┐
//    │     PEAKS       │              │    SETTINGS     │
//    │                 │              │                 │
//    │  V: XX.X V      │              │  ◦ About        │
//    │  I: XX.XX A     │              │  ◦ Back         │
//    │  P: XX.X W      │              │                 │
//    └─────────────────┘              └─────────────────┘
//           ▲                                  │
//           │                                  │
//           │                                  ▼
//    ┌─────────────────┐              ┌─────────────────┐
//    │     RESET       │              │     ABOUT       │
//    │                 │              │                 │
//    │  ◦ Reset Peaks  │              │  Power Meter    │
//    │  ◦ Reset Energy │              │  v1.0           │
//    │  ◦ Cancel       │              │  STM32L052K6    │
//    └─────────────────┘              └─────────────────┘
```

### State Transition Logic
```c
// State transition function
void Handle_Menu_Action(uint8_t action_type) {
    switch (current_menu) {
        case MENU_POWER_METER:
            if (action_type == ACTION_BUTTON_PRESS) {
                current_menu = MENU_MAIN;
                menu_selection = 0;
            }
            break;
            
        case MENU_MAIN:
            if (action_type == ACTION_ENCODER_CW) {
                menu_selection = (menu_selection + 1) % MAX_MAIN_ITEMS;
            } else if (action_type == ACTION_ENCODER_CCW) {
                menu_selection = (menu_selection - 1 + MAX_MAIN_ITEMS) % MAX_MAIN_ITEMS;
            } else if (action_type == ACTION_BUTTON_PRESS) {
                switch (menu_selection) {
                    case 0: current_menu = MENU_POWER_METER; break;
                    case 1: current_menu = MENU_PEAKS; break;
                    case 2: current_menu = MENU_SETTINGS; break;
                    case 3: current_menu = MENU_RESET; break;
                    case 4: current_menu = MENU_GRAPHICS; break;
                }
                menu_selection = 0;
            }
            break;
            
        // ... additional states
    }
    
    // Update activity timestamp
    last_activity_time = HAL_GetTick();
    menu_changed = 1;  // Trigger display update
}
```

## ⚠️ Error Handling

### Error Detection Strategy

#### Hardware Error Detection
```c
// ADC conversion errors
HAL_StatusTypeDef adc_status = HAL_ADC_Start(&hadc);
if (adc_status != HAL_OK) {
    // ADC initialization failed
    Error_Handler();
}

// I2C communication errors
HAL_StatusTypeDef i2c_status = HAL_I2C_Transmit(&hi2c1, data, size, timeout);
if (i2c_status != HAL_OK) {
    // Display communication failed
    display_error_count++;
    if (display_error_count > MAX_DISPLAY_ERRORS) {
        // Switch to minimal display mode
        display_mode = DISPLAY_MODE_MINIMAL;
    }
}

// System clock monitoring
if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK) {
    // Clock source unexpected
    SystemClock_Config();  // Attempt recovery
}
```

#### Software Error Detection
```c
// Range checking for measurements
if (measured_voltage < 0.0f || measured_voltage > 35.0f) {
    // Voltage out of expected range
    measured_voltage = 0.0f;  // Safe default
    measurement_error_flags |= ERROR_VOLTAGE_RANGE;
}

// Stack overflow detection (periodic check)
extern uint32_t _estack;  // Linker symbol
uint32_t stack_usage = (uint32_t)&_estack - __get_MSP();
if (stack_usage > STACK_WARNING_THRESHOLD) {
    // Stack usage approaching limit
    system_warnings |= WARNING_STACK_USAGE;
}

// Memory corruption detection
if (measurement_history.history_index >= GRAPH_DATA_POINTS) {
    // Array index corruption
    measurement_history.history_index = 0;
    memory_error_flags |= ERROR_INDEX_CORRUPTION;
}
```

### Error Recovery Mechanisms

#### Graceful Degradation
```c
// Display system failure recovery
if (display_error_count > MAX_DISPLAY_ERRORS) {
    // Continue operation without display
    display_enabled = 0;
    // Use LED for basic status indication
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
}

// Measurement system failure recovery  
if (adc_error_count > MAX_ADC_ERRORS) {
    // Reset ADC peripheral
    HAL_ADC_DeInit(&hadc);
    HAL_Delay(10);
    MX_ADC_Init();
    adc_error_count = 0;
}

// User interface failure recovery
if (button_error_count > MAX_BUTTON_ERRORS) {
    // Disable button interrupts temporarily
    HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
    HAL_Delay(1000);  // Allow settling
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
    button_error_count = 0;
}
```

#### System Reset Strategy
```c
// Critical error handling
void Error_Handler(void) {
    // Disable interrupts
    __disable_irq();
    
    // Save critical data if possible
    // (Note: EEPROM not available due to Flash constraints)
    
    // Indicate error state
    while (1) {
        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
        HAL_Delay(200);  // Fast blink for error indication
    }
    
    // System will require manual reset
    // Could implement watchdog for automatic recovery in future
}

// Soft reset for recoverable errors
void System_Soft_Reset(void) {
    // Reset application state
    Reset_Energy();
    Reset_Peaks();
    current_menu = MENU_POWER_METER;
    menu_selection = 0;
    
    // Clear error flags
    measurement_error_flags = 0;
    system_warnings = 0;
    
    // Reinitialize peripherals
    MX_ADC_Init();
    ssd1306_Init();
}
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*Software Version: Production v1.0*