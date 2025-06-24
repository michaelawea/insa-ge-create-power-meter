# STM32L052K6T6 Power Meter Documentation

## 📚 Documentation Structure

This documentation is organized into the following categories:

### 🔧 [Hardware Documentation](./hardware/)
- [Hardware Overview](./hardware/hardware-overview.md) - Complete hardware specifications
- [Pin Configuration](./hardware/pin-configuration.md) - Detailed pin mappings and connections
- [Schematic Guide](./hardware/schematic-guide.md) - Circuit design and connections
- [PCB Layout](./hardware/pcb-layout.md) - Board layout considerations
- [Component Specifications](./hardware/component-specs.md) - Bill of materials and specs

### 💻 [Software Documentation](./software/)
- [Software Architecture](./software/software-architecture.md) - System design overview
- [Code Structure](./software/code-structure.md) - File organization and modules
- [Core Functions](./software/core-functions.md) - Main functionality implementation
- [Interrupt Handling](./software/interrupt-handling.md) - Interrupt system design
- [Display System](./software/display-system.md) - SSD1306 OLED implementation

### 📖 [User Guide](./user-guide/)
- [Quick Start Guide](./user-guide/quick-start.md) - Getting started instructions
- [User Interface](./user-guide/user-interface.md) - How to use the device
- [Menu Navigation](./user-guide/menu-navigation.md) - Complete menu system guide
- [Measurement Guide](./user-guide/measurement-guide.md) - Understanding readings
- [Calibration](./user-guide/calibration.md) - Calibration procedures

### 🛠️ [Development Documentation](./development/)
- [Build Environment](./development/build-environment.md) - Setup instructions
- [Compilation Guide](./development/compilation-guide.md) - How to build the project
- [Debug Setup](./development/debug-setup.md) - Debugging configuration
- [Memory Optimization](./development/memory-optimization.md) - Flash/RAM optimization
- [Testing Procedures](./development/testing-procedures.md) - Testing guidelines

### 🔌 [API Documentation](./api/)
- [Function Reference](./api/function-reference.md) - Complete API documentation
- [Data Structures](./api/data-structures.md) - Structs and enums
- [Configuration Options](./api/configuration-options.md) - Build-time configurations
- [HAL Integration](./api/hal-integration.md) - STM32 HAL usage

### 🔍 [Troubleshooting](./troubleshooting/)
- [Common Issues](./troubleshooting/common-issues.md) - Frequently encountered problems
- [Debug Guide](./troubleshooting/debug-guide.md) - Debugging techniques
- [Error Codes](./troubleshooting/error-codes.md) - Error code reference
- [Performance Issues](./troubleshooting/performance-issues.md) - Performance optimization

## 📊 Project Overview

### Description
Real-time power meter for STM32L052K6T6 microcontroller featuring voltage/current measurement, power calculation, energy accumulation, and graphical display.

### Key Features
- **Real-time Monitoring**: Voltage (0-30V), Current (0-5A), Power calculation
- **Energy Tracking**: Accumulation with mWh/kWh display
- **Peak Detection**: Maximum value tracking
- **Graphical Display**: Real-time waveforms
- **Interactive UI**: Rotary encoder navigation
- **Compact Design**: Optimized for 32KB Flash

### Hardware Specifications
- **MCU**: STM32L052K6T6 (32KB Flash, 8KB RAM, 2KB EEPROM)
- **Display**: SSD1306 128x64 OLED via I2C
- **Inputs**: 2x 12-bit ADC channels
- **Interface**: Rotary encoder + button
- **Communication**: UART debug interface

### Software Features
- **Real-time OS**: Bare metal with timer-based scheduling
- **Display Engine**: Complete SSD1306 driver with graphics
- **Menu System**: Multi-level navigation
- **Data Processing**: Power calculations and energy integration
- **Memory Management**: Optimized for resource constraints

## 🚀 Quick Links

- **Getting Started**: [Quick Start Guide](./user-guide/quick-start.md)
- **Hardware Setup**: [Hardware Overview](./hardware/hardware-overview.md)
- **Software Setup**: [Build Environment](./development/build-environment.md)
- **API Reference**: [Function Reference](./api/function-reference.md)
- **Troubleshooting**: [Common Issues](./troubleshooting/common-issues.md)

## 📝 Documentation Standards

All documentation follows these standards:
- **Markdown Format**: Using GitHub-flavored markdown
- **Code Examples**: Syntax highlighted with language tags
- **Diagrams**: ASCII art or mermaid diagrams where applicable
- **Cross-references**: Internal links between related documents
- **Version Control**: Documentation versioned with code

## 🤝 Contributing

When contributing to this project:
1. Update relevant documentation for any code changes
2. Follow the existing documentation structure
3. Include code examples for new features
4. Test all documentation links
5. Update this README if adding new documentation sections

## 📄 License

This project and documentation are provided under the same license as the main codebase.

---

*Last Updated: 2024-12-24*  
*Version: 1.0*  
*Target Hardware: STM32L052K6T6*