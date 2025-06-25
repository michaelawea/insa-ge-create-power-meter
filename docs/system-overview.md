# STM32 Power Meter - System Overview

## System Architecture

Looking at the system architecture diagram below, the power meter is built around three main interrupt sources that drive the entire system:

![System Architecture](system-overview-diagram.jpg)

*The diagram above shows how the interrupt system drives the entire power meter operation. The 100ms timer triggers measurements, GPIO interrupts handle user input, and everything flows through the menu system to the display.*

The whole thing works pretty simply - there's a 100ms timer that kicks off measurements, GPIO interrupts that handle user input, and a 1ms SysTick for general housekeeping.

## How the Code Works

When you power on the device, it sits in the main power meter display showing voltage, current, power and energy. Every 100ms, TIM6 fires an interrupt that:

- Grabs ADC readings from PA3 (current sensor) and PA4 (voltage divider)
- Runs the conversion math using those scale factors (7.32 for voltage, 1.22 for current)
- Calculates power and updates the energy counter
- Keeps track of peak values
- Updates the display if something changed

The user interface is handled by separate GPIO interrupts. The button on PB3 gets debounced over 20ms and can detect short presses vs long presses. The rotary encoder on PB4/PB5 gets debounced over 5ms and figures out which direction you're turning.

All the menu logic is pretty straightforward - there are 7 different states (POWER_METER, MAIN, PEAKS, etc.) and the encoder/button combo lets you navigate between them. If you don't touch anything for 30 seconds, it automatically goes back to the main power display.

The graphics system keeps 32 data points of history for voltage, current and power so you can see trends over time. The display functions just read from these buffers and draw whatever menu or graph you're looking at.

## Memory Organization (Plan Mémoire des Variables)

The STM32L052K6T6 has limited memory (32KB Flash, 8KB RAM), so variables are organized efficiently:

**Measurement Variables (RAM: ~24 bytes)**
```c
static float measured_voltage = 0.0f;      // Current voltage reading (4 bytes)
static float measured_current = 0.0f;      // Current current reading (4 bytes) 
static float calculated_power = 0.0f;      // Calculated power P=V*I (4 bytes)
static float accumulated_energy = 0.0f;    // Energy accumulator in Wh (4 bytes)
static uint32_t last_timestamp = 0;        // For energy integration (4 bytes)
```

**Peak Tracking (RAM: ~12 bytes)**
```c
static float peak_voltage = 0.0f;          // Maximum voltage seen (4 bytes)
static float peak_current = 0.0f;          // Maximum current seen (4 bytes) 
static float peak_power = 0.0f;            // Maximum power seen (4 bytes)
```

**Graphics Buffers (RAM: ~384 bytes)**
```c
static float voltage_history[32];          // 32 voltage samples (128 bytes)
static float current_history[32];          // 32 current samples (128 bytes)
static float power_history[32];            // 32 power samples (128 bytes)
```

**User Interface State (RAM: ~20 bytes)**
```c
static MenuState_t current_menu;           // Current menu state (1 byte)
static uint8_t menu_selection;             // Menu selection index (1 byte)
static uint8_t button_state;               // Button state (1 byte)
static uint8_t rotary_counter;             // Encoder position (1 byte)
static uint32_t button_press_time;         // For long press detection (4 bytes)
static uint32_t last_activity_time;        // For 30s timeout (4 bytes)
```

**Hardware Handles (RAM: ~50 bytes)**
```c
ADC_HandleTypeDef hadc;                    // ADC configuration
I2C_HandleTypeDef hi2c1;                   // I2C for OLED display
TIM_HandleTypeDef htim6;                   // Timer for 100ms measurements
```

**Total RAM usage: ~490 bytes out of 8KB available**

## Code Structure and Comments (Code Commenté)

**Main Functions:**
- `Convert_ADC_to_Voltage()` - Applies calibration factors to convert raw ADC to voltage
- `Convert_ADC_to_Current()` - Converts ADC reading to current with sensor scaling  
- `Calculate_Power()` - Simple P = V × I calculation
- `Update_Energy()` - Integrates power over time to get energy
- `Update_Peaks()` - Tracks maximum values seen
- `Handle_Menu_Navigation()` - Processes encoder input for menu navigation
- `Handle_Menu_Action()` - Processes button presses for menu selection

**Key Constants (Flash memory):**
```c
#define VOLTAGE_SCALE_FACTOR    7.32f    // Hardware voltage divider ratio
#define VOLTAGE_GAIN            1.36f    // Calibration slope correction  
#define VOLTAGE_OFFSET         -0.962f   // Calibration zero offset
#define CURRENT_SCALE_FACTOR    1.22f    // Current sensor scaling (5A→3.3V)
#define GRAPH_DATA_POINTS       32       // Reduced from 64 to save RAM
```

## Testing the User Interface

When you want to test the device, here's what you do:

**Start with the power display** - When you first turn it on, you'll see the voltage, current, power and energy readings updating every 100ms.

**Get into the menu** - Hold down the button for more than 2.5 seconds. The screen will switch to show the main menu.

**Navigate around** - Turn the encoder knob clockwise or counter-clockwise to move between menu options. You'll see the selection highlight move up and down.

**Select stuff** - When you find what you want, press the button briefly to select it. This takes you into that submenu.

**Try each menu:**
- Peak Values: Shows the highest voltage, current and power you've measured
- Graphics: Lets you pick voltage, current or power to see a real-time graph
- Settings: Just shows some basic info about the device
- Reset Options: Clears different types of data

**Getting back** - From any submenu, press the button to go back to the main menu. Or just wait 30 seconds and it'll automatically return to the power display.

The whole interface is pretty simple - just the encoder to navigate and the button to select. If you get lost, wait 30 seconds and you'll be back at the main power display.

---
*Written for the STM32L052K6T6 Power Meter project*