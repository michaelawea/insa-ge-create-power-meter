# Build Environment Setup

## 📋 Table of Contents
- [Prerequisites](#prerequisites)
- [STM32CubeIDE Installation](#stm32cubeide-installation)
- [Project Setup](#project-setup)
- [Build Configuration](#build-configuration)
- [Debug Configuration](#debug-configuration)
- [Version Control Setup](#version-control-setup)
- [Alternative Build Methods](#alternative-build-methods)
- [Troubleshooting](#troubleshooting)

## 📋 Prerequisites

### System Requirements

#### Windows
```
Operating System:   Windows 10/11 (64-bit)
RAM:               4GB minimum, 8GB recommended
Storage:           2GB free space for IDE + toolchain
Processor:         Intel Core i3 or equivalent
Additional:        USB port for ST-Link programmer
```

#### macOS
```
Operating System:   macOS 10.15 (Catalina) or later
RAM:               4GB minimum, 8GB recommended  
Storage:           2GB free space for IDE + toolchain
Processor:         Intel x64 or Apple Silicon (M1/M2)
Additional:        USB port for ST-Link programmer
Xcode:             Command line tools recommended
```

#### Linux
```
Operating System:   Ubuntu 18.04+ / CentOS 7+ / similar
RAM:               4GB minimum, 8GB recommended
Storage:           2GB free space for IDE + toolchain  
Processor:         x86_64 architecture
Additional:        USB port for ST-Link programmer
Libraries:         libusb, gtk3-dev (for GUI)
```

### Hardware Requirements

#### Development Hardware
```
STM32 Development Board:
├── STM32L052K6T6 (target MCU)
├── Or compatible STM32L0 series board
└── Breadboard setup with individual components

Programming Interface:
├── ST-Link/V2 programmer (integrated or standalone)
├── SWD connector (4-pin: VDD, GND, SWDIO, SWCLK)
└── USB cable for programmer connection

Test Equipment:
├── Multimeter (for calibration verification)
├── Variable DC power supply (0-30V, 0-5A)
├── Oscilloscope (optional, for signal analysis)
└── Test loads (resistors, LEDs, motors)
```

#### SSD1306 OLED Display
```
Display Specifications:
├── Size: 128x64 pixels (0.96" typical)
├── Interface: I2C (SDA, SCL, VCC, GND)
├── Voltage: 3.3V operation
├── Address: 0x3C or 0x3D (configurable)
└── Pull-up resistors: 2.2kΩ on SDA/SCL lines

Connection Requirements:
├── VCC → 3.3V power supply
├── GND → Common ground
├── SDA → PB7 (I2C1_SDA)
└── SCL → PB6 (I2C1_SCL)
```

## 🛠️ STM32CubeIDE Installation

### Download and Installation

#### STM32CubeIDE Download
```
Official Source:
├── Website: https://www.st.com/en/development-tools/stm32cubeide.html
├── Version: Latest stable (1.13.0+ recommended)
├── License: Free for commercial and non-commercial use
└── Registration: ST account required for download

Package Selection:
├── All-in-one installer (recommended)
├── Includes Eclipse IDE, GCC compiler, GDB debugger
├── Includes STM32CubeMX integration
└── Includes ST-Link drivers
```

#### Installation Steps

**Windows Installation:**
```
1. Download STM32CubeIDE installer (.exe)
2. Run installer as Administrator
3. Accept license agreement
4. Choose installation directory (default: C:\ST\STM32CubeIDE_X.X.X)
5. Select components:
   ✅ STM32CubeIDE
   ✅ STM32CubeMX
   ✅ ST-Link drivers
   ✅ GNU ARM toolchain
6. Complete installation (may require restart)
7. Verify installation by launching STM32CubeIDE
```

**macOS Installation:**
```
1. Download STM32CubeIDE installer (.dmg)
2. Mount disk image and run installer
3. Drag STM32CubeIDE to Applications folder
4. Install ST-Link drivers separately if needed:
   - Download STSW-LINK007 driver package
   - Install driver package
   - May require system security permissions
5. Launch STM32CubeIDE from Applications
6. Grant necessary permissions in System Preferences > Security
```

**Linux Installation:**
```
1. Download STM32CubeIDE installer (.sh)
2. Make installer executable:
   chmod +x st-stm32cubeide_*.sh
3. Run installer:
   sudo ./st-stm32cubeide_*.sh
4. Follow installation prompts
5. Install udev rules for ST-Link:
   sudo cp 49-stlinkv*.rules /etc/udev/rules.d/
   sudo udevadm control --reload-rules
6. Add user to dialout group:
   sudo usermod -a -G dialout $USER
7. Log out and back in for group changes to take effect
```

### Post-Installation Verification

#### IDE Functionality Check
```
1. Launch STM32CubeIDE
2. Create new workspace (e.g., ~/STM32CubeIDE/workspace)
3. Verify toolchain installation:
   - Help → About STM32CubeIDE
   - Check GNU ARM version (should be 10.3.x or later)
4. Test ST-Link connection:
   - Connect ST-Link programmer
   - Window → Show View → STM32 ST-LINK Utility
   - Should detect connected programmer
```

#### Compiler Verification
```
Test Compilation:
1. File → New → STM32 Project
2. Select target: STM32L052K6T6
3. Choose project template: "Empty"
4. Build project (Ctrl+B)
5. Verify successful compilation
6. Check output files in Debug/ folder

Expected Output:
├── project_name.elf (executable)
├── project_name.bin (binary image)
├── project_name.map (memory map)
└── Build successful message in console
```

## 📂 Project Setup

### Importing Existing Project

#### From Git Repository
```
1. Clone repository:
   git clone <repository-url>
   cd insa-ge-create-power-meter

2. Import in STM32CubeIDE:
   File → Import → General → Existing Projects into Workspace
   Browse to cloned repository folder
   Select project: "insa-ge-create-power-meter"
   Import project

3. Verify project structure:
   ├── Core/
   │   ├── Inc/ (header files)
   │   └── Src/ (source files)
   ├── Drivers/ (HAL library)
   ├── Debug/ (build output)
   └── .project, .cproject (Eclipse project files)
```

#### Project Configuration Verification
```
1. Right-click project → Properties
2. C/C++ Build → Settings → Tool Settings:
   
   MCU Settings:
   ├── MCU: STM32L052K6T6
   ├── Core: Cortex-M0+  
   ├── Floating Point: Software
   └── Instruction Set: Thumb

   Compiler Settings:
   ├── Optimization: -Os (Size optimization)
   ├── Debug Level: Default (-g)
   ├── Language Standard: C99
   └── Include Paths: Auto-generated

   Linker Settings:
   ├── Script: STM32L052K6TX_FLASH.ld
   ├── Libraries: System libraries
   └── Memory Layout: Flash 32KB, RAM 8KB
```

### Creating New Project (Alternative)

#### STM32CubeMX Configuration
```
1. Start new STM32 Project in CubeIDE
2. Select MCU: STM32L052K6T6
3. Configure peripherals in CubeMX:

   GPIO Configuration:
   ├── PA3: ADC1_IN3 (Current Input)
   ├── PA4: ADC1_IN4 (Voltage Input)  
   ├── PA9: USART1_TX (Debug)
   ├── PA10: USART1_RX (Debug)
   ├── PA15: GPIO_Output (LED)
   ├── PB3: GPIO_EXTI3 (Button)
   ├── PB4: GPIO_EXTI4 (Encoder B)
   ├── PB5: GPIO_EXTI5 (Encoder A)
   ├── PB6: I2C1_SCL (Display)
   └── PB7: I2C1_SDA (Display)

   Peripheral Configuration:
   ├── ADC1: 12-bit, single conversion, channels 3&4
   ├── I2C1: Standard mode, 100kHz
   ├── TIM6: Basic timer, 10Hz interrupt
   ├── USART1: 115200 baud, 8N1
   └── NVIC: Enable required interrupts

   Clock Configuration:
   ├── HSI: 16MHz internal oscillator
   ├── PLL: PLLMUL=4, PLLDIV=2 → 32MHz
   ├── SYSCLK: 32MHz
   └── AHB/APB1/APB2: 32MHz (no division)

4. Generate code and import to workspace
```

## ⚙️ Build Configuration

### Compiler Optimization Settings

#### Release Configuration
```
Optimization Level: -Os (Optimize for size)
Rationale: 32KB Flash limitation requires size optimization

Additional Flags:
├── -ffunction-sections (Enable dead code elimination)
├── -fdata-sections (Remove unused data)  
├── -Wall (Enable all warnings)
├── -Wextra (Additional warnings)
└── -Werror (Treat warnings as errors - optional)

Preprocessor Defines:
├── USE_HAL_DRIVER
├── STM32L052xx
└── DEBUG (for debug builds only)
```

#### Debug Configuration
```
Optimization Level: -Og (Optimize for debugging)
Debug Information: -g3 (Maximum debug info)

Additional Debug Flags:
├── -DDEBUG (Enable debug code)
├── -DUSE_FULL_ASSERT (Enable HAL assertions)
└── Stack usage reporting: -fstack-usage

Memory Protection:
├── Stack check: -fstack-protector-strong
├── Buffer overflow: -D_FORTIFY_SOURCE=2
└── Format string: -Wformat-security
```

### Linker Configuration

#### Memory Layout Verification
```
Linker Script: STM32L052K6TX_FLASH.ld

Memory Regions:
FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 8K
EEPROM (r)  : ORIGIN = 0x08080000, LENGTH = 2K (not used)

Stack Size: 0x400 (1KB)
Heap Size:  0x200 (512B)

Section Placement:
├── .isr_vector: Interrupt vector table (Flash start)
├── .text: Program code and constants
├── .rodata: Read-only data (string literals)  
├── .data: Initialized variables (copied to RAM)
├── .bss: Uninitialized variables (zeroed RAM)
└── .stack: Stack space (top of RAM)
```

#### Link-Time Optimization
```
Enable LTO: -flto (Link-time optimization)
Benefits:
├── Better dead code elimination
├── Cross-module inlining
├── Smaller binary size
└── Potential performance improvement

Linker Flags:
├── --gc-sections (Garbage collect unused sections)
├── --print-memory-usage (Show memory usage)
├── --cref (Cross-reference table)
└── -Map=output.map (Generate memory map)
```

### Build Targets and Configurations

#### Standard Build Targets
```
Debug Build:
├── Purpose: Development and debugging
├── Optimization: -Og (debug-friendly)
├── Debug Info: Full (-g3)
├── Assertions: Enabled
├── Size: ~35KB (may not fit in Flash)
└── Use: Development only

Release Build:
├── Purpose: Production deployment
├── Optimization: -Os (size optimized)  
├── Debug Info: Minimal (-g1)
├── Assertions: Disabled
├── Size: ~28-31KB (fits in 32KB Flash)
└── Use: Final deployment

Custom Build Configurations:
├── Size-Optimized: Maximum size reduction
├── Speed-Optimized: Performance critical builds
├── Debug-Release: Release with debug symbols
└── Test: Special configurations for testing
```

## 🐛 Debug Configuration

### ST-Link Debug Setup

#### Hardware Connection
```
SWD Interface Connections:
├── Pin 1: VDD (3.3V) - Power reference
├── Pin 2: SWDIO (PA13) - Data line
├── Pin 3: GND - Ground reference
├── Pin 4: SWCLK (PA14) - Clock line
└── Pin 5: NRST (Reset) - Optional reset control

Connection Verification:
1. Connect ST-Link to target board
2. Launch STM32CubeIDE
3. Window → Show View → STM32 ST-LINK Utility
4. Connect → Connect to the target
5. Verify MCU detection: STM32L052K6T6
```

#### Debug Configuration Setup
```
1. Right-click project → Debug As → Debug Configurations
2. Create new "STM32 Cortex-M C/C++ Application"
3. Configuration tabs:

   Main Tab:
   ├── Project: insa-ge-create-power-meter
   ├── C/C++ Application: Debug/project-name.elf
   └── Connection: ST-LINK (OpenOCD)

   Debugger Tab:
   ├── Debug probe: ST-LINK (OpenOCD)
   ├── Interface: SWD (Serial Wire Debug)
   ├── Target: STM32L052K6T6
   ├── Reset Mode: Software system reset
   └── Speed: 4000 kHz (default)

   Startup Tab:
   ├── Initialization Commands: Default
   ├── Load Image: Enabled
   ├── Set PC to: Reset_Handler  
   └── Resume: Enabled
```

### Debug Features and Usage

#### Breakpoint Management
```
Setting Breakpoints:
1. Double-click line number in editor
2. Or right-click → Toggle Breakpoint
3. Conditional breakpoints: Right-click breakpoint → Properties

Useful Breakpoint Locations:
├── Timer_Interrupt_Handler() - Main processing cycle
├── User_Button_Interrupt_Handler() - User input
├── Convert_ADC_to_Voltage() - Measurement processing
├── ssd1306_UpdateScreen() - Display updates
└── Error_Handler() - Error conditions

Breakpoint Types:
├── Line breakpoints: Stop at specific line
├── Function breakpoints: Stop at function entry
├── Watchpoints: Stop on variable access
└── Exception breakpoints: Stop on system exceptions
```

#### Variable Inspection
```
Live Variable Monitoring:
1. Debug → Debug Configurations → Variables view
2. Add global variables to watch:
   ├── measured_voltage
   ├── measured_current  
   ├── calculated_power
   ├── accumulated_energy
   └── current_menu

Memory Browser:
1. Window → Show View → Memory Browser
2. Monitor specific addresses:
   ├── 0x20000000: RAM start
   ├── 0x08000000: Flash start
   └── Variable addresses from map file

SFR (Special Function Register) View:
1. Window → Show View → SFRs
2. Monitor peripheral registers:
   ├── ADC1: ADC conversion results
   ├── I2C1: Communication status
   ├── TIM6: Timer operation
   └── GPIO: Pin states
```

### Performance Analysis Tools

#### Execution Time Measurement
```
Cycle Counting:
1. Enable DWT (Data Watchpoint and Trace) unit
2. Use DWT_CYCCNT register for cycle counting
3. Measure function execution time

Code Example:
```c
#ifdef DEBUG
#include "core_cm0plus.h"

uint32_t start_cycles, end_cycles, elapsed_cycles;

// Start measurement
start_cycles = DWT->CYCCNT;

// Function to measure
Timer_Interrupt_Handler();

// End measurement  
end_cycles = DWT->CYCCNT;
elapsed_cycles = end_cycles - start_cycles;

// Convert to microseconds (at 32MHz)
uint32_t time_us = elapsed_cycles / 32;
```

#### Stack Usage Analysis
```
Stack Monitoring:
1. Enable stack usage reports: -fstack-usage
2. Check .su files in build output
3. Monitor stack pointer during debugging

Stack Overflow Detection:
├── Fill stack with pattern at startup
├── Check pattern integrity periodically  
├── Set watchpoint on stack limit
└── Use static analysis tools

Memory Usage Analysis:
1. Check map file for section sizes
2. Monitor heap usage (if dynamic allocation used)
3. Track global variable sizes
4. Identify largest consumers
```

## 📁 Version Control Setup

### Git Configuration

#### Repository Initialization
```bash
# Initialize repository
git init
git remote add origin <repository-url>

# Configure .gitignore for STM32CubeIDE
cat > .gitignore << EOF
# Build outputs
Debug/
Release/
*.o
*.elf
*.bin
*.map
*.list

# IDE files
.metadata/
.settings/
*.launch

# Generated files  
Drivers/CMSIS/
Drivers/STM32L0xx_HAL_Driver/

# User files
*.user
*.tmp
*~

# Documentation builds
docs/_build/
EOF

# Initial commit
git add .
git commit -m "Initial commit: STM32L052K6T6 Power Meter"
git push -u origin main
```

#### Branch Strategy
```
Main Branches:
├── main: Stable production code
├── develop: Integration branch for features
├── feature/*: Individual feature development
└── hotfix/*: Critical bug fixes

Workflow:
1. Create feature branch from develop
2. Develop and test feature
3. Create pull request to develop
4. Merge to develop after review
5. Release from develop to main
```

### Code Quality Tools

#### Static Analysis Integration
```
PC-lint Plus (Commercial):
├── Download and install PC-lint Plus
├── Configure for ARM Cortex-M0+
├── Add custom rules for STM32 HAL
└── Integrate with IDE build process

Cppcheck (Free):
├── Install: sudo apt-get install cppcheck
├── Run analysis: cppcheck --enable=all src/
├── Generate reports for CI/CD
└── Configure custom rules

Clang Static Analyzer:
├── Install LLVM/Clang toolchain
├── Run: scan-build make
├── Review HTML reports
└── Fix identified issues
```

## 🔄 Alternative Build Methods

### Command Line Build

#### Makefile Generation
```bash
# Generate Makefile from CubeIDE
1. Right-click project → Properties
2. C/C++ Build → Tool Chain Editor
3. Select "Cross GCC" toolchain
4. Generate Makefile: make -n > Makefile

# Manual Makefile creation
cat > Makefile << 'EOF'
# STM32L052K6T6 Power Meter Makefile

TARGET = power-meter
MCU = cortex-m0plus

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

# Directories
SRCDIR = Core/Src
INCDIR = Core/Inc
DRVDIR = Drivers
BUILDDIR = build

# Sources
SOURCES = $(wildcard $(SRCDIR)/*.c)
SOURCES += $(wildcard $(SRCDIR)/ssd1306/*.c)
SOURCES += $(wildcard $(DRVDIR)/STM32L0xx_HAL_Driver/Src/*.c)
SOURCES += $(DRVDIR)/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c

# Includes
INCLUDES = -I$(INCDIR)
INCLUDES += -I$(INCDIR)/ssd1306
INCLUDES += -I$(DRVDIR)/STM32L0xx_HAL_Driver/Inc
INCLUDES += -I$(DRVDIR)/CMSIS/Device/ST/STM32L0xx/Include
INCLUDES += -I$(DRVDIR)/CMSIS/Include

# Defines
DEFINES = -DUSE_HAL_DRIVER -DSTM32L052xx

# Compiler flags
CFLAGS = -mcpu=$(MCU) -mthumb -mfloat-abi=soft
CFLAGS += -Wall -Wextra -Og -g3
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += $(INCLUDES) $(DEFINES)

# Linker flags
LDFLAGS = -mcpu=$(MCU) -mthumb
LDFLAGS += -specs=nano.specs -specs=nosys.specs
LDFLAGS += -Wl,--gc-sections -static
LDFLAGS += -Wl,-Map=$(BUILDDIR)/$(TARGET).map
LDFLAGS += -T STM32L052K6TX_FLASH.ld

# Build rules
all: $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).bin

$(BUILDDIR)/$(TARGET).elf: $(SOURCES)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).elf
	$(CP) -O binary $< $@

clean:
	rm -rf $(BUILDDIR)

flash: $(BUILDDIR)/$(TARGET).bin
	st-flash write $< 0x08000000

.PHONY: all clean flash
EOF

# Build project
make clean
make -j4
```

### Docker Build Environment

#### Dockerfile for Reproducible Builds
```dockerfile
# STM32 Development Environment
FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    wget \
    xz-utils \
    build-essential \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Install ARM toolchain
RUN wget -O gcc-arm.tar.xz \
    "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2" \
    && tar -xf gcc-arm.tar.xz -C /opt \
    && rm gcc-arm.tar.xz

# Add toolchain to PATH
ENV PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin:${PATH}"

# Set working directory
WORKDIR /workspace

# Copy project files
COPY . .

# Build command
CMD ["make", "all"]
```

## 🔧 Troubleshooting

### Common Build Issues

#### Flash Memory Overflow
```
Error: "region `FLASH' overflowed by XXX bytes"

Solutions:
1. Enable size optimization: -Os
2. Enable link-time optimization: -flto
3. Remove unused code and data
4. Simplify string literals
5. Reduce buffer sizes if possible

Memory Analysis:
├── Check .map file for largest consumers
├── Use nm tool: arm-none-eabi-nm --size-sort *.elf
├── Analyze object sizes: arm-none-eabi-objdump -h *.elf
└── Profile section usage: arm-none-eabi-readelf -S *.elf
```

#### Compiler/Linker Errors
```
Error: "undefined reference to..."
Solutions:
├── Check if source file is included in build
├── Verify function prototype matches implementation
├── Ensure required libraries are linked
└── Check for missing weak symbol definitions

Error: "multiple definition of..."
Solutions:
├── Remove duplicate function definitions
├── Check header file guards (#ifndef/#define/#endif)
├── Verify inline functions are properly declared
└── Resolve naming conflicts
```

#### Debug Connection Issues
```
Problem: ST-Link not detected
Solutions:
1. Check USB cable and connections
2. Install/update ST-Link drivers
3. Try different USB port
4. Check ST-Link firmware version
5. Use STM32CubeProgrammer to test connection

Problem: Cannot connect to target
Solutions:
1. Verify SWD connections (SWDIO, SWCLK, GND, VDD)
2. Check target power supply
3. Ensure target is not in STOP/STANDBY mode
4. Try connecting under reset
5. Check for hardware conflicts on debug pins
```

#### Runtime Issues
```
Problem: Hard fault or system reset
Debugging Steps:
1. Enable fault handlers in startup code
2. Check stack overflow (monitor SP register)
3. Verify pointer validity before dereferencing
4. Check array bounds access
5. Monitor for uninitialized variables

Problem: Incorrect measurements  
Debugging Steps:
1. Verify ADC configuration and calibration
2. Check voltage/current scaling factors
3. Test with known input values
4. Monitor ADC raw values vs. calculated
5. Check for electrical noise or grounding issues
```

### Performance Optimization

#### Build Time Optimization
```
Techniques:
├── Use parallel builds: make -j$(nproc)
├── Enable precompiled headers
├── Reduce include dependencies
├── Use incremental linking
└── Cache build artifacts

IDE Settings:
├── Increase heap size: -Xmx2g
├── Disable unnecessary plugins
├── Use workspace-specific settings
└── Enable build acceleration features
```

#### Code Size Optimization
```
Compiler Options:
├── -Os: Size optimization
├── -flto: Link-time optimization  
├── -ffunction-sections: Function-level linking
├── -fdata-sections: Data-level linking
└── -Wl,--gc-sections: Dead code elimination

Code Techniques:
├── Use const for read-only data
├── Minimize global variables
├── Use bit fields for flags
├── Optimize string storage
└── Remove debug code in release builds
```

---

*Document Version: 1.0*  
*Last Updated: 2024-12-24*  
*Development Environment: STM32CubeIDE 1.13.0+*