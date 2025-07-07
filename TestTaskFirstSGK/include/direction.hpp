/**
 * @file direction.hpp
 * @brief Defines movement directions for the elevator system.
 */

#pragma once
#include <cstdint>

namespace elevator {

/**
 * @enum Direction
 * @brief Represents the current movement direction of the elevator.
 *
 * @var Direction::UP
 *      Elevator is moving upwards.
 * @var Direction::DOWN
 *      Elevator is moving downwards.
 * @var Direction::IDLE
 *      Elevator is stationary (no active requests).
 */
enum class Direction : uint8_t { UP, DOWN, IDLE };

}  // namespace elevator