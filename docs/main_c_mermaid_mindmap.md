# STM32 Power Meter - Mermaid Architecture Documentation

## System Architecture Flowchart

```mermaid
flowchart TD
    %% Hardware Layer
    subgraph HW["Hardware Layer"]
        MCU["STM32L052K6T6<br/>32KB Flash, 8KB RAM<br/>32MHz Clock"]
        ADC["ADC1<br/>12-bit Resolution<br/>Channel 3: PA3 (Current)<br/>Channel 4: PA4 (Voltage)"]
        I2C["I2C1<br/>PB6 = SCL<br/>PB7 = SDA"]
        GPIO["GPIO<br/>PB3: Button<br/>PB4/PB5: Encoder<br/>LED Output"]
        TIM["TIM6<br/>100ms Period<br/>Measurement Trigger"]
        UART["UART1<br/>PA9/PA10<br/>Serial Communication"]
        OLED["SSD1306 OLED<br/>128x64 Display"]
    end
    
    %% HAL Layer
    subgraph HAL["HAL Layer"]
        STM32HAL["STM32 HAL"]
        SSD1306DRV["SSD1306 Driver"]
    end
    
    %% Application Layer
    subgraph APP["Application Layer"]
        MAIN["Main Application<br/>main()<br/>SystemClock_Config()<br/>Error_Handler()"]
        
        subgraph MEAS["Measurement System"]
            ADCCONV["ADC Conversion<br/>Get_ADC_Value()<br/>Convert_ADC_to_Voltage()<br/>Convert_ADC_to_Current()"]
            CALC["Power Calculation<br/>Calculate_Power()<br/>Update_Energy()<br/>Update_Peaks()"]
            CONST["Constants<br/>VOLTAGE_SCALE_FACTOR: 7.32f<br/>CURRENT_SCALE_FACTOR: 1.22f<br/>ADC_VREF: 3.3V"]
        end
        
        subgraph INPUT["Input System"]
            BTNHDL["Button Handler<br/>Debouncing: 20ms<br/>Short/Long Press"]
            ENCHDL["Encoder Handler<br/>Debouncing: 5ms<br/>Direction Detection"]
        end
        
        subgraph MENU["Menu System"]
            STATES["Menu States<br/>POWER_METER<br/>MAIN<br/>PEAKS<br/>GRAPHICS<br/>SETTINGS<br/>RESET<br/>ABOUT"]
            NAV["Navigation<br/>Handle_Menu_Navigation()<br/>Handle_Menu_Action()<br/>30s Timeout"]
        end
        
        subgraph DISP["Display System"]
            DISPFUNC["Display Functions<br/>Display_Current_Menu()<br/>Display_Power_Meter()<br/>Display_Graphics()"]
            GRAPHICS["Graphics System<br/>32 Data Points<br/>voltage_history[]<br/>current_history[]<br/>power_history[]"]
        end
        
        subgraph ISR["Interrupt System"]
            TIMERISR["Timer ISR<br/>TIM6_DAC_IRQHandler()<br/>100ms Trigger"]
            GPIOISR["GPIO ISR<br/>EXTI2_3_IRQHandler()<br/>EXTI4_15_IRQHandler()"]
            SYSTICK["SysTick<br/>HAL Tick Update<br/>1ms Interrupt"]
        end
    end
    
    %% External Inputs
    subgraph EXT["External Inputs"]
        VSRC["Voltage Source<br/>Via Voltage Divider"]
        ISRC["Current Source<br/>Via Current Sensor"]
        USER["User Input<br/>Button & Encoder"]
    end
    
    %% Connections - Hardware
    MCU --> ADC
    MCU --> I2C
    MCU --> GPIO
    MCU --> TIM
    MCU --> UART
    I2C --> OLED
    
    %% External Connections
    VSRC --> ADC
    ISRC --> ADC
    USER --> GPIO
    
    %% HAL Connections
    STM32HAL --> MCU
    SSD1306DRV --> OLED
    
    %% Application Connections
    MAIN --> STM32HAL
    MAIN --> SSD1306DRV
    
    %% Interrupt Flow
    TIM --> TIMERISR
    GPIO --> GPIOISR
    TIMERISR --> ADCCONV
    GPIOISR --> BTNHDL
    GPIOISR --> ENCHDL
    
    %% Data Flow
    ADCCONV --> CALC
    CALC --> GRAPHICS
    CALC --> DISPFUNC
    BTNHDL --> NAV
    ENCHDL --> NAV
    NAV --> STATES
    STATES --> DISPFUNC
    DISPFUNC --> SSD1306DRV
    
    %% Styling
    classDef hardware fill:#e1f5fe,stroke:#01579b,stroke-width:2px
    classDef hal fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef app fill:#e8f5e8,stroke:#1b5e20,stroke-width:2px
    classDef external fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef interrupt fill:#ffebee,stroke:#c62828,stroke-width:2px
    
    class MCU,ADC,I2C,GPIO,TIM,UART,OLED hardware
    class STM32HAL,SSD1306DRV hal
    class MAIN,MEAS,ADCCONV,CALC,CONST,INPUT,BTNHDL,ENCHDL,MENU,STATES,NAV,DISP,DISPFUNC,GRAPHICS app
    class VSRC,ISRC,USER external
    class ISR,TIMERISR,GPIOISR,SYSTICK interrupt
```

