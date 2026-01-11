# 🏎️ Sim Racing CAN Node (Zephyr RTOS & C++)

## 📖 Overview
This project is a **CAN Bus Node implementation for a Sim Racing Steering Wheel**, developed using **Zephyr RTOS** and **Modern C++**. It emulates the gear shifting mechanism of a racing wheel, transmitting gear state data cyclically over the CAN bus.

To overcome the lack of a physical CAN Transceiver during the initial development phase, this project utilizes **Zephyr's Virtual CAN Loopback Driver**. This approach allows for the verification of the Application Layer (C++ Classes, RTOS Threads, and Timing) without hardware dependencies, demonstrating a flexible engineering approach to hardware constraints.

## 🛠️ Tech Stack & Development Environment
* **Operating System:** Zephyr RTOS (v4.3.0+)
* **Host Environment:** Windows 11 + WSL2 (Ubuntu 22.04) with VS Code Remote
* **Language:** C++17 (Modern C++ with RAII pattern, No-Heap policy)
* **Build System:** West (Zephyr Meta-Tool), CMake, Ninja
* **Configuration:** Kconfig (`prj.conf`), DeviceTree Overlay (`app.overlay`)
* **Hardware Target:** Espressif ESP32 DevKitC V4 (ESP32-WROOM-32)
* **Debugging:** `idf_monitor`, West Build Tools

## 🔌 Hardware Specifications
* **Board:** ESP32 DevKitC V4
* **MCU:** ESP32-WROOM-32 (ESP32-D0WD-V3, Xtensa LX6 Dual Core)
* **CAN Transceiver:** **Virtual / Not Required**
    * *Note: This project is configured to use the internal `zephyr,can-loopback` driver. To deploy on physical hardware (e.g., with an SN65HVD230 transceiver), the `prj.conf` and `app.overlay` must be modified to disable loopback mode.*

## 💡 Key Technical Features

### 1. Virtual Hardware Abstraction
* Utilized `CONFIG_CAN_LOOPBACK` to create a virtual CAN bus environment.
* Abstracted hardware dependencies using **DeviceTree Overlays**, allowing the software logic to remain agnostic to the physical layer.

### 2. Object-Oriented Design (OOP)
* Implemented the `SimWheel` class to encapsulate hardware initialization and gear-shifting logic.
* Applied the **RAII (Resource Acquisition Is Initialization)** pattern to ensure the CAN device is ready immediately upon object creation.

### 3. Embedded Safety & Stability
* **No-Heap Policy:** All objects are allocated on the stack or statically. No `new` or `malloc` is used to prevent memory fragmentation and ensure deterministic behavior.
* **Thread Safety:** Logic is executed within a dedicated Zephyr thread (`k_thread`), ensuring real-time performance.

## 📂 Project Structure
```text
sim_racing_can_node/
├── src/
│   └── main.cpp          # Main application logic (C++ Class & Threading)
├── app.overlay           # DeviceTree overlay for Virtual CAN configuration
├── prj.conf              # Kconfig file for enabling C++ and CAN drivers
├── CMakeLists.txt        # CMake build configuration
└── README.md             # Project documentation
```

## 🚀 How to Build & Run (ESP32-WROOM-32)

### Prerequisites
* Zephyr SDK installed
* West build tool configured
* **ESP32-WROOM-32** board connected via USB (e.g., `/dev/ttyUSB0`)

### 1. Build (Virtual Mode)
Build the project for the `esp32_devkitc` board (compatible with WROOM-32).
```bash
west build -p always -b esp32_devkitc/esp32/procpu .
```

### 2. Flash Firmware
Flash the compiled binary to the ESP32 chip.
```bash
west flash
```

### 3. Monitor Logs (UART Output)
Open the serial monitor to view the real-time gear shifting logs.
```bash
west espressif monitor
```

## 📝 Example Output (Real UART Log)
Below is the actual UART output captured from an **ESP32-WROOM-32** device running the simulation. It demonstrates the successful initialization of the Virtual Loopback driver and the cyclic gear shifting logic (1 → 6 → 0).

```text
west espressif monitor
-- west flash: rebuilding
ninja: no work to do.
-- west flash: using runner esp32
-- runners.esp32: reset after flashing requested
-- runners.esp32: Flashing esp32 chip on None (921600bps)
esptool v5.1.0
Connected to ESP32 on /dev/ttyUSB0:
Chip type:         ESP32-D0WD-V3 (revision v3.1)
Features:          Wi-Fi, BT, Dual Core + LP Core, 240MHz, Vref calibration in eFuse, Coding Scheme None
Crystal frequency: 40MHz
MAC:               cc:db:a7:32:25:40

Stub flasher running.
Changing baud rate to 921600...
Changed.

Configuring flash size...
Flash will be erased from 0x00001000 to 0x00023fff...
Wrote 147456 bytes at 0x00001000 in 2.5 seconds (477.3 kbit/s).
Hash of data verified.

Hard resetting via RTS pin...
Serial port /dev/ttyUSB0:
Connecting....
Detecting chip type... ESP32
--- idf_monitor on /dev/ttyUSB0 115200 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
ets Jul 29 2019 12:21:46
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3ffb0000,len:8308
load:0x40080000,len:42832
entry 0x40086244
I (35) soc_init: ESP Simple boot
I (35) soc_init: compile time Jan 11 2026 13:37:39
W (35) soc_init: Unicore bootloader
I (35) soc_init: chip revision: v3.1
I (39) flash_init: SPI Speed      : 40MHz
I (42) flash_init: SPI Mode       : DIO
I (46) flash_init: SPI Flash Size : 4MB
I (49) boot: DRAM        : lma=00001020h vma=3ffb0000h size=02074h (  8308)
I (56) boot: IRAM        : lma=0000309ch vma=40080000h size=0a750h ( 42832)
I (62) boot: IROM        : lma=00010000h vma=400d0000h size=0b0f4h ( 45300)
I (68) boot: DROM        : lma=00020000h vma=3f400000h size=03d54h ( 15700)
I (85) boot: libc heap size 173 kB.
I (85) spi_flash: detected chip: generic
I (85) spi_flash: flash io: dio
*** Booting Zephyr OS build v4.3.0-3505-ga989118dba78 ***
[00:00:00.119,000] <inf> sim_racing_node: SimWheel initialized successfully (Virtual Loopback Mode)
[00:00:00.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 1
[00:00:00.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 1

uart:~$ [00:00:02.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 2
[00:00:02.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 2
[00:00:02.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 2
[00:00:02.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 2
uart:~$ [00:00:04.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 3
[00:00:04.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 3
[00:00:04.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 3
[00:00:04.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 3
uart:~$ [00:00:06.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 4
[00:00:06.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 4
[00:00:06.119,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 4
[00:00:06.119,000] <inf> sim_racing_node: [TX] Gear Shifted -> 4
uart:~$ [00:00:08.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 5
[00:00:08.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 5
[00:00:08.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 5
[00:00:08.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 5
uart:~$ [00:00:10.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 6
[00:00:10.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 6
[00:00:10.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 6
[00:00:10.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 6
uart:~$ [00:00:12.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 0
[00:00:12.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 0
[00:00:12.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 0
[00:00:12.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 0
uart:~$ [00:00:14.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 1
[00:00:14.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 1
[00:00:14.120,000] <inf> sim_racing_node: >>> [RX] Base Unit received Gear: 1
[00:00:14.120,000] <inf> sim_racing_node: [TX] Gear Shifted -> 1
```