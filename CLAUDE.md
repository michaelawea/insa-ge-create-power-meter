# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an STM32L052K6T6-based power meter project developed using STM32CubeIDE. The project measures voltage and current inputs via ADC channels and displays the values on an SSD1306 OLED display.

## Build and Development Commands

This is an STM32CubeIDE project that uses Eclipse's managed build system. There are no command-line build commands - the project must be built using STM32CubeIDE:

1. **Build**: Use STM32CubeIDE → Project → Build Project (Ctrl+B)
2. **Clean**: Use STM32CubeIDE → Project → Clean
3. **Debug**: Use STM32CubeIDE → Run → Debug As → STM32 Cortex-M C/C++ Application
4. **Generate Code from CubeMX**: Open `insa-ge-create-power-meter.ioc` in STM32CubeMX and regenerate

## Architecture and Structure

### Hardware Configuration
- **MCU**: STM32L052K6T6 (32KB Flash, 8KB RAM, 32MHz)
- **ADC Channels**: PA3 (current), PA4 (voltage)
- **Display**: SSD1306 OLED via I2C1 (PB6=SCL, PB7=SDA)
- **UI Inputs**: Rotary encoder (PB4/PB5) with button (PB3)
- **UART1**: PA9/PA10 for serial communication
- **Timer**: TIM6 for 100ms measurement updates

### Code Architecture
The project follows STM32 HAL architecture with interrupt-driven peripherals:

1. **Main Loop** (`main.c`): Handles display updates and non-critical tasks
2. **Interrupt Handlers** (`stm32l0xx_it.c`):
   - `TIM6_DAC_IRQHandler`: Triggers ADC readings every 100ms
   - `HAL_ADC_ConvCpltCallback`: Processes ADC results
   - `HAL_GPIO_EXTI_Callback`: Handles button and encoder inputs

3. **Display Driver** (`ssd1306/`): Complete SSD1306 OLED driver with graphics primitives
4. **HAL Configuration** (`stm32l0xx_hal_conf.h`): Enables ADC, I2C, TIM, UART, GPIO modules

### Key Global Variables
- `adc_current`: Current ADC reading (12-bit)
- `adc_voltage`: Voltage ADC reading (12-bit)
- `encoder_count`: Rotary encoder position
- `button_pressed`: Button state flag

### Peripheral Initialization Flow
1. System clock configuration (32MHz from HSI+PLL)
2. GPIO initialization for LED, button, encoder
3. ADC initialization with DMA for two channels
4. I2C1 initialization for display
5. UART1 initialization
6. TIM6 initialization for periodic measurements
7. SSD1306 display initialization

## Important Notes

- The project uses STM32CubeIDE's managed build system - no Makefile exists
- ADC values are currently raw 12-bit values (0-4095) from potentiometers
- Power calculation logic is not yet implemented
- The SSD1306 driver is included in the project (not an external dependency)
- All pin configurations are defined in the `.ioc` file and generated into `main.c`