## Data Flow Diagram

```mermaid
flowchart LR
    %% Input Sources
    subgraph INPUT["Input Sources"]
        VSRC["Voltage Source<br/>Real Voltage"]
        ISRC["Current Source<br/>Real Current"]
        VDIV["Voltage Divider<br/>PA4 Pin<br/>Scale: 7.32"]
        ISENS["Current Sensor<br/>PA3 Pin<br/>Scale: 1.22"]
        BTN["Button<br/>PB3"]
        ENC["Encoder<br/>PB4/PB5"]
    end
    
    %% Hardware Interface
    subgraph HW_IF["Hardware Interface"]
        ADC_CH4["ADC Channel 4<br/>12-bit (0-4095)<br/>Vref: 3.3V"]
        ADC_CH3["ADC Channel 3<br/>12-bit (0-4095)<br/>Vref: 3.3V"]
        GPIO_BTN["GPIO EXTI<br/>Button Events"]
        GPIO_ENC["GPIO EXTI<br/>Encoder Events"]
        TIM_100MS["TIM6<br/>100ms Timer"]
    end
    
    %% Processing Layer
    subgraph PROC["Processing Layer"]
        ADC_CONV["ADC Conversion<br/>Get_ADC_Value()"]
        VOLT_CONV["Voltage Conversion<br/>Scale + Gain + Offset<br/>measured_voltage"]
        CURR_CONV["Current Conversion<br/>Scale Factor<br/>measured_current<br/>Negative Protection"]
        PWR_CALC["Power Calculation<br/>P = V × I<br/>calculated_power"]
        ENERGY_UPD["Energy Update<br/>accumulated_energy<br/>ΔE = P × Δt"]
        PEAK_UPD["Peak Update<br/>peak_voltage<br/>peak_current<br/>peak_power"]
    end
    
    %% Data Storage
    subgraph STORAGE["Data Storage"]
        REALTIME["Real-time Values<br/>voltage, current<br/>power, energy"]
        PEAKS["Peak Values<br/>max V, I, P"]
        HISTORY["Historical Data<br/>voltage_history[32]<br/>current_history[32]<br/>power_history[32]<br/>200ms update"]
    end
    
    %% Input Processing
    subgraph INPUT_PROC["Input Processing"]
        BTN_DEBOUNCE["Button Debounce<br/>20ms threshold<br/>Short/Long Press"]
        ENC_DECODE["Encoder Decode<br/>5ms threshold<br/>Direction Detection"]
        MENU_NAV["Menu Navigation<br/>Handle_Menu_Navigation()<br/>Handle_Menu_Action()"]
    end
    
    %% Menu System
    subgraph MENU_SYS["Menu System"]
        MENU_STATE["Menu State<br/>current_menu<br/>menu_selection<br/>menu_changed"]
        TIMEOUT["Timeout Logic<br/>30s auto-return<br/>last_activity_time"]
    end
    
    %% Display System
    subgraph DISPLAY["Display System"]
        DISP_LOGIC["Display Logic<br/>Display_Current_Menu()<br/>Display_Power_Meter()<br/>Display_Graphics()"]
        OLED_DRV["OLED Driver<br/>ssd1306_WriteString()<br/>ssd1306_UpdateScreen()"]
        SCREEN_OUT["SSD1306 Display<br/>128x64 OLED"]
    end
    
    %% Flow Connections
    VSRC --> VDIV
    ISRC --> ISENS
    VDIV --> ADC_CH4
    ISENS --> ADC_CH3
    BTN --> GPIO_BTN
    ENC --> GPIO_ENC
    
    TIM_100MS --> ADC_CONV
    ADC_CH4 --> ADC_CONV
    ADC_CH3 --> ADC_CONV
    
    ADC_CONV --> VOLT_CONV
    ADC_CONV --> CURR_CONV
    VOLT_CONV --> PWR_CALC
    CURR_CONV --> PWR_CALC
    PWR_CALC --> ENERGY_UPD
    VOLT_CONV --> PEAK_UPD
    CURR_CONV --> PEAK_UPD
    PWR_CALC --> PEAK_UPD
    
    VOLT_CONV --> REALTIME
    CURR_CONV --> REALTIME
    PWR_CALC --> REALTIME
    ENERGY_UPD --> REALTIME
    PEAK_UPD --> PEAKS
    REALTIME --> HISTORY
    
    GPIO_BTN --> BTN_DEBOUNCE
    GPIO_ENC --> ENC_DECODE
    BTN_DEBOUNCE --> MENU_NAV
    ENC_DECODE --> MENU_NAV
    MENU_NAV --> MENU_STATE
    MENU_STATE --> TIMEOUT
    
    REALTIME --> DISP_LOGIC
    PEAKS --> DISP_LOGIC
    HISTORY --> DISP_LOGIC
    MENU_STATE --> DISP_LOGIC
    DISP_LOGIC --> OLED_DRV
    OLED_DRV --> SCREEN_OUT
    
    %% Styling
    classDef input fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef hardware fill:#e1f5fe,stroke:#01579b,stroke-width:2px
    classDef processing fill:#e8f5e8,stroke:#1b5e20,stroke-width:2px
    classDef storage fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef menu fill:#ffebee,stroke:#c62828,stroke-width:2px
    classDef display fill:#e0f2f1,stroke:#00695c,stroke-width:2px
    
    class VSRC,ISRC,VDIV,ISENS,BTN,ENC input
    class ADC_CH4,ADC_CH3,GPIO_BTN,GPIO_ENC,TIM_100MS hardware
    class ADC_CONV,VOLT_CONV,CURR_CONV,PWR_CALC,ENERGY_UPD,PEAK_UPD processing
    class REALTIME,PEAKS,HISTORY storage
    class BTN_DEBOUNCE,ENC_DECODE,MENU_NAV,MENU_STATE,TIMEOUT menu
    class DISP_LOGIC,OLED_DRV,SCREEN_OUT display
```

