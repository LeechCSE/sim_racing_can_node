# 🏎️ Sim Racing CAN Node (Zephyr RTOS & C++)

## 📖 Overview
This project is a **CAN Bus Node implementation for a Sim Racing Steering Wheel**, developed using **Zephyr RTOS** and **Modern C++**. It emulates the gear shifting mechanism of a racing wheel, transmitting gear state data cyclically over the CAN bus.

To overcome the lack of a physical CAN Transceiver during the initial development phase, this project utilizes **Zephyr's Virtual CAN Loopback Driver**. This approach allows for the verification of the Application Layer (C++ Classes, RTOS Threads, and Timing) without hardware dependencies, demonstrating a flexible engineering approach to hardware constraints.

## 🛠️ Tech Stack & Development Environment
* **Operating System:** Zephyr RTOS (v4.3.0+)
* **Language:** C++17 (Modern C++ with RAII pattern, No-Heap policy)
* **Build System:** West (Zephyr Meta-Tool), CMake, Ninja
* **Configuration:** Kconfig (`prj.conf`), DeviceTree Overlay (`app.overlay`)
* **Hardware Target:** Espressif ESP32 DevKitC V4 (Xtensa LX6)
* **Debugging:** `idf_monitor`, West Build Tools

## 🔌 Hardware Specifications
* **MCU:** ESP32-WROOM-32
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