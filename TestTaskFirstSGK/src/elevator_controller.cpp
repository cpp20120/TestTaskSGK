#include "../include/elevator_controller.hpp"

#include <algorithm>
#include <expected>
#include <string>

#include "../include/direction.hpp"

namespace elevator {

std::string make_error_message(ElevatorError error, int floor) {
  switch (error) {
    case ElevatorError::InvalidFloor:
      return "Invalid floor: " + std::to_string(floor) + ". Must be between " +
             std::to_string(elevator::ElevatorController::kMinFloor) + " and " +
             std::to_string(elevator::ElevatorController::kMaxFloor);
    default:
      return "Unknown elevator error";
  }
}

std::expected<void, ElevatorError> ElevatorController::validateFloor(
    int floor) {
  if (floor < kMinFloor || floor > kMaxFloor) {
    return std::unexpected(ElevatorError::InvalidFloor);
  }
  return {};
}

std::expected<void, ElevatorError> ElevatorController::addInternalRequest(
    int floor) {
  if (auto validation = validateFloor(floor); !validation) {
    return validation;
  }
  internal_requests_.insert(floor);
  updateDirection();
  return {};
}

std::expected<void, ElevatorError> ElevatorController::addExternalRequest(
    int floor) {
  if (auto validation = validateFloor(floor); !validation) {
    return validation;
  }
  external_requests_.insert(floor);
  updateDirection();
  return {};
}

void ElevatorController::move() {
  if (direction_ == Direction::IDLE) {
    return;
  }

  internal_requests_.erase(current_floor_);
  external_requests_.erase(current_floor_);

  if (direction_ == Direction::UP && current_floor_ < kMaxFloor) {
    ++current_floor_;
  } else if (direction_ == Direction::DOWN && current_floor_ > kMinFloor) {
    --current_floor_;
  }

  internal_requests_.erase(current_floor_);
  external_requests_.erase(current_floor_);

  updateDirection();
}

bool ElevatorController::hasRequests() const noexcept {
  return !internal_requests_.empty() || !external_requests_.empty();
}

void ElevatorController::updateDirection() noexcept {
  internal_requests_.erase(current_floor_);
  external_requests_.erase(current_floor_);

  if (!hasRequests()) {
    direction_ = Direction::IDLE;
    return;
  }

  if (direction_ == Direction::UP) {
    direction_ = hasRequestsAbove() ? Direction::UP : Direction::DOWN;
  } else if (direction_ == Direction::DOWN) {
    direction_ = hasRequestsBelow() ? Direction::DOWN : Direction::UP;
  } else {  // IDLE
    if (hasRequestsAbove()) {
      direction_ = Direction::UP;
    } else if (hasRequestsBelow()) {
      direction_ = Direction::DOWN;
    }
  }
}

bool ElevatorController::hasRequestsAbove() const noexcept {
  auto is_above = [this](int floor) { return floor > current_floor_; };
  return std::ranges::any_of(internal_requests_, is_above) ||
         std::ranges::any_of(external_requests_, is_above);
}

bool ElevatorController::hasRequestsBelow() const noexcept {
  auto is_below = [this](int floor) { return floor < current_floor_; };
  return std::ranges::any_of(internal_requests_, is_below) ||
         std::ranges::any_of(external_requests_, is_below);
}

}  // namespace elevator