## Interrupt Flow Sequence

```mermaid
sequenceDiagram
    participant TIM as TIM6 Timer
    participant ISR as Timer ISR
    participant ADC as ADC System
    participant CALC as Calculation
    participant DISP as Display
    participant BTN as Button
    participant ENC as Encoder
    participant GPIO as GPIO ISR
    participant MENU as Menu System
    participant OLED as OLED Display

    Note over TIM: Every 100ms
    TIM->>ISR: TIM6_DAC_IRQHandler()
    activate ISR
    
    ISR->>ADC: Get_ADC_Value(CH4)
    ISR->>ADC: Get_ADC_Value(CH3)
    ADC-->>ISR: voltage_adc, current_adc
    
    ISR->>CALC: Convert_ADC_to_Voltage()
    ISR->>CALC: Convert_ADC_to_Current()
    ISR->>CALC: Calculate_Power()
    CALC-->>ISR: measured values
    
    ISR->>CALC: Update_Energy()
    ISR->>CALC: Update_Peaks()
    ISR->>CALC: Update_Graphics_Data()
    
    alt menu_changed OR display_update_needed
        ISR->>DISP: Display_Current_Menu()
        DISP->>OLED: ssd1306_WriteString()
        DISP->>OLED: ssd1306_UpdateScreen()
    end
    
    deactivate ISR

    Note over BTN: User Input
    BTN->>GPIO: EXTI2_3_IRQHandler()
    activate GPIO
    GPIO->>GPIO: Debounce Logic (20ms)
    
    alt Short Press
        GPIO->>MENU: Handle_Menu_Action(SHORT)
    else Long Press
        GPIO->>MENU: Handle_Menu_Action(LONG)
    end
    
    MENU-->>GPIO: menu_changed = 1
    deactivate GPIO

    Note over ENC: Encoder Rotation
    ENC->>GPIO: EXTI4_15_IRQHandler()
    activate GPIO
    GPIO->>GPIO: Debounce Logic (5ms)
    GPIO->>GPIO: Direction Detection
    
    alt Clockwise
        GPIO->>MENU: Handle_Menu_Navigation(+1)
    else Counter-clockwise
        GPIO->>MENU: Handle_Menu_Navigation(-1)
    end
    
    MENU-->>GPIO: menu_changed = 1
    deactivate GPIO
```

