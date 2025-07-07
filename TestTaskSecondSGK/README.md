## Решение тестового для СГК

### Тестовое задание ByteStreamController

### 1.1 Purpose
Потокобезопасный контроллер для управления потоками байтов с:
- Асинхронным добавлением данных
- Синхронным извлечением данных
- Настраиваемым размером буфера
- Обработкой тайм-аута
- Отчетами об ошибках

### 1.2 Architecture
```text
┌──────────────────────┐
│ ByteStreamController │
├──────────────────────┤
│ - Thread-safe buffer │
│ - Condition variable │
│ - Atomic state flag  │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│   Producer/Consumer  │
│      Pattern         │
└──────────────────────┘
```

###  **1.3 Methods**

| Method           | Parameters                                 | Returns     | Thread Safety | Description                |
|------------------|--------------------------------------------|-------------|---------------|----------------------------|
| `async_add_data` | `ByteSpan`                                 | `ErrorCode` | Thread-safe   | Non-blocking data addition |
| `sync_get_data`  | `min_bytes`, `max_bytes`, `timeout`        | `Result`    | Thread-safe   | Blocking data retrieval    |
| `stop`           | None                                       | `void`      | Thread-safe   | Stops all operations       |
| `start`          | None                                       | `void`      | Thread-safe   | Resumes operations         |

---

###  **1.4 State**

| Method                | Returns    | Description                 |
|-----------------------|------------|-----------------------------|
| `current_buffer_size` | `size_t`   | Current bytes in buffer     |
| `is_stopped`          | `bool`     | Controller state            |
| `get_callback`        | `Callback` | Producer callback generator |

---

###  **1.5 Error Codes**

| Code                | Description        | Possible Causes                |
|---------------------|--------------------|--------------------------------|
| `NoError`           | Success            | Normal operation               |
| `BufferOverflow`    | Exceeded capacity  | Adding data to full buffer     |
| `ControllerStopped` | Inactive state     | Operations after `stop()`      |
| `Timeout`           | Wait expired       | `sync_get_data` timeout        |



### How to build

```sh
mkdir build && cd build
cmake .. -G=Ninja # or another generator
ninja -j4 # or another number of thread
./TEST_TASK_SECOND_SGK.exe 
```