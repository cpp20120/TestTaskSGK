#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include "../include/async_controller.hpp"

static void device_simulation(const ByteStreamController::Callback& callback,
                              bool& running) {
  for (size_t i = 0; i < ByteStreamController::DEFAULT_DEVICE_ITERATIONS; ++i) {
    if (!running) break;

    std::vector<std::byte> data(ByteStreamController::DEFAULT_DEVICE_DATA_SIZE,
                                static_cast<std::byte>(i));
    callback(data);
    std::this_thread::sleep_for(ByteStreamController::DEFAULT_DEVICE_DELAY);
  }
  running = false;
}

int main() {
  ByteStreamController controller;
  bool device_running = true;

  const std::chrono::milliseconds timeout(200);

  auto device_callback = controller.get_callback();
  std::thread device_thread(device_simulation, std::cref(device_callback),
                            std::ref(device_running));

  while (device_running || controller.current_buffer_size() > 0) {
    auto result = controller.sync_get_data(
        ByteStreamController::DEFAULT_READ_SIZE,
        ByteStreamController::DEFAULT_READ_SIZE,
        timeout);

    if (!result) {
      if (result.error == ByteStreamController::ErrorCode::Timeout) {
        continue;
      }
      std::cout << "Error occurred: " << static_cast<int>(result.error) << "\n";
      break;
    }

    std::cout << "Received " << result.data.size() << " bytes\n";
  }

  controller.stop();
  device_thread.join();

  std::cout << "Program finished successfully\n";
  return 0;
}