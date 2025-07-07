/**
 * @file elevator_controller.hpp
 * @brief Implements a controller for managing elevator movement and requests.
 *
 */

#pragma once
#include <cstdint>
#include <expected>
#include <string>
#include <unordered_set>

#include "direction.hpp"

namespace elevator {

/**
 * @enum ElevatorError
 * @brief Errors that can occur during elevator operation.
 *
 * @var ElevatorError::InvalidFloor
 *      Requested floor is outside valid range (1-9).
 */
enum class ElevatorError : uint8_t { InvalidFloor };

/**
 * @brief Generates a human-readable error message for ElevatorError.
 * @param error The error type.
 * @param floor The invalid floor (if applicable).
 * @return Descriptive error string.
 */
std::string make_error_message(ElevatorError error, int floor);

/**
 * @class ElevatorController
 * @brief Manages elevator state, requests, and movement logic.
 */
class ElevatorController {
 public:
  ElevatorController() noexcept = default;
  ~ElevatorController() = default;

  /**
   * @brief Adds a request from inside the elevator (passenger input).
   * @param floor Target floor (1-9).
   * @return std::expected<void, ElevatorError>
   *         - Success: void
   *         - Failure: ElevatorError with details.
   */
  [[nodiscard]] std::expected<void, ElevatorError> addInternalRequest(
      int floor);

  /**
   * @brief Adds a request from outside the elevator (hall call).
   * @param floor Requested floor (1-9).
   * @return std::expected<void, ElevatorError>
   *         - Success: void
   *         - Failure: ElevatorError with details.
   */
  [[nodiscard]] std::expected<void, ElevatorError> addExternalRequest(
      int floor);

  /**
   * @brief Moves the elevator one floor in the current direction.
   * @note Automatically stops at requested floors and updates direction.
   */
  void move();

  /// @return Current floor (1-9).
  [[nodiscard]] int getCurrentFloor() const noexcept { return current_floor_; }

  /// @return Current movement direction (UP/DOWN/IDLE).
  [[nodiscard]] Direction getCurrentDirection() const noexcept {
    return direction_;
  }

  /// @return True if there are pending requests.
  [[nodiscard]] bool hasRequests() const noexcept;

  // Constants for floor bounds.
  static constexpr int kMinFloor = 1;  ///< Minimum valid floor.
  static constexpr int kMaxFloor = 9;  ///< Maximum valid floor.

 private:
  int current_floor_{kMinFloor};               ///< Current floor position.
  Direction direction_{Direction::IDLE};       ///< Current movement direction.
  std::unordered_set<int> internal_requests_;  ///< Passenger-selected floors.
  std::unordered_set<int> external_requests_;  ///< Hall-call requests.

  // Validates a floor number (1-9).
  [[nodiscard]] static std::expected<void, ElevatorError> validateFloor(
      int floor);

  // Updates direction based on pending requests.
  void updateDirection() noexcept;

  /// @return True if there are requests above current floor.
  [[nodiscard]] bool hasRequestsAbove() const noexcept;

  /// @return True if there are requests below current floor.
  [[nodiscard]] bool hasRequestsBelow() const noexcept;
};

}  // namespace elevator