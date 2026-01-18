/*
 * Copyright (c) 2026 Chanwoo Lee
 * SPDX-License-Identifier: Apache-2.0
 *
 * Sim Racing Gear Shift Emulator using Zephyr RTOS & Virtual CAN
 */

#include <zephyr/drivers/can.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <cstdint>  // uint8_t, uint32_t

#include "app_config.hpp"

/* Register Log Module */
LOG_MODULE_REGISTER(sim_racing_node, LOG_LEVEL_INF);

enum class Gear : uint8_t { N = 0, First, Second, Third, Fourth, Fifth, Sixth };

/* Overload prefix increment operator (++g) for cyclic shifting */
Gear& operator++(Gear& g) {
  using enum Gear;  // C++20 Feature: Reduces verbosity (no need for Gear::N,
                    // Gear::Sixth)

  // Cyclic Logic: If Sixth, reset to N. Otherwise, increment.
  g = (g == Sixth) ? N : static_cast<Gear>(static_cast<uint8_t>(g) + 1);
  return g;
}

/**
 * @brief SimWheel Class
 * * Encapsulates the CAN hardware abstraction and gear shifting logic.
 * Implements RAII pattern to ensure device readiness and configuration upon
 * instantiation.
 */
class SimWheel {
 private:
  const struct device* dev;
  Gear current_gear;

 public:
  /**
   * @brief Construct a new Sim Wheel object
   * * @param can_device Pointer to the Zephyr CAN device structure
   */
  SimWheel(const struct device* can_device)
      : dev(can_device), current_gear(Gear::N) {
    if (!device_is_ready(dev)) {
      LOG_ERR("CAN device not ready");
      return;
    }

    /* * Configure Loopback Mode.
     * This allows the node to receive its own messages, simulating a full bus
     * environment without requiring an external physical transceiver.
     */
    int ret = can_set_mode(dev, CAN_MODE_LOOPBACK);
    if (ret != 0) {
      LOG_ERR("Failed to set CAN mode: %d", ret);
    }

    ret = can_start(dev);
    if (ret != 0) {
      LOG_ERR("Failed to start CAN controller: %d", ret);
    }

    LOG_INF("SimWheel initialized successfully (Virtual Loopback Mode)");
  }

  /**
   * @brief Simulates a gear shift operation and transmits the state via CAN.
   * Cycles through gears N(0) -> 1..6 -> N(0).
   */
  void shift_gear() {
    struct can_frame frame = {0};

    /* Update Gear Logic using overloaded operator */
    ++current_gear;

    /* Prepare CAN Frame */
    frame.id = Config::CAN_GEAR_MSG_ID;
    frame.dlc = Config::CAN_MSG_DLC;
    // Explicit cast required for type safety (Enum Class -> uint8_t)
    frame.data[0] = static_cast<uint8_t>(current_gear);

    /* Transmit Frame (Non-blocking with timeout) */
    int ret = can_send(dev, &frame, Config::TX_TIMEOUT, NULL, NULL);

    if (ret == 0) {
      // Cast for logging display
      LOG_INF("[TX] Gear Shifted -> %d", static_cast<uint8_t>(current_gear));
    } else {
      LOG_ERR("CAN Send Failed (Error: %d)", ret);
    }
  }
};

/* Retrieve the CAN device from DeviceTree (Virtual or Physical) */
const struct device* const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

/**
 * @brief TX Thread Entry Point
 * * Runs the main application logic. The SimWheel object is allocated
 * on the stack to prevent memory fragmentation (No-Heap policy).
 */
void tx_thread_entry(void* arg1, void* arg2, void* arg3) {
  SimWheel myWheel(can_dev);

  while (1) {
    myWheel.shift_gear();
    k_sleep(Config::GEAR_SHIFT_INTERVAL);
  }
}

/**
 * @brief CAN RX Callback
 * * Interrupt context callback for processing received CAN frames.
 */
void can_rx_callback(const struct device* dev, struct can_frame* frame,
                     void* user_data) {
  if (frame->id == Config::CAN_GEAR_MSG_ID) {
    LOG_INF(">>> [RX] Base Unit received Gear: %d", frame->data[0]);
  }
}

/* Define and initialize the TX thread using Config constants */
K_THREAD_DEFINE(tx_tid, Config::TX_THREAD_STACK_SIZE, tx_thread_entry, NULL,
                NULL, NULL, Config::TX_THREAD_PRIORITY, 0, 0);

int main(void) {
  /* Configure RX Filter to accept specific IDs */
  struct can_filter filter = {.id = Config::CAN_GEAR_MSG_ID,
                              .mask = CAN_STD_ID_MASK};

  can_add_rx_filter(can_dev, &can_rx_callback, NULL, &filter);

  return 0;
}