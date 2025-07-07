/**
 * @file ByteStreamController.h
 * @brief A controller for managing byte stream operations with asynchronous
 * data addition and synchronous data retrieval.
 */

#pragma once

#include <atomic>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <limits>
#include <mutex>
#include <span>
#include <iterator>
#include <utility>
#include <vector>

/**
 * @class ByteStreamController
 * @brief A thread-safe controller for byte stream operations with configurable
 * buffer size and timeout options.
 *
 * This class provides functionality for asynchronous data addition and
 * synchronous data retrieval with configurable buffer size, timeout, and error
 * handling.
 */
class ByteStreamController {
 public:
  using Byte = std::byte;  ///< Type alias for byte
  using ByteSpan =
      std::span<const Byte>;          ///< Type alias for read-only byte span
  using ByteVec = std::vector<Byte>;  ///< Type alias for byte vector
  using Callback =
      std::function<void(ByteSpan)>;  ///< Type alias for callback function

  /// Default buffer size (4096 bytes)
  static constexpr size_t DEFAULT_BUFFER_SIZE = 4096;
  /// Minimum read size (1 byte)
  static constexpr size_t MIN_READ_SIZE = 1;
  /// Default read size (512 bytes)
  static constexpr size_t DEFAULT_READ_SIZE = 512;
  /// Default device iterations (5)
  static constexpr size_t DEFAULT_DEVICE_ITERATIONS = 5;
  /// Default device data size (1024 bytes)
  static constexpr size_t DEFAULT_DEVICE_DATA_SIZE = 1024;
  /// Default device delay (100ms)
  static constexpr std::chrono::milliseconds DEFAULT_DEVICE_DELAY{100};
  /// Default read timeout (1000ms)
  static constexpr std::chrono::milliseconds DEFAULT_READ_TIMEOUT{1000};

  /**
   * @enum ErrorCode
   * @brief Error codes for stream operations
   */
  enum class ErrorCode {
    NoError,            ///< Operation completed successfully
    BufferOverflow,     ///< Operation would exceed buffer capacity
    ControllerStopped,  ///< Operation failed because controller was stopped
    Timeout             ///< Operation timed out
  };

  /**
   * @struct Result
   * @brief Result of synchronous data retrieval operation
   */
  struct Result {
    ByteVec data;                          ///< Retrieved data
    ErrorCode error = ErrorCode::NoError;  ///< Error code
    size_t dropped_bytes = 0;              ///< Number of dropped bytes (if any)
    size_t buffer_size = 0;  ///< Current buffer size at time of operation

    /**
     * @brief Conversion to bool indicating success
     * @return true if no error occurred, false otherwise
     */
    explicit operator bool() const { return error == ErrorCode::NoError; }
  };

  /**
   * @brief Construct a new ByteStreamController object
   * @param max_buffer_size Maximum buffer size in bytes (default:
   * DEFAULT_BUFFER_SIZE)
   */
  explicit ByteStreamController(size_t max_buffer_size = DEFAULT_BUFFER_SIZE);

  /**
   * @brief Destroy the ByteStreamController object
   * Stops the controller and cleans up resources
   */
  ~ByteStreamController();

  // Delete copy and move operations
  ByteStreamController(const ByteStreamController&) = delete;
  ByteStreamController& operator=(const ByteStreamController&) = delete;
  ByteStreamController(ByteStreamController&&) = delete;
  ByteStreamController& operator=(ByteStreamController&&) = delete;

  /**
   * @brief Stop the controller
   *
   * Stops all operations and wakes up any waiting threads
   */
  void stop();

  /**
   * @brief Start the controller
   *
   * Resets the controller to operational state
   */
  void start();

  /**
   * @brief Asynchronously add data to the buffer
   * @param data Span of bytes to add
   * @return ErrorCode indicating success or failure
   */
   ErrorCode async_add_data(ByteSpan data);

  /**
   * @brief Synchronously get data from the buffer
   * @param min_bytes Minimum number of bytes to retrieve (default:
   * MIN_READ_SIZE)
   * @param max_bytes Maximum number of bytes to retrieve (default: unlimited)
   * @param timeout Maximum time to wait for data (default:
   * DEFAULT_READ_TIMEOUT)
   * @return Result structure containing data and operation status
   */
  Result sync_get_data(
      size_t min_bytes = MIN_READ_SIZE,
      size_t max_bytes = std::numeric_limits<size_t>::max(),
      std::chrono::milliseconds timeout = DEFAULT_READ_TIMEOUT);

  /**
   * @brief Get current buffer size
   * @return Current number of bytes in buffer
   */
  size_t current_buffer_size() const;

  /**
   * @brief Check if controller is stopped
   * @return true if controller is stopped, false otherwise
   */
  bool is_stopped() const;

  /**
   * @brief Get a callback for asynchronous data addition
   * @return Callback function that can be used to add data
   */
  Callback get_callback();

 private:
  mutable std::mutex mutex_;      ///< Mutex for thread safety
  std::condition_variable cv_;    ///< Condition variable for synchronization
  ByteVec buffer_;                ///< Internal data buffer
  const size_t max_buffer_size_;  ///< Maximum buffer capacity
  std::atomic<bool> stopped_;     ///< Atomic flag indicating stopped state
};