## Menu State Machine

```mermaid
stateDiagram-v2
    [*] --> POWER_METER : System Start
    
    state POWER_METER {
        [*] --> DisplayValues
        DisplayValues : Display V, I, P, E
        DisplayValues : Auto-refresh 100ms
        DisplayValues : Real-time Mode
    }
    
    state MAIN {
        [*] --> ShowOptions
        ShowOptions : 1. Power Meter
        ShowOptions : 2. Peak Values
        ShowOptions : 3. Graphics
        ShowOptions : 4. Settings
        ShowOptions : 5. Reset Options
        ShowOptions : Navigation: Encoder
        ShowOptions : Selection: Button
    }
    
    state PEAKS {
        [*] --> ShowPeaks
        ShowPeaks : Peak Voltage: XX.XX V
        ShowPeaks : Peak Current: XX.XX A  
        ShowPeaks : Peak Power: XX.XX W
        ShowPeaks : Read-only Display
    }
    
    state GRAPHICS_SELECT {
        [*] --> SelectParam
        SelectParam : 1. Voltage Graph
        SelectParam : 2. Current Graph
        SelectParam : 3. Power Graph
        SelectParam : 4. Back
    }
    
    state GRAPHICS {
        [*] --> ShowGraph
        ShowGraph : Real-time Plot
        ShowGraph : 32 Data Points
        ShowGraph : Auto-refresh
        ShowGraph : Selected Parameter
    }
    
    state SETTINGS {
        [*] --> ShowSettings
        ShowSettings : 1. About
        ShowSettings : 2. Back
    }
    
    state RESET {
        [*] --> ShowResetOptions
        ShowResetOptions : 1. Reset Energy
        ShowResetOptions : 2. Reset Peaks
        ShowResetOptions : 3. Back
    }
    
    state ABOUT {
        [*] --> ShowInfo
        ShowInfo : Version: 1.0.0
        ShowInfo : MCU: STM32L052K6
        ShowInfo : Flash: 32KB
        ShowInfo : RAM: 8KB
    }
    
    %% Transitions from POWER_METER
    POWER_METER --> MAIN : Long Press
    
    %% Transitions from MAIN
    MAIN --> POWER_METER : Select "Power Meter"
    MAIN --> PEAKS : Select "Peak Values" 
    MAIN --> GRAPHICS_SELECT : Select "Graphics"
    MAIN --> SETTINGS : Select "Settings"
    MAIN --> RESET : Select "Reset Options"
    
    %% Transitions from sub-menus back to MAIN
    PEAKS --> MAIN : Any Button Press
    GRAPHICS_SELECT --> MAIN : Select "Back"
    SETTINGS --> MAIN : Select "Back"
    RESET --> MAIN : Any Selection
    
    %% Graphics flow
    GRAPHICS_SELECT --> GRAPHICS : Select Parameter
    GRAPHICS --> GRAPHICS_SELECT : Any Button Press
    
    %% Settings flow
    SETTINGS --> ABOUT : Select "About"
    ABOUT --> SETTINGS : Any Button Press
    
    %% Timeout transitions (30s)
    MAIN --> POWER_METER : 30s Timeout
    PEAKS --> POWER_METER : 30s Timeout
    GRAPHICS_SELECT --> POWER_METER : 30s Timeout
    GRAPHICS --> POWER_METER : 30s Timeout
    SETTINGS --> POWER_METER : 30s Timeout
    RESET --> POWER_METER : 30s Timeout
    ABOUT --> POWER_METER : 30s Timeout
    
    note right of POWER_METER
        Default state
        Shows live measurements
        V, I, P, Energy
    end note
    
    note right of MAIN
        Main menu hub
        Access all functions
        Encoder navigation
    end note
    
    note right of GRAPHICS
        Real-time plotting
        Historical data view
        Parameter selection
    end note
```

