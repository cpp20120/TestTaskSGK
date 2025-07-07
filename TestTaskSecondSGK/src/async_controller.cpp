#include "../include/async_controller.hpp"

#include <algorithm>
#include <cstddef>

ByteStreamController::ByteStreamController(size_t max_buffer_size)
    : max_buffer_size_(max_buffer_size), stopped_(false) {
  buffer_.reserve(max_buffer_size);
}

ByteStreamController::~ByteStreamController() { stop(); }

void ByteStreamController::stop() {
  {
    const std::scoped_lock lock(mutex_);
    stopped_ = true;
  }
  cv_.notify_all();
}

void ByteStreamController::start() {
  const std::scoped_lock lock(mutex_);
  stopped_ = false;
  cv_.notify_all();
}

ByteStreamController::ErrorCode ByteStreamController::async_add_data(
    ByteSpan data) {
  if (stopped_.load(std::memory_order_relaxed)) {
    return ErrorCode::ControllerStopped;
  }

  {
    const std::scoped_lock lock(mutex_);

    if (buffer_.size() + data.size() > max_buffer_size_) {
      return ErrorCode::BufferOverflow;
    }

    buffer_.insert(buffer_.end(), data.begin(), data.end());
  }

  cv_.notify_one();
  return ErrorCode::NoError;
}

ByteStreamController::Result ByteStreamController::sync_get_data(
    size_t min_bytes, size_t max_bytes, std::chrono::milliseconds timeout) {
  if (min_bytes > max_bytes) {
    return Result{ByteVec{}, ErrorCode::InvalidArgs, 0, buffer_.size()};
  }

  std::unique_lock lock(mutex_);

  if (!cv_.wait_for(lock, timeout, [this, min_bytes] {
        return stopped_ || buffer_.size() >= min_bytes;
      })) {
    return Result{ByteVec{}, ErrorCode::Timeout, 0, buffer_.size()};
  }

  if (stopped_) {
    if (buffer_.empty()) {
      return Result{ByteVec{}, ErrorCode::ControllerStopped, 0, 0};
    }
  }

  const size_t bytes_available = buffer_.size();
  const size_t bytes_to_take = std::min(bytes_available, max_bytes);

  ByteVec result;
  result.reserve(bytes_to_take);
  result.insert(
      result.end(), std::make_move_iterator(buffer_.begin()),
      std::make_move_iterator(buffer_.begin() +
                              static_cast<std::ptrdiff_t>(bytes_to_take)));

  buffer_.erase(buffer_.begin(),
                buffer_.begin() + static_cast<std::ptrdiff_t>(bytes_to_take));

  return Result{std::move(result),
                stopped_ ? ErrorCode::ControllerStopped : ErrorCode::NoError,
                bytes_to_take, buffer_.size()};
}

size_t ByteStreamController::current_buffer_size() const {
  const std::scoped_lock lock(mutex_);
  return buffer_.size();
}

bool ByteStreamController::is_stopped() const {
  return stopped_.load(std::memory_order_relaxed);
}

ByteStreamController::Callback ByteStreamController::get_callback() {
  return [this](ByteSpan data) { async_add_data(data); };
}