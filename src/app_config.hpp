/*
 * src/app_config.hpp
 * Application-wide configuration constants
 */

#pragma once

#include <zephyr/kernel.h>  // Required for K_SECONDS, K_MSEC

#include <cstdint>  // Required for uint32_t, uint8_t

namespace Config {
// CAN Bus Settings
constexpr uint32_t CAN_GEAR_MSG_ID = 0x100;
constexpr uint8_t CAN_MSG_DLC = 1;

// Thread Settings
constexpr size_t TX_THREAD_STACK_SIZE = 2048;
constexpr int TX_THREAD_PRIORITY = 5;

// Timing Settings
constexpr auto GEAR_SHIFT_INTERVAL = K_SECONDS(2);
constexpr auto TX_TIMEOUT = K_MSEC(100);
}  // namespace Config