## Design Patterns Architecture

```mermaid
graph TB
    subgraph PATTERNS["Design Patterns Used"]
        
        subgraph STATE["State Machine Pattern"]
            SM_IMPL["Menu System Implementation"]
            SM_STATES["Defined States<br/>POWER_METER, MAIN<br/>PEAKS, GRAPHICS, etc."]
            SM_TRANS["State Transitions<br/>Button/Encoder Events<br/>Timeout Handling"]
            SM_BENEFITS["Benefits<br/>• Clear Navigation<br/>• Predictable Behavior<br/>• Easy Extension"]
        end
        
        subgraph OBSERVER["Observer Pattern"]
            OBS_IMPL["Interrupt System"]
            OBS_EVENTS["Event Sources<br/>• Timer Events (100ms)<br/>• GPIO Events (Button/Enc)"]
            OBS_HANDLERS["Event Handlers<br/>• Measurement Updates<br/>• Display Updates<br/>• Menu Updates"]
            OBS_BENEFITS["Benefits<br/>• Decoupled Design<br/>• Real-time Response<br/>• Efficient Updates"]
        end
        
        subgraph CIRCULAR["Circular Buffer Pattern"]
            CB_IMPL["Graphics History System"]
            CB_DATA["Data Storage<br/>• voltage_history[32]<br/>• current_history[32]<br/>• power_history[32]"]
            CB_LOGIC["Ring Buffer Logic<br/>• Fixed Size<br/>• Automatic Wrap<br/>• Index Management"]
            CB_BENEFITS["Benefits<br/>• Memory Efficient<br/>• Constant Time Access<br/>• No Reallocation"]
        end
        
        subgraph HAL["Hardware Abstraction Pattern"]
            HAL_IMPL["STM32 HAL Layer"]
            HAL_IFACE["Driver Interfaces<br/>• ADC Operations<br/>• I2C Communication<br/>• GPIO Control<br/>• Timer Management"]
            HAL_PORTABILITY["Platform Independence<br/>• Standardized API<br/>• Hardware Agnostic Code"]
            HAL_BENEFITS["Benefits<br/>• Portability<br/>• Maintainability<br/>• Standardization"]
        end
        
        subgraph SRP["Single Responsibility Pattern"]
            SRP_MODULES["Module Design"]
            SRP_MEAS["Measurement Module<br/>• ADC Conversion<br/>• Calibration<br/>• Calculations"]
            SRP_DISP["Display Module<br/>• OLED Control<br/>• Screen Updates<br/>• Graphics Rendering"]
            SRP_INPUT["Input Module<br/>• Button Handling<br/>• Encoder Processing<br/>• Debouncing"]
            SRP_MENU["Menu Module<br/>• State Management<br/>• Navigation<br/>• Timeout Logic"]
            SRP_BENEFITS["Benefits<br/>• Clear Interfaces<br/>• Easy Testing<br/>• Maintainable Code"]
        end
    end
    
    %% Pattern Relationships
    SM_IMPL --> OBS_IMPL
    OBS_HANDLERS --> CB_IMPL
    CB_IMPL --> HAL_IMPL
    HAL_IMPL --> SRP_MODULES
    
    %% Internal Connections
    SM_STATES --> SM_TRANS
    SM_TRANS --> SM_BENEFITS
    
    OBS_EVENTS --> OBS_HANDLERS
    OBS_HANDLERS --> OBS_BENEFITS
    
    CB_DATA --> CB_LOGIC
    CB_LOGIC --> CB_BENEFITS
    
    HAL_IFACE --> HAL_PORTABILITY
    HAL_PORTABILITY --> HAL_BENEFITS
    
    SRP_MODULES --> SRP_MEAS
    SRP_MODULES --> SRP_DISP
    SRP_MODULES --> SRP_INPUT
    SRP_MODULES --> SRP_MENU
    SRP_MEAS --> SRP_BENEFITS
    SRP_DISP --> SRP_BENEFITS
    SRP_INPUT --> SRP_BENEFITS
    SRP_MENU --> SRP_BENEFITS
    
    %% Styling
    classDef pattern fill:#e3f2fd,stroke:#1976d2,stroke-width:2px
    classDef implementation fill:#f1f8e9,stroke:#388e3c,stroke-width:2px
    classDef benefits fill:#fff3e0,stroke:#f57c00,stroke-width:2px
    
    class SM_IMPL,OBS_IMPL,CB_IMPL,HAL_IMPL,SRP_MODULES pattern
    class SM_STATES,SM_TRANS,OBS_EVENTS,OBS_HANDLERS,CB_DATA,CB_LOGIC,HAL_IFACE,HAL_PORTABILITY,SRP_MEAS,SRP_DISP,SRP_INPUT,SRP_MENU implementation
    class SM_BENEFITS,OBS_BENEFITS,CB_BENEFITS,HAL_BENEFITS,SRP_BENEFITS benefits
```

