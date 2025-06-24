# Common Issues and Solutions

## 📋 Table of Contents
- [Hardware Issues](#hardware-issues)
- [Software Issues](#software-issues)
- [Display Problems](#display-problems)
- [Measurement Issues](#measurement-issues)
- [User Interface Problems](#user-interface-problems)
- [Build and Programming Issues](#build-and-programming-issues)
- [Performance Issues](#performance-issues)
- [Calibration Problems](#calibration-problems)

## 🔧 Hardware Issues

### Power Supply Problems

#### No Power / Device Won't Start
```
Symptoms:
├── No display illumination
├── No LED activity
├── No response to inputs
└── ST-Link cannot connect

Diagnostic Steps:
1. Check power supply voltage with multimeter
   Expected: 3.3V ±10% (2.97V - 3.63V)
2. Verify power supply current capability
   Required: 50-100mA minimum
3. Check power connections for continuity
4. Test power supply under load

Common Causes & Solutions:
├── Insufficient current capability
   → Use supply rated for >100mA
├── Wrong voltage (5V instead of 3.3V)
   → Use 3.3V regulated supply or add regulator
├── Loose connections
   → Check solder joints and wire connections
├── Reverse polarity
   → Verify positive/negative connections
└── Power supply failure
   → Test with known good supply
```

#### Intermittent Power Issues
```
Symptoms:
├── Random resets or shutdowns
├── Display flickering
├── Measurement instability
└── Communication errors

Diagnostic Steps:
1. Monitor supply voltage during operation
2. Check for voltage drops during load changes
3. Measure supply ripple with oscilloscope
4. Test power connections under vibration

Solutions:
├── Add bulk capacitance (100µF-1000µF)
├── Use low-dropout (LDO) regulator
├── Improve power distribution (wider traces)
├── Add ferrite beads for noise suppression
└── Check ground connections
```

### Connection Problems

#### SWD Programming Issues
```
Symptoms:
├── ST-Link cannot detect target
├── Programming fails intermittently
├── Debug connection unstable
└── "No target connected" errors

Diagnostic Steps:
1. Verify SWD pin connections:
   ├── SWDIO (PA13) - Data line
   ├── SWCLK (PA14) - Clock line  
   ├── VDD - Power reference
   ├── GND - Ground reference
   └── NRST - Reset (optional)

2. Check signal integrity:
   ├── Measure voltage levels (should be 3.3V logic)
   ├── Check for shorts between signals
   ├── Verify signal continuity
   └── Test with shorter connections

Common Solutions:
├── Reduce SWD clock speed (4MHz → 1MHz)
├── Add pull-up resistors (10kΩ on SWDIO/SWCLK)
├── Connect NRST for more reliable programming
├── Use dedicated SWD connector instead of breadboard
├── Check for conflicting pin usage in software
└── Update ST-Link firmware
```

#### I2C Display Connection Issues
```
Symptoms:
├── Display remains blank
├── Garbled display content
├── I2C communication errors
└── Display works intermittently

Diagnostic Steps:
1. Verify I2C connections:
   ├── SDA (PB7) - Data line
   ├── SCL (PB6) - Clock line
   ├── VCC - 3.3V power
   └── GND - Ground

2. Check I2C signals with oscilloscope:
   ├── Clock frequency (should be 100kHz)
   ├── Signal levels (0V/3.3V logic)
   ├── ACK/NACK responses from display
   └── Start/stop conditions

Solutions:
├── Add pull-up resistors (2.2kΩ - 4.7kΩ)
├── Reduce I2C clock speed
├── Check display I2C address (0x3C or 0x3D)
├── Verify power supply to display
├── Use twisted pair for SDA/SCL signals
└── Check for bus contention or noise
```

### Analog Input Issues

#### ADC Reading Problems
```
Symptoms:
├── Voltage/current readings stuck at zero
├── Readings fluctuate wildly
├── Maximum reading regardless of input
└── Non-linear response

Diagnostic Steps:
1. Test ADC inputs directly:
   ├── Apply known voltage to PA3/PA4
   ├── Read raw ADC values (0-4095)
   ├── Verify against expected values
   └── Test with multimeter reference

2. Check analog signal path:
   ├── Measure voltage at MCU pins
   ├── Check signal conditioning circuits
   ├── Verify ground connections
   └── Test without signal conditioning

Common Causes & Solutions:
├── Pin configuration incorrect
   → Verify GPIO mode set to analog
├── ADC not initialized properly
   → Check ADC configuration in code
├── Reference voltage problem
   → Verify VDDA = 3.3V
├── Input protection engaged
   → Check for overvoltage conditions
├── Ground loops or noise
   → Improve grounding and filtering
└── Incorrect scaling factors
   → Verify calibration constants
```

## 💻 Software Issues

### Programming and Flash Issues

#### Flash Memory Overflow
```
Error Message: "region `FLASH' overflowed by XXX bytes"

Immediate Solutions:
1. Enable size optimization:
   Project → Properties → C/C++ Build → Settings
   Tool Settings → MCU GCC Compiler → Optimization
   Select: "Optimize for size (-Os)"

2. Enable dead code elimination:
   MCU GCC Linker → Miscellaneous
   Add: "-Wl,--gc-sections"

3. Reduce program size:
   ├── Minimize string literals
   ├── Remove unused functions
   ├── Reduce buffer sizes
   ├── Use smaller data types where possible
   └── Remove debug code from release builds

Memory Analysis:
├── Check .map file for largest consumers
├── Use: arm-none-eabi-nm --size-sort *.o
├── Analyze: arm-none-eabi-objdump -h *.elf
└── Monitor section sizes in build output
```

#### Stack Overflow
```
Symptoms:
├── Random hard faults
├── Corrupted variables
├── System resets during operation
└── Unpredictable behavior

Diagnostic Steps:
1. Monitor stack usage:
   ├── Enable -fstack-usage compiler flag
   ├── Check .su files for function stack usage
   ├── Set stack canary values
   └── Monitor stack pointer during debugging

2. Identify stack consumers:
   ├── Deep function call chains
   ├── Large local variables
   ├── Recursive functions
   └── Interrupt handler stack usage

Solutions:
├── Increase stack size in linker script
├── Move large variables to global scope
├── Reduce local variable sizes
├── Optimize function call depth
├── Use dynamic allocation sparingly
└── Monitor stack usage in production
```

### Runtime Errors

#### Hard Fault Debugging
```
Symptoms:
├── System enters infinite loop
├── LED blinks rapidly (error indication)
├── Program counter jumps to Error_Handler
└── Debugger shows hard fault exception

Debug Procedure:
1. Enable fault handlers:
   ├── Implement HardFault_Handler with debugging
   ├── Examine fault status registers
   ├── Check faulting instruction address
   └── Analyze register contents at fault

2. Common causes:
   ├── Null pointer dereference
   ├── Array bounds violation
   ├── Stack overflow
   ├── Invalid function pointer
   ├── Unaligned memory access
   └── Division by zero

Example Debug Handler:
```c
void HardFault_Handler(void) {
    // Save fault information
    volatile uint32_t *hardfault_args;
    asm("TST lr, #4");
    asm("ITE EQ");
    asm("MRSEQ %0, MSP" : "=r" (hardfault_args));
    asm("MRSNE %0, PSP" : "=r" (hardfault_args));
    
    // Extract fault information
    uint32_t stacked_pc = hardfault_args[6];
    uint32_t stacked_lr = hardfault_args[5];
    
    // Set breakpoint here and examine variables
    __BKPT(0);
    
    // Call error handler
    Error_Handler();
}
```

#### Memory Corruption
```
Symptoms:
├── Variables change unexpectedly
├── Array indices out of bounds
├── Display shows corrupted data
└── Intermittent system crashes

Detection Methods:
1. Array bounds checking:
   ```c
   #define SAFE_ARRAY_ACCESS(arr, idx, max) \
       ((idx) < (max) ? (arr)[(idx)] : 0)
   ```

2. Buffer overflow protection:
   ```c
   void safe_strcpy(char *dest, const char *src, size_t dest_size) {
       strncpy(dest, src, dest_size - 1);
       dest[dest_size - 1] = '\0';
   }
   ```

3. Memory pattern checking:
   ```c
   #define STACK_CANARY 0xDEADBEEF
   uint32_t stack_canary = STACK_CANARY;
   
   void check_stack_integrity(void) {
       if (stack_canary != STACK_CANARY) {
           // Stack corruption detected
           Error_Handler();
       }
   }
   ```
```

## 📺 Display Problems

### SSD1306 OLED Issues

#### Blank Display
```
Symptoms:
├── Display backlight on but no content
├── No startup message appears
├── Display never initializes
└── I2C communication fails

Diagnostic Steps:
1. Verify power and connections:
   ├── VCC = 3.3V ±10%
   ├── GND connected
   ├── SDA/SCL connections secure
   └── I2C address correct (0x3C or 0x3D)

2. Test I2C communication:
   ```c
   // Test basic I2C communication
   HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, 0x3C<<1, 3, 100);
   if (status != HAL_OK) {
       // Display not responding
   }
   ```

3. Check initialization sequence:
   ├── Proper reset timing
   ├── Initialization commands sent
   ├── Display configuration correct
   └── Screen buffer allocated

Solutions:
├── Add I2C pull-up resistors (2.2kΩ)
├── Reduce I2C clock speed
├── Check display module compatibility
├── Verify initialization timing
├── Test with known working display
└── Check for hardware damage
```

#### Corrupted Display Content
```
Symptoms:
├── Partial text/graphics displayed
├── Random pixels scattered on screen  
├── Display content shifts or wraps
└── Intermittent display updates

Common Causes & Solutions:
├── I2C communication errors
   → Add error checking and retry logic
├── Buffer overflow/underflow
   → Verify buffer bounds checking
├── Incorrect display buffer size
   → Check SSD1306_BUFFER_SIZE definition
├── Race conditions in multi-threaded access
   → Add synchronization mechanisms
├── Power supply noise
   → Add decoupling capacitors
└── EMI interference
   → Shield display cables, add ferrites
```

#### Slow Display Updates
```
Symptoms:
├── Noticeable delay in menu transitions
├── Measurement values update slowly
├── Graphics appear to lag
└── User interface feels unresponsive

Performance Analysis:
1. Measure display update time:
   ```c
   uint32_t start_time = HAL_GetTick();
   ssd1306_UpdateScreen();
   uint32_t update_time = HAL_GetTick() - start_time;
   // Should be < 20ms for full screen update
   ```

2. Optimize display operations:
   ├── Update only changed regions
   ├── Use double buffering sparingly
   ├── Reduce unnecessary screen clears
   ├── Batch multiple text operations
   └── Optimize I2C speed if reliable

Solutions:
├── Increase I2C clock to 400kHz (if stable)
├── Minimize full screen redraws
├── Use partial screen updates
├── Cache formatted strings
├── Reduce update frequency for static content
└── Profile and optimize slow functions
```

## 📊 Measurement Issues

### Incorrect Readings

#### Voltage Measurement Problems
```
Symptoms:
├── Voltage always reads zero
├── Voltage reading too high/low
├── Non-linear voltage response
└── Voltage reading unstable

Diagnostic Steps:
1. Test voltage divider circuit:
   ├── Apply known voltage (e.g., 3.3V)
   ├── Measure voltage at PA4 with multimeter
   ├── Calculate expected ADC reading
   └── Compare with actual ADC value

2. Verify calibration:
   ```c
   // Expected voltage calculation
   float expected_voltage = (adc_reading / 4095.0f) * 3.3f * VOLTAGE_SCALE_FACTOR;
   // Compare with multimeter reading
   ```

Common Issues & Solutions:
├── Incorrect scale factor
   → Recalibrate VOLTAGE_SCALE_FACTOR
├── Voltage divider loading
   → Use higher resistance values
├── ADC reference voltage error
   → Verify VDDA = 3.3V precisely
├── Input protection activated
   → Check for overvoltage conditions
├── Poor connections
   → Verify continuity of signal path
└── Ground potential differences
   → Use single-point grounding
```

#### Current Measurement Problems
```
Symptoms:
├── Current always reads zero
├── Current reading proportional to voltage
├── Current sensor not responding
└── Negative current readings

Diagnostic Steps:
1. Test current sensor independently:
   ├── Apply known current through sensor
   ├── Measure sensor output voltage
   ├── Verify sensor specifications
   └── Check sensor power supply

2. Verify current path:
   ├── Ensure current flows through sensor
   ├── Check for parallel current paths
   ├── Verify series connection
   └── Test sensor with multimeter

Current Sensor Types & Issues:
├── Hall-effect sensors:
   • Require proper supply voltage
   • Sensitive to magnetic fields
   • May have offset voltage
   • Temperature dependent
   
├── Shunt resistors:
   • Very low resistance (mΩ range)
   • Requires differential amplifier
   • Power dissipation concerns
   • Precision resistor needed
   
├── Current transformers:
   • AC only (not suitable for DC)
   • Burden resistor required
   • Turns ratio important
   • Frequency limitations

Solutions:
├── Verify sensor type compatibility
├── Check sensor orientation/polarity
├── Calibrate offset and scale factors
├── Add signal conditioning if needed
├── Use appropriate measurement range
└── Consider sensor replacement if damaged
```

### Calibration Drift

#### Long-term Accuracy Issues
```
Symptoms:
├── Readings drift over time
├── Calibration becomes inaccurate
├── Temperature-dependent errors
└── Systematic measurement bias

Causes & Solutions:
├── Component aging
   → Periodic recalibration schedule
├── Temperature effects
   → Temperature compensation algorithms
├── Supply voltage variations
   → Use precision voltage reference
├── Mechanical stress on components
   → Proper mounting and strain relief
├── Environmental factors (humidity, etc.)
   → Environmental protection measures
└── Wear of potentiometer (if used)
   → Replace with fixed resistors

Calibration Maintenance:
1. Establish calibration schedule (monthly/quarterly)
2. Use NIST-traceable reference standards
3. Document calibration results and adjustments
4. Monitor drift trends over time
5. Set acceptance criteria for drift limits
```

## 🎮 User Interface Problems

### Button Issues

#### Button Not Responding
```
Symptoms:
├── Button press has no effect
├── Menu doesn't appear when pressed
├── No navigation possible
└── Button state always shows OFF

Diagnostic Steps:
1. Test button hardware:
   ├── Measure continuity across button
   ├── Check for mechanical damage
   ├── Test button with multimeter
   └── Verify button type (NO vs NC)

2. Check software configuration:
   ├── GPIO mode set to input with pull-up
   ├── EXTI interrupt enabled
   ├── Interrupt handler implemented
   └── Debouncing logic working

3. Monitor button signal:
   ```c
   // Debug button state
   uint8_t button_raw = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
   printf("Button state: %d\r\n", button_raw);
   ```

Solutions:
├── Check button connections and solder joints
├── Verify pull-up resistor (internal or external)
├── Test different button if available
├── Check interrupt priority conflicts
├── Verify debouncing parameters
└── Monitor for electromagnetic interference
```

#### Button Bouncing Issues
```
Symptoms:
├── Single press registers multiple times
├── Menu rapidly changes states
├── Unreliable button response
└── False triggering

Software Debouncing Solutions:
```c
// Improved debouncing algorithm
#define DEBOUNCE_TIME_MS 20
#define STABLE_TIME_MS 5

static uint32_t last_change_time = 0;
static uint8_t button_stable_state = 0;
static uint8_t button_raw_state = 0;

void Button_Debounce_Handler(void) {
    uint8_t current_raw = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
    uint32_t current_time = HAL_GetTick();
    
    if (current_raw != button_raw_state) {
        // State change detected
        button_raw_state = current_raw;
        last_change_time = current_time;
    } else if ((current_time - last_change_time) > DEBOUNCE_TIME_MS) {
        // State has been stable
        if (current_raw != button_stable_state) {
            button_stable_state = current_raw;
            // Process stable state change
            Process_Button_Event(button_stable_state);
        }
    }
}
```

Hardware Solutions:
├── Add RC filter (100Ω + 100nF)
├── Use Schmitt trigger buffer
├── Shield button wires from noise
├── Use higher quality switches
└── Add mechanical damping
```

### Rotary Encoder Issues

#### Encoder Not Working
```
Symptoms:
├── Menu selection doesn't change
├── No response to rotation
├── Encoder position stuck
└── Random direction changes

Diagnostic Steps:
1. Test encoder signals:
   ├── Monitor CHA and CHB pins with oscilloscope
   ├── Verify quadrature relationship (90° phase)
   ├── Check signal voltage levels
   └── Test mechanical rotation smoothness

2. Verify software configuration:
   ├── Both channels configured as inputs
   ├── Pull-up resistors enabled
   ├── EXTI interrupts enabled for both pins
   └── Quadrature decoding logic correct

3. Encoder signal analysis:
   ```c
   // Debug encoder signals
   uint8_t cha = HAL_GPIO_ReadPin(ROT_CHA_GPIO_Port, ROT_CHA_Pin);
   uint8_t chb = HAL_GPIO_ReadPin(ROT_CHB_GPIO_Port, ROT_CHB_Pin);
   printf("Encoder: CHA=%d, CHB=%d\r\n", cha, chb);
   ```

Common Issues & Solutions:
├── Wrong encoder type (absolute vs incremental)
   → Use incremental rotary encoder
├── Poor connections or broken traces
   → Check continuity and connections
├── Insufficient pull-up resistance
   → Use 2.2kΩ - 10kΩ pull-ups
├── Mechanical damage or wear
   → Replace encoder if damaged
├── Incorrect quadrature decoding
   → Verify state machine logic
└── EMI causing false transitions
   → Add filtering and shielding
```

#### Erratic Encoder Behavior
```
Symptoms:
├── Occasional missed counts
├── Direction detection errors
├── Multiple counts per detent
└── Count drift over time

Improved Encoder Handling:
```c
// Robust quadrature decoder
typedef struct {
    uint8_t last_state;
    int32_t position;
    uint32_t last_update_time;
} Encoder_t;

static Encoder_t encoder = {0};

void Encoder_Update(void) {
    uint8_t cha = HAL_GPIO_ReadPin(ROT_CHA_GPIO_Port, ROT_CHA_Pin);
    uint8_t chb = HAL_GPIO_ReadPin(ROT_CHB_GPIO_Port, ROT_CHB_Pin);
    uint8_t current_state = (cha << 1) | chb;
    uint32_t current_time = HAL_GetTick();
    
    // Ignore changes too close together (debouncing)
    if ((current_time - encoder.last_update_time) < 2) {
        return;
    }
    
    // State transition table for quadrature decoding
    static const int8_t transitions[16] = {
        0, -1, 1, 0,   // 00 -> 00,01,10,11
        1, 0, 0, -1,   // 01 -> 00,01,10,11  
        -1, 0, 0, 1,   // 10 -> 00,01,10,11
        0, 1, -1, 0    // 11 -> 00,01,10,11
    };
    
    uint8_t index = (encoder.last_state << 2) | current_state;
    int8_t direction = transitions[index];
    
    if (direction != 0) {
        encoder.position += direction;
        encoder.last_update_time = current_time;
    }
    
    encoder.last_state = current_state;
}
```

## 🔨 Build and Programming Issues

### Compilation Problems

#### Missing Include Files
```
Error: "fatal error: file.h: No such file or directory"

Solutions:
1. Check include paths:
   Project → Properties → C/C++ Build → Settings
   Tool Settings → MCU GCC Compiler → Include paths
   Verify all necessary paths are listed

2. Common missing include paths:
   ├── Core/Inc
   ├── Core/Inc/ssd1306  
   ├── Drivers/STM32L0xx_HAL_Driver/Inc
   ├── Drivers/CMSIS/Device/ST/STM32L0xx/Include
   └── Drivers/CMSIS/Include

3. Case sensitivity issues (Linux/macOS):
   ├── Verify exact filename capitalization
   ├── Check path separator consistency
   └── Use relative paths when possible
```

#### Linker Errors
```
Error: "undefined reference to 'function_name'"

Common Causes & Solutions:
├── Function declared but not implemented
   → Implement missing function
├── Source file not included in build
   → Add to project or makefile
├── Library not linked
   → Add required libraries to linker
├── Name mangling (C++ vs C)
   → Use extern "C" for C functions in C++
├── Weak symbol not resolved
   → Provide strong implementation
└── Case sensitivity mismatch
   → Check function name capitalization

Error: "multiple definition of 'variable_name'"

Solutions:
├── Use static for file-local variables
├── Use extern declarations in headers
├── Remove duplicate definitions
├── Check header include guards
└── Use linker script to resolve conflicts
```

### Programming and Debug Issues

#### ST-Link Connection Problems
```
Problem: "No ST-LINK detected"

Hardware Solutions:
├── Check USB cable (try different cable)
├── Try different USB port
├── Check ST-Link power LED
├── Verify ST-Link drivers installed
└── Test ST-Link with STM32CubeProgrammer

Software Solutions:
├── Update ST-Link firmware
├── Reset ST-Link (disconnect/reconnect)
├── Change debug probe settings
├── Try different debug interface (SWD vs JTAG)
└── Check for conflicting debug software

Problem: "Cannot connect to target"

Target Hardware Check:
├── Verify target power (3.3V)
├── Check SWD connections (SWDIO, SWCLK)
├── Test continuity of debug signals
├── Ensure target not in low-power mode
└── Try connecting under reset (NRST)

Software Configuration:
├── Verify target MCU selection
├── Check debug interface settings
├── Adjust SWD clock speed (lower)
├── Enable "Connect under reset" option
└── Check for conflicting pin usage
```

## ⚡ Performance Issues

### Slow Response Times

#### Menu Navigation Delays
```
Symptoms:
├── Delay between button press and response
├── Encoder rotation feels sluggish
├── Menu transitions are slow
└── Display updates lag behind input

Performance Analysis:
1. Measure response times:
   ```c
   uint32_t start_time = HAL_GetTick();
   Handle_Menu_Action(0);  // Process button press
   uint32_t response_time = HAL_GetTick() - start_time;
   // Should be < 10ms for good responsiveness
   ```

2. Profile interrupt handlers:
   ├── Measure ISR execution time
   ├── Check for blocking operations in ISRs
   ├── Verify interrupt priorities
   └── Look for interrupt nesting issues

Optimization Strategies:
├── Move time-consuming operations out of ISRs
├── Use deferred processing for complex operations
├── Optimize display update frequency
├── Cache frequently used calculations
├── Minimize string formatting in real-time code
└── Use efficient algorithms for menu handling
```

#### Measurement Update Rate Issues
```
Symptoms:
├── Power calculations appear to lag
├── Display values update slowly
├── Energy accumulation seems incorrect
└── Peak detection misses transients

Root Cause Analysis:
1. Timer interrupt frequency:
   ├── Verify TIM6 configured for 10Hz (100ms)
   ├── Check if interrupt handler takes too long
   ├── Monitor interrupt jitter and latency
   └── Ensure interrupt is actually firing

2. ADC conversion timing:
   ├── Measure ADC conversion completion time
   ├── Check for ADC configuration errors
   ├── Verify ADC sampling time settings
   └── Look for ADC overrun conditions

Solutions:
├── Optimize Timer_Interrupt_Handler() execution time
├── Use ADC DMA for faster data transfer
├── Parallel processing of voltage/current channels
├── Reduce floating-point calculations if possible
├── Use fixed-point arithmetic for performance
└── Profile and optimize critical code paths
```

### Memory Usage Issues

#### RAM Shortage
```
Symptoms:
├── Stack overflow errors
├── Heap allocation failures  
├── Variables getting corrupted
└── Unpredictable system behavior

Memory Analysis:
1. Check memory usage:
   ├── Analyze .map file for memory usage
   ├── Use debugger to monitor stack pointer
   ├── Check heap usage (if dynamic allocation used)
   └── Profile maximum stack usage

2. Identify memory consumers:
   ```bash
   # Analyze object file sizes
   arm-none-eabi-nm --size-sort *.o | tail -20
   
   # Check section sizes
   arm-none-eabi-objdump -h project.elf
   ```

Memory Optimization:
├── Reduce buffer sizes where possible
├── Use stack allocation instead of global
├── Pack structures to minimize padding
├── Use smaller data types (uint8_t vs uint32_t)
├── Eliminate unused global variables
└── Consider overlay techniques for large data
```

#### Flash Memory Optimization
```
Current Status: ~31KB used of 32KB available

Additional Optimization Techniques:
1. Compiler optimizations:
   ├── Use -Os (size optimization)
   ├── Enable -flto (link-time optimization)
   ├── Add -ffunction-sections -fdata-sections
   └── Use --gc-sections linker flag

2. Code size reduction:
   ├── Remove debug strings from release builds
   ├── Use shorter variable names (if significant)
   ├── Combine similar functions
   ├── Use lookup tables instead of calculations
   ├── Minimize string literals
   └── Remove unused library functions

3. Data optimization:
   ├── Store constants in flash (const qualifier)
   ├── Use bit fields for boolean flags
   ├── Pack related variables into structures
   ├── Use PROGMEM for large constant arrays
   └── Compress or encode data when possible
```

## 🎯 Calibration Problems

### Accuracy Issues

#### Calibration Drift Over Time
```
Symptoms:
├── Measurements become inaccurate gradually
├── Readings don't match reference meter
├── Systematic errors develop
└── Temperature-dependent variations

Calibration Verification Procedure:
1. Use NIST-traceable reference standards
2. Test at multiple points across range
3. Document environmental conditions
4. Compare with multiple reference instruments
5. Perform statistical analysis of results

Drift Compensation:
```c
// Temperature compensation example
typedef struct {
    float temp_coeff_voltage;  // ppm/°C
    float temp_coeff_current;  // ppm/°C
    float reference_temp;      // Reference calibration temperature
} TempCompensation_t;

float Apply_Temperature_Compensation(float raw_value, float current_temp, 
                                   TempCompensation_t* comp) {
    float temp_delta = current_temp - comp->reference_temp;
    float correction_factor = 1.0f + (comp->temp_coeff_voltage * temp_delta / 1000000.0f);
    return raw_value * correction_factor;
}
```

#### Multi-point Calibration
```
Problem: Linear calibration insufficient for accuracy

Solution: Implement multi-point calibration with interpolation

```c
typedef struct {
    float input_value;   // Reference input
    float measured_value; // Actual measurement
} CalibrationPoint_t;

typedef struct {
    CalibrationPoint_t points[5]; // 5-point calibration
    uint8_t num_points;
} CalibrationTable_t;

float Apply_Calibration(float raw_measurement, CalibrationTable_t* cal_table) {
    // Find surrounding calibration points
    for (int i = 0; i < cal_table->num_points - 1; i++) {
        if (raw_measurement >= cal_table->points[i].measured_value &&
            raw_measurement <= cal_table->points[i+1].measured_value) {
            
            // Linear interpolation between points
            float x1 = cal_table->points[i].measured_value;
            float y1 = cal_table->points[i].input_value;
            float x2 = cal_table->points[i+1].measured_value;
            float y2 = cal_table->points[i+1].input_value;
            
            return y1 + (raw_measurement - x1) * (y2 - y1) / (x2 - x1);
        }
    }
    
    // Extrapolation if outside calibrated range
    return raw_measurement; // Or apply single-point correction
}
```

### Calibration Procedures

#### Systematic Calibration Process
```
Equipment Required:
├── NIST-traceable DC voltage source (0-30V, ±0.1%)
├── NIST-traceable DC current source (0-5A, ±0.1%)  
├── Reference multimeter (6.5 digit, calibrated)
├── Precision load resistors (1%, low tempco)
└── Environmental monitoring (temperature, humidity)

Voltage Calibration Procedure:
1. Allow 30-minute warm-up time
2. Set environmental conditions (20°C ±2°C, <60% RH)
3. Apply calibration voltages in sequence:
   ├── 0.000V (short circuit)
   ├── 3.000V (10% of range)
   ├── 7.500V (25% of range)  
   ├── 15.000V (50% of range)
   ├── 22.500V (75% of range)
   └── 30.000V (100% of range)
4. Record reference and measured values
5. Calculate correction factors
6. Verify linearity and accuracy specifications

Current Calibration Procedure:
1. Use precision current source or shunt method
2. Apply calibration currents:
   ├── 0.000A (open circuit)
   ├── 0.500A (10% of range)
   ├── 1.250A (25% of range)
   ├── 2.500A (50% of range)
   ├── 3.750A (75% of range)
   └── 5.000A (100% of range)
3. Allow settling time between measurements
4. Record temperature for compensation
5. Calculate and verify correction factors
```

#### Calibration Documentation
```
Required Documentation:
├── Calibration certificate for reference equipment
├── Environmental conditions during calibration
├── Raw measurement data and calculations
├── Applied correction factors
├── Uncertainty analysis
├── Calibration validity period
├── Operator identification and signature
└── Any deviations from standard procedure

Calibration Record Template:
Date: ___________
Operator: ___________
Temperature: _____°C
Humidity: _____%RH
Reference Equipment: ___________
Serial Numbers: ___________

Voltage Calibration:
Applied (V) | Measured (V) | Error (%) | Correction
0.000       | _______      | _____     | _______
3.000       | _______      | _____     | _______
...

Current Calibration:
Applied (A) | Measured (A) | Error (%) | Correction  
0.000       | _______      | _____     | _______
0.500       | _______      | _____     | _______
...

Notes: ___________
Next Calibration Due: ___________
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*Troubleshooting Guide: Comprehensive issue resolution*