# Function Reference

## 📋 Table of Contents
- [Core Functions](#core-functions)
- [Measurement Functions](#measurement-functions)
- [Display Functions](#display-functions)
- [User Interface Functions](#user-interface-functions)
- [Utility Functions](#utility-functions)
- [Hardware Abstraction](#hardware-abstraction)
- [Configuration Functions](#configuration-functions)
- [Error Handling](#error-handling)

## ⚙️ Core Functions

### System Initialization

#### `main()`
```c
int main(void)
```
**Description**: Main program entry point and system initialization  
**Parameters**: None  
**Returns**: `int` - Never returns (infinite loop)  
**Usage**:
```c
// Called automatically at system startup
// Initializes all peripherals and enters main loop
```

#### `SystemClock_Config()`
```c
void SystemClock_Config(void)
```
**Description**: Configures the system clock to 32MHz using HSI+PLL  
**Parameters**: None  
**Returns**: `void`  
**Details**:
- HSI: 16MHz internal oscillator
- PLL: ×4 multiplication, ÷2 division = 32MHz
- AHB/APB1/APB2: No division (32MHz)

#### `Error_Handler()`
```c
void Error_Handler(void)
```
**Description**: System error handler - infinite loop with LED indication  
**Parameters**: None  
**Returns**: `void` - Never returns  
**Behavior**: Fast LED blinking to indicate error state

### Timer Interrupt Handler

#### `Timer_Interrupt_Handler()`
```c
void Timer_Interrupt_Handler(void)
```
**Description**: Main measurement cycle handler (called every 100ms)  
**Parameters**: None  
**Returns**: `void`  
**Functionality**:
- ADC voltage/current reading
- Power calculation and energy integration
- Peak value tracking
- Display updates
- Menu timeout handling

## 📊 Measurement Functions

### ADC Conversion Functions

#### `Convert_ADC_to_Voltage()`
```c
float Convert_ADC_to_Voltage(uint32_t adc_value)
```
**Description**: Converts raw ADC value to real voltage measurement  
**Parameters**:
- `adc_value`: Raw ADC reading (0-4095)  
**Returns**: `float` - Voltage in volts (0-30V range)  
**Formula**: `V = (adc_value/4095) × 3.3V × 7.32`  
**Example**:
```c
uint32_t raw_adc = 2048;  // Mid-scale reading
float voltage = Convert_ADC_to_Voltage(raw_adc);
// Result: ~15V (half of 30V range)
```

#### `Convert_ADC_to_Current()`
```c
float Convert_ADC_to_Current(uint32_t adc_value)
```
**Description**: Converts raw ADC value to real current measurement  
**Parameters**:
- `adc_value`: Raw ADC reading (0-4095)  
**Returns**: `float` - Current in amperes (0-5A range)  
**Formula**: `I = (adc_value/4095) × 3.3V × 1.22`  
**Example**:
```c
uint32_t raw_adc = 1024;  // Quarter-scale reading
float current = Convert_ADC_to_Current(raw_adc);
// Result: ~1.25A (quarter of 5A range)
```

### Power Calculation Functions

#### `Calculate_Power()`
```c
float Calculate_Power(float voltage, float current)
```
**Description**: Calculates instantaneous power from voltage and current  
**Parameters**:
- `voltage`: Voltage in volts  
- `current`: Current in amperes  
**Returns**: `float` - Power in watts  
**Formula**: `P = V × I`  
**Example**:
```c
float power = Calculate_Power(12.0f, 2.5f);
// Result: 30.0W
```

#### `Update_Energy()`
```c
void Update_Energy(float power, uint32_t delta_time_ms)
```
**Description**: Updates accumulated energy using trapezoidal integration  
**Parameters**:
- `power`: Current power in watts  
- `delta_time_ms`: Time interval in milliseconds  
**Returns**: `void`  
**Side Effects**: Updates global `accumulated_energy` variable  
**Formula**: `Energy += Power × (ΔTime/3600000)` (converts ms to hours)

#### `Update_Peaks()`
```c
void Update_Peaks(float voltage, float current, float power)
```
**Description**: Updates peak (maximum) values for voltage, current, and power  
**Parameters**:
- `voltage`: Current voltage reading  
- `current`: Current current reading  
- `power`: Current power reading  
**Returns**: `void`  
**Side Effects**: Updates global peak variables if new values exceed current peaks

### Reset Functions

#### `Reset_Energy()`
```c
void Reset_Energy(void)
```
**Description**: Resets accumulated energy to zero  
**Parameters**: None  
**Returns**: `void`  
**Usage**: Called during system initialization or user reset

#### `Reset_Peaks()`
```c
void Reset_Peaks(void)
```
**Description**: Resets all peak values to zero  
**Parameters**: None  
**Returns**: `void`  
**Usage**: Called during system initialization or user reset

## 🖥️ Display Functions

### Core Display Functions

#### `Display_Current_Menu()`
```c
void Display_Current_Menu(void)
```
**Description**: Updates display based on current menu state  
**Parameters**: None  
**Returns**: `void`  
**Functionality**: Routes to appropriate display function based on `current_menu` state

#### `Display_Power_Meter()`
```c
void Display_Power_Meter(void)
```
**Description**: Displays main power meter screen with real-time measurements  
**Parameters**: None  
**Returns**: `void`  
**Display Format**:
```
V:12.3V  I:1.25A
P:15.4W E:123mWh  
ROT:001 BTN:OFF
```

#### `Display_Graphics()`
```c
void Display_Graphics(void)
```
**Description**: Displays real-time graphs for voltage, current, or power  
**Parameters**: None  
**Returns**: `void`  
**Features**:
- Switches between V/I/P based on `graphics_parameter`
- Shows historical data with 32-point resolution
- Auto-scaling based on measurement range

### Graphics Data Management

#### `Update_Graphics_Data()`
```c
void Update_Graphics_Data(void)
```
**Description**: Updates circular buffers with latest measurement data  
**Parameters**: None  
**Returns**: `void`  
**Data Updated**:
- `voltage_history[]` array
- `current_history[]` array  
- `power_history[]` array
- `history_index` pointer

## 🎮 User Interface Functions

### Menu Navigation

#### `Handle_Menu_Action()`
```c
void Handle_Menu_Action(uint8_t action_type)
```
**Description**: Processes user input and updates menu state  
**Parameters**:
- `action_type`: Type of user action (0=short press, 1=long press)  
**Returns**: `void`  
**Functionality**:
- Menu navigation logic
- State transitions
- Action execution

### Interrupt Handlers

#### `User_Button_Interrupt_Handler()`
```c
void User_Button_Interrupt_Handler(void)
```
**Description**: Handles user button press/release events  
**Parameters**: None  
**Returns**: `void`  
**Features**:
- Software debouncing (20ms filter)
- Short press detection (<2s)
- Long press detection (≥4s for reset)
- State tracking for press/release cycles

#### `Rotary_Encoder_Interrupt_Handler()`
```c
void Rotary_Encoder_Interrupt_Handler(void)
```
**Description**: Handles rotary encoder rotation events  
**Parameters**: None  
**Returns**: `void`  
**Features**:
- Quadrature decoding
- Direction detection (CW/CCW)
- Position counter update
- Debouncing and noise filtering

## 🔧 Utility Functions

### ADC Interface

#### `Get_ADC_Value()`
```c
uint32_t Get_ADC_Value(uint32_t adc_channel)
```
**Description**: Reads ADC value from specified channel  
**Parameters**:
- `adc_channel`: ADC channel number (ADC_CHANNEL_3 or ADC_CHANNEL_4)  
**Returns**: `uint32_t` - Raw ADC value (0-4095)  
**Usage**:
```c
uint32_t voltage_raw = Get_ADC_Value(ADC_CHANNEL_4);  // PA4
uint32_t current_raw = Get_ADC_Value(ADC_CHANNEL_3);  // PA3
```

### String Formatting

#### `sprintf()` Usage Examples
```c
// Voltage display with 1 decimal place
sprintf(line1, "V:%d.%dV", v_int, v_frac);

// Current display with 2 decimal places  
sprintf(line2, "I:%d.%02dA", i_int, i_frac);

// Power display with 1 decimal place
sprintf(line3, "P:%d.%dW", p_int, p_frac);

// Energy display (conditional format)
if (accumulated_energy < 1.0f) {
    int e_wh = (int)(accumulated_energy * 1000.0f);
    sprintf(energy_str, "E:%dmWh", e_wh);
} else {
    int e_int = (int)accumulated_energy;
    int e_frac = (int)((accumulated_energy - e_int) * 1000.0f);
    sprintf(energy_str, "E:%d.%03dkWh", e_int, e_frac);
}
```

## 🖼️ SSD1306 Display API

### Core Display Functions

#### `ssd1306_Init()`
```c
void ssd1306_Init(void)
```
**Description**: Initializes SSD1306 OLED display  
**Parameters**: None  
**Returns**: `void`  
**Prerequisites**: I2C peripheral must be initialized first

#### `ssd1306_Fill()`
```c
void ssd1306_Fill(SSD1306_COLOR color)
```
**Description**: Fills entire display buffer with specified color  
**Parameters**:
- `color`: `Black` or `White`  
**Returns**: `void`  
**Note**: Requires `ssd1306_UpdateScreen()` to take effect

#### `ssd1306_UpdateScreen()`
```c
void ssd1306_UpdateScreen(void)
```
**Description**: Transfers display buffer to OLED via I2C  
**Parameters**: None  
**Returns**: `void`  
**Performance**: ~5-10ms transfer time for full screen

### Text Display Functions

#### `ssd1306_SetCursor()`
```c
void ssd1306_SetCursor(uint8_t x, uint8_t y)
```
**Description**: Sets text cursor position  
**Parameters**:
- `x`: Horizontal position (0-127 pixels)  
- `y`: Vertical position (0-63 pixels)  
**Returns**: `void`

#### `ssd1306_WriteString()`
```c
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
```
**Description**: Writes text string at current cursor position  
**Parameters**:
- `str`: Null-terminated string to display  
- `Font`: Font size (`Font_6x8`, `Font_7x10`, etc.)  
- `color`: Text color (`White` or `Black`)  
**Returns**: `char` - Last character written  
**Example**:
```c
ssd1306_SetCursor(0, 0);
ssd1306_WriteString("Voltage: 12.3V", Font_7x10, White);
```

### Graphics Functions

#### `ssd1306_DrawPixel()`
```c
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
```
**Description**: Sets or clears individual pixel  
**Parameters**:
- `x`: Horizontal position (0-127)  
- `y`: Vertical position (0-63)  
- `color`: Pixel color (`White` or `Black`)  
**Returns**: `void`

#### `ssd1306_Line()`
```c
void ssd1306_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
```
**Description**: Draws line between two points  
**Parameters**:
- `x1, y1`: Starting point coordinates  
- `x2, y2`: Ending point coordinates  
- `color`: Line color  
**Returns**: `void`

## ⚡ Hardware Abstraction

### HAL Integration Functions

#### GPIO Functions
```c
// Read GPIO pin state
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// Write GPIO pin state  
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

// Toggle GPIO pin
void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// Examples:
uint8_t button_state = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
```

#### ADC Functions
```c
// Start ADC conversion
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc);

// Wait for conversion complete
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);

// Get conversion result
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef* hadc);

// Configure ADC channel
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig);
```

#### I2C Functions  
```c
// Transmit data to device
HAL_StatusTypeDef HAL_I2C_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, 
                                   uint8_t *pData, uint16_t Size, uint32_t Timeout);

// Check if device is ready
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, 
                                        uint32_t Trials, uint32_t Timeout);
```

## 🔧 Configuration Functions

### Peripheral Initialization

#### `MX_GPIO_Init()`
```c
static void MX_GPIO_Init(void)
```
**Description**: Initializes all GPIO pins and interrupts  
**Parameters**: None  
**Returns**: `void`  
**Configuration**:
- Input pins: Pull-up enabled, interrupt on both edges
- Output pins: Push-pull, low speed
- Analog pins: Floating input

#### `MX_ADC_Init()`  
```c
static void MX_ADC_Init(void)
```
**Description**: Initializes ADC peripheral for voltage/current measurement  
**Parameters**: None  
**Returns**: `void`  
**Configuration**:
- 12-bit resolution
- Single conversion mode
- Software trigger
- Channels 3 and 4 configured

#### `MX_I2C1_Init()`
```c
static void MX_I2C1_Init(void)
```
**Description**: Initializes I2C1 peripheral for display communication  
**Parameters**: None  
**Returns**: `void`  
**Configuration**:
- Standard mode (100kHz)
- 7-bit addressing
- Internal pull-ups enabled

#### `MX_TIM6_Init()`
```c
static void MX_TIM6_Init(void)
```
**Description**: Initializes Timer 6 for 10Hz measurement interrupts  
**Parameters**: None  
**Returns**: `void`  
**Configuration**:
- Prescaler: 31999 (32MHz → 1kHz)
- Period: 99 (1kHz → 10Hz)
- Update interrupt enabled

## ⚠️ Error Handling

### Error Detection

#### Return Value Checking
```c
// HAL function error checking pattern
HAL_StatusTypeDef status = HAL_ADC_Start(&hadc);
if (status != HAL_OK) {
    // Handle ADC start error
    Error_Handler();
}

// I2C communication error checking
if (HAL_I2C_Transmit(&hi2c1, address, data, size, timeout) != HAL_OK) {
    // Handle I2C error - could retry or use alternate method
    i2c_error_count++;
    if (i2c_error_count > MAX_RETRIES) {
        Error_Handler();
    }
}
```

#### Range Validation
```c
// Input validation example
float Validate_Voltage(float voltage) {
    if (voltage < 0.0f) {
        return 0.0f;  // Clamp to minimum
    } else if (voltage > 35.0f) {
        return 35.0f;  // Clamp to maximum  
    }
    return voltage;   // Valid range
}

// Array bounds checking
void Update_History_Safe(float new_value) {
    if (history_index >= GRAPH_DATA_POINTS) {
        history_index = 0;  // Wrap around
    }
    voltage_history[history_index] = new_value;
    history_index++;
}
```

### Debug Support

#### UART Debug Output
```c
// Printf redirection to UART (if implemented)
#ifdef DEBUG_UART
    printf("ADC Voltage: %d, Current: %d\r\n", voltage_adc, current_adc);
    printf("Calculated Power: %.2f W\r\n", calculated_power);
#endif

// LED status indication
void Indicate_Status(SystemStatus status) {
    switch (status) {
        case STATUS_NORMAL:
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
            break;
        case STATUS_WARNING:
            // Slow blink
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            HAL_Delay(500);
            break;
        case STATUS_ERROR:
            // Fast blink
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            HAL_Delay(100);
            break;
    }
}
```

## 📋 Function Usage Examples

### Complete Measurement Cycle
```c
void Measurement_Cycle_Example(void) {
    // Read raw ADC values
    uint32_t voltage_adc = Get_ADC_Value(ADC_CHANNEL_4);
    uint32_t current_adc = Get_ADC_Value(ADC_CHANNEL_3);
    
    // Convert to engineering units
    float voltage = Convert_ADC_to_Voltage(voltage_adc);
    float current = Convert_ADC_to_Current(current_adc);
    
    // Calculate power
    float power = Calculate_Power(voltage, current);
    
    // Update energy and peaks
    Update_Energy(power, 100);  // 100ms interval
    Update_Peaks(voltage, current, power);
    
    // Update display
    Display_Current_Menu();
}
```

### Menu Navigation Example
```c
void Menu_Navigation_Example(void) {
    // Simulate encoder rotation (clockwise)
    if (encoder_direction == CLOCKWISE) {
        switch (current_menu) {
            case MENU_MAIN:
                menu_selection = (menu_selection + 1) % MAX_MAIN_ITEMS;
                break;
            case MENU_GRAPHICS:
                graphics_parameter = (graphics_parameter + 1) % 3;  // V/I/P
                break;
        }
        menu_changed = 1;  // Trigger display update
    }
    
    // Simulate button press
    if (button_pressed) {
        Handle_Menu_Action(0);  // Short press
        button_pressed = 0;
    }
}
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*API Version: Production v1.0*