## Critical Specifications Overview

```mermaid
graph LR
    subgraph TIMING["Timing Requirements"]
        MAIN_TIMING["Main System Timing"]
        MEAS_100MS["Measurement: 100ms<br/>ADC Trigger<br/>Calculate Power<br/>Update Display"]
        DEBOUNCE["Debouncing<br/>Button: 20ms<br/>Encoder: 5ms"]
        TIMEOUT["Menu Timeout<br/>30 seconds<br/>Auto-return to<br/>Power Meter"]
        GRAPHICS_RATE["Graphics Update<br/>200ms Rate<br/>History Buffer<br/>32 Data Points"]
    end
    
    subgraph MEMORY["Memory Constraints"]
        MCU_LIMITS["STM32L052K6T6 Limits"]
        FLASH_32KB["Flash Memory<br/>32KB Total<br/>Code Optimization<br/>Constant Storage"]
        RAM_8KB["RAM Memory<br/>8KB Total<br/>Static Allocation<br/>Buffer Management"]
        DATA_BUFFERS["Data Storage<br/>voltage_history[32]<br/>current_history[32]<br/>power_history[32]<br/>Reduced from 64"]
    end
    
    subgraph MEASUREMENT["Measurement Specifications"]
        ADC_SPECS["ADC Configuration"]
        RESOLUTION["Resolution<br/>12-bit (0-4095)<br/>Voltage Ref: 3.3V<br/>Two Channels"]
        CALIBRATION["Calibration Constants<br/>Voltage Scale: 7.32<br/>Current Scale: 1.22<br/>Voltage Offset: -0.962"]
        ACCURACY["Target Accuracy<br/>Voltage: ±0.1V<br/>Current: ±0.01A<br/>Power: P = V × I"]
        RANGE["Measurement Range<br/>Voltage: 0-30V<br/>Current: 0-5A<br/>Power: 0-150W"]
    end
    
    subgraph SAFETY["Safety Features"]
        PROTECTION["Input Protection"]
        NEG_GUARD["Negative Current Guard<br/>Value Clamping<br/>Range Validation"]
        ERROR_HANDLE["Error Handling<br/>Error_Handler()<br/>Safe Defaults<br/>System Recovery"]
        NOISE_IMMUNITY["Noise Immunity<br/>Debouncing Logic<br/>False Trigger Prevention<br/>Signal Filtering"]
        WATCHDOG["System Monitoring<br/>SysTick Handler<br/>Timeout Detection<br/>Auto Recovery"]
    end
    
    subgraph PERFORMANCE["Performance Metrics"]
        RESPONSE["Response Times"]
        USER_RESP["User Response<br/>< 50ms Button<br/>< 20ms Display<br/>Real-time Feel"]
        DISPLAY_RATE["Display Refresh<br/>100ms Update<br/>Smooth Animation<br/>Flicker-free"]
        POWER_CALC["Calculation Speed<br/>< 1ms Processing<br/>Floating Point<br/>Real-time Updates"]
    end
    
    %% Timing Relationships
    MAIN_TIMING --> MEAS_100MS
    MAIN_TIMING --> DEBOUNCE
    MAIN_TIMING --> TIMEOUT
    MAIN_TIMING --> GRAPHICS_RATE
    
    %% Memory Relationships  
    MCU_LIMITS --> FLASH_32KB
    MCU_LIMITS --> RAM_8KB
    RAM_8KB --> DATA_BUFFERS
    
    %% Measurement Chain
    ADC_SPECS --> RESOLUTION
    RESOLUTION --> CALIBRATION
    CALIBRATION --> ACCURACY
    ACCURACY --> RANGE
    
    %% Safety Chain
    PROTECTION --> NEG_GUARD
    PROTECTION --> ERROR_HANDLE
    PROTECTION --> NOISE_IMMUNITY
    PROTECTION --> WATCHDOG
    
    %% Performance Chain
    RESPONSE --> USER_RESP
    RESPONSE --> DISPLAY_RATE
    RESPONSE --> POWER_CALC
    
    %% Cross-dependencies
    MEAS_100MS --> DISPLAY_RATE
    DEBOUNCE --> NOISE_IMMUNITY
    DATA_BUFFERS --> GRAPHICS_RATE
    RESOLUTION --> POWER_CALC
    
    %% Styling
    classDef timing fill:#e8eaf6,stroke:#3f51b5,stroke-width:2px
    classDef memory fill:#f3e5f5,stroke:#9c27b0,stroke-width:2px
    classDef measurement fill:#e0f2f1,stroke:#009688,stroke-width:2px
    classDef safety fill:#ffebee,stroke:#f44336,stroke-width:2px
    classDef performance fill:#fff8e1,stroke:#ff9800,stroke-width:2px
    
    class MAIN_TIMING,MEAS_100MS,DEBOUNCE,TIMEOUT,GRAPHICS_RATE timing
    class MCU_LIMITS,FLASH_32KB,RAM_8KB,DATA_BUFFERS memory
    class ADC_SPECS,RESOLUTION,CALIBRATION,ACCURACY,RANGE measurement
    class PROTECTION,NEG_GUARD,ERROR_HANDLE,NOISE_IMMUNITY,WATCHDOG safety
    class RESPONSE,USER_RESP,DISPLAY_RATE,POWER_CALC performance
```

