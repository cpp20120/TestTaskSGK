#include <cassert>
#include <iostream>

#include "../include/direction.hpp"
#include "../include/elevator_controller.hpp"

namespace {
constexpr int kFirstTestFloor = 3;
constexpr int kSecondTestFloor = 5;
constexpr int kThirdTestFloor = 1;
constexpr int kFourthTestFloor = 9;
constexpr int kFifthTestFloor = 7;
constexpr int kSixthTestFloor = 4;
constexpr int kSeventhTestFloor = 2;
constexpr int kEighthTestFloor = 6;
}  // namespace

void runTests() {
  // Test 1
  {
    elevator::ElevatorController ec;
    assert(ec.getCurrentFloor() == elevator::ElevatorController::kMinFloor);

    auto result1 = ec.addExternalRequest(kFirstTestFloor);
    assert(result1 && "Failed to add external request");
    assert(ec.getCurrentDirection() == elevator::Direction::UP);

    ec.move();  // 1 -> 2
    assert(ec.getCurrentFloor() == 2);
    ec.move();  // 2 -> 3
    assert(ec.getCurrentFloor() == kFirstTestFloor);
    assert(ec.getCurrentDirection() == elevator::Direction::IDLE);
  }

  // Test 2
  {
    elevator::ElevatorController ec;
    auto result1 = ec.addExternalRequest(kSecondTestFloor);  // floor 5
    assert(result1);

    auto result2 = ec.addInternalRequest(kFirstTestFloor);  // floor 3
    assert(result2);

    for (int i = 0; i < 4; ++i) {
      ec.move();  // 1 -> 5
    }
    assert(ec.getCurrentFloor() == kSecondTestFloor);  // floor 5

    auto result3 = ec.addInternalRequest(kSeventhTestFloor);  // floor 2
    assert(result3);
    assert(ec.getCurrentDirection() == elevator::Direction::DOWN);

    ec.move();                                        // 5 -> 4
    ec.move();                                        // 4 -> 3
    assert(ec.getCurrentFloor() == kFirstTestFloor);  // floor 3
    assert(ec.getCurrentDirection() == elevator::Direction::DOWN);

    ec.move();                                          // 3 -> 2
    assert(ec.getCurrentFloor() == kSeventhTestFloor);  // floor 2
    assert(ec.getCurrentDirection() == elevator::Direction::IDLE);
  }

  // Test 3
  {
    const int eight_floor = 8;

    elevator::ElevatorController ec;
    assert(ec.getCurrentFloor() == elevator::ElevatorController::kMinFloor);

    auto result1 = ec.addExternalRequest(kThirdTestFloor);
    assert(result1);

    ec.move();
    assert(ec.getCurrentFloor() == kThirdTestFloor);
    assert(ec.getCurrentDirection() == elevator::Direction::IDLE);

    auto result2 = ec.addExternalRequest(kFourthTestFloor);
    assert(result2);
    assert(ec.getCurrentDirection() == elevator::Direction::UP);

    for (int i = 0; i < eight_floor; ++i) {
      ec.move();
    }  // 1 -> 9
    assert(ec.getCurrentFloor() == kFourthTestFloor);
    assert(ec.getCurrentDirection() == elevator::Direction::IDLE);

    auto result3 = ec.addInternalRequest(kThirdTestFloor);
    assert(result3);
    assert(ec.getCurrentDirection() == elevator::Direction::DOWN);

    for (int i = 0; i < eight_floor; ++i) {
      ec.move();
    }  // 9 -> 1
    assert(ec.getCurrentFloor() == kThirdTestFloor);
    assert(ec.getCurrentDirection() == elevator::Direction::IDLE);
  }

  std::cout << "All tests passed!\n";
}

void simulationDemo() {
  elevator::ElevatorController ec;

  std::cout << "\nElevator Controller Simulation Demo\n";
  std::cout << "Initial floor: " << ec.getCurrentFloor() << "\n";

  constexpr int kFirstRequest = 5;
  if (auto result = ec.addExternalRequest(kFirstRequest); !result) {
    std::cerr << "Error: " << make_error_message(result.error(), kFirstRequest)
              << "\n";
    return;
  }
  std::cout << "Added external request for floor " << kFirstRequest << "\n";

  while (ec.getCurrentFloor() != kFirstRequest) {
    ec.move();
    std::cout << "Moved to floor " << ec.getCurrentFloor() << "\n";
  }

  // Test 4
  constexpr int kSecondRequest = 2;
  if (auto result = ec.addInternalRequest(kSecondRequest); !result) {
    std::cerr << "Error: " << make_error_message(result.error(), kSecondRequest)
              << "\n";
    return;
  }
  std::cout << "Added internal request for floor " << kSecondRequest << "\n";

  while (ec.getCurrentFloor() != kSecondRequest) {
    ec.move();
    std::cout << "Moved to floor " << ec.getCurrentFloor() << "\n";
  }

  // Test 5
  constexpr int kThirdRequest = 7;
  constexpr int kFourthRequest = 4;
  constexpr int kFifthRequest = 9;
  constexpr int kSixthRequest = 6;

  assert(ec.addExternalRequest(kThirdRequest));
  assert(ec.addExternalRequest(kFourthRequest));
  assert(ec.addInternalRequest(kFifthRequest));
  assert(ec.addInternalRequest(kSixthRequest));

  while (ec.hasRequests()) {
    ec.move();
    std::cout << "Moved to floor " << ec.getCurrentFloor();
    std::cout << " (Direction: ";
    switch (ec.getCurrentDirection()) {
      case elevator::Direction::UP:
        std::cout << "UP";
        break;
      case elevator::Direction::DOWN:
        std::cout << "DOWN";
        break;
      case elevator::Direction::IDLE:
        std::cout << "IDLE";
        break;
    }
    std::cout << ")\n";
  }
}

int main() {
  runTests();

  simulationDemo();

  return 0;
}