## Summary

This comprehensive Mermaid architecture documentation provides clear visual representations of the STM32 Power Meter system using structured diagrams with logical hierarchies:

### Diagram Types Used:

1. **System Architecture Flowchart** - Hierarchical component organization with clear layer separation
2. **Data Flow Diagram** - Left-to-right flow showing data transformation from inputs to outputs
3. **Interrupt Flow Sequence** - Time-based sequence diagram showing interrupt handling
4. **Menu State Machine** - State diagram with transitions and timeout logic
5. **Design Patterns Architecture** - Structured graph showing pattern implementations and benefits
6. **Critical Specifications Overview** - Comprehensive requirements with interconnected dependencies

### Key Improvements over Original:

- **Fixed Syntax Errors**: All diagrams use correct Mermaid syntax
- **Logical Hierarchy**: Clear separation of concerns and system layers
- **Color-coded Components**: Different colors for hardware, software, external, and interrupt components
- **Explicit Relationships**: Clear arrows showing data flow and dependencies
- **Comprehensive Coverage**: All original PlantUML information preserved and enhanced

### Visual Benefits:

- **Strict Logical Structure**: Each diagram follows architectural principles
- **Easy Navigation**: Subgraphs organize related components
- **Professional Appearance**: Consistent styling and color schemes
- **Scalable Representation**: Works well at different zoom levels
- **GitHub Compatible**: Renders properly in GitHub markdown

This documentation serves as both a reference for understanding the current system architecture and a guide for future development and maintenance.