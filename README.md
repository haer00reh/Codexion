# This project has been created as part of the 42 curriculum by **haer-reh**

## about Codexion
Codexion is a project about managing threads mutexes and condition variables. The goal is to create a program that simulates coders (threads) and a shared dongles. each coder have a time to compile debug and refactor, to compile they need to take two dongles, and each coder cant comminucate with each other and they should avoid burning out by taking too much time to compile. The program should end when all coders have compiled at least the number of times specified in the arguments.

the number of dongles is exactly the number of coders, and each coder need to take the dongle on their left and right to compile. the program should print the state of each coder every time they change state (compiling, debugging, refactoring).
## how to compile
To compile the program, you can use the following command in the terminal:
```make```
This will create an executable file named `codexion`.
## how to run the program, you can use the following command in the terminal:
```./codexion number_of_coders  time_to_burnout time_to_compile  time_to_debug   time_to_refactor   number_of_compiles_required   dongle_cooldown   scheduler (fifo or edf)```
For example:
```./codexion 5 800 200 100 300 3 50 fifo```
This will run the program with 5 coders, a burnout time of 800ms, a compile time of 200ms, a debug time of 100ms, a refactor time of 300ms, a requirement of 3 compiles for each coder, a dongle cooldown of 50ms, and a FIFO scheduler.
## how to clean the project, you can use the following command in the terminal:
```make fclean```
This will remove the executable file and any object files created during compilation.
```make clean```
to remove only the object files
## how to recompile To recompile the project, you can use the following command in the
terminal:
```make re```
This will first clean the project and then compile it again, creating a new executable file.

## resources
**the man** (linux manual)

**helpful youtube videos i watched**:

[▶ Video 1](https://youtu.be/ldJ8WGZVXZk?si=M0Gj9gajzUvNp4jg)

[▶ Video 2](https://youtu.be/uA8X5zNOGw8?si=Zx52pBCl3WZgZvG9)

[▶ Video 3](https://youtu.be/1myWEH8IGt4?si=hOvkS9zfIKIbVczL)

[▶ Video 4](https://youtu.be/d9s_d28yJq0?si=sv_lJasjFDoGtb1W)

[▶ Video 5](https://youtu.be/4rLW7zg21gI?si=uWbk_5VjjcOnIfon) 


this one good and straighforward: https://docs.oracle.com/cd/E26502_01/html/E35303/tlib-1.html

### AI usage
ai was used to help with norminette of the code, helping with detecting data races using thread sanitizer, and also used to help with autocompletion of this readme file. AI was rarely used to help with the logic of the code.
**peer learning** was heavily used in this project, as I had to discuss with my peers about the logic of the code and how to implement it, and also to help each other with debugging and testing the code.


## Blocking cases handled
This implementation addresses several critical concurrency issues through carefully designed synchronization:

- **Deadlock Prevention**: Deadlock is prevented through ordered dongle acquisition. The `set_ordered_dongles()` function ensures that all coders always acquire dongles in the same numerical order (by ID), eliminating the circular wait condition from Coffman's four conditions. Since every coder acquires the lower-ID dongle before the higher-ID dongle, no circular dependency can form.

- **Coffman's Conditions Mitigation**: By enforcing ordered acquisition, the circular wait condition (the fourth and only preventable Coffman condition) is eliminated. Combined with proper mutex semantics, this prevents deadlock entirely.

- **Starvation Prevention**: A custom heap-based priority queue (`t_heap`) maintains fairness in dongle access. Coders are queued with either FIFO or EDF (Earliest Deadline First) scheduling policies. The `can_take_dongle()` function ensures that only the head of the queue can acquire the dongle, preventing any coder from being starved indefinitely.

- **Cooldown Handling**: After a coder releases a dongle, a cooldown period is enforced through the `dongle->available_at` timestamp. The `wait_dongle_until_ready()` function uses `pthread_cond_timedwait()` to block until both the cooldown expires and the dongle becomes available, preventing rapid reacquisition while still allowing fair scheduling.

- **Precise Burnout Detection**: The monitor thread checks the `last_compile_start` timestamp (protected by `counter_mutex`) against the current time. Burnout is detected only when the elapsed time exceeds `time_to_burnout` and the coder has not yet finished all required compilations. The timestamp is updated atomically at the exact moment compilation begins, not when waiting for dongles.

- **Log Serialization**: All console output is protected by the `print_mutex`. The `print_coder_state()` function acquires this mutex before calling `printf()`, ensuring that state transitions are logged atomically without interleaving from multiple threads.

## Thread synchronization mechanisms
The implementation uses the following threading primitives and coordination strategies:

- **`pthread_mutex_t` Synchronization**: Multiple mutexes protect different shared resources:
  - **`dongle->mutex`**: Protects each dongle's state (in_use flag, available_at timestamp, and waiting_heap queue). Coders lock this mutex while checking if they can acquire the dongle and while modifying its state.
  - **`print_mutex`**: Serializes all console output to prevent interleaved log messages from multiple threads.
  - **`counter_mutex`**: Protects shared coder state (compiles_done count and last_compile_start timestamp), ensuring atomic updates when tracking progress and burnout.
  - **`stop_mutex`**: Protects the global `stop` flag that signals simulation termination. All threads check this flag before making progress, allowing the monitor to cleanly halt the simulation.
  - **`read_write_mutex`**: Provides additional coordination for complex read-write scenarios.

- **`pthread_cond_t` Condition Variables**: Each dongle has an associated condition variable (`dongle->cond`) for efficient event-based notification. When a dongle is released, `pthread_cond_broadcast()` wakes all waiting coders. Coders use `pthread_cond_wait()` for indefinite waits or `pthread_cond_timedwait()` when a cooldown period must expire before retrying.

- **Custom Event Queue (Heap-based Scheduler)**: Instead of relying solely on primitive synchronization, the system implements a fair queueing mechanism. Each dongle maintains a priority heap of pending requests. The FIFO or EDF scheduler determines priority, and `can_take_dongle()` ensures only the head-of-queue coder can proceed. This prevents starvation and enforces fairness at the application level.

- **Race Condition Prevention**: Race conditions are prevented through atomic lock-unlock pairs:
  - When acquiring a dongle, a coder submits a request to the heap, locks the dongle's mutex, waits until it's their turn and the dongle is ready, then marks `dongle->in_use = true` and pops from the heap—all while holding the mutex.
  - When releasing a dongle, the coder locks the mutex, sets `in_use = false`, updates the cooldown timestamp, broadcasts the condition variable, and unlocks—ensuring no other thread can wrongly observe an inconsistent state.
  - State updates (e.g., incrementing `compiles_done` or updating `last_compile_start`) are always protected by holding the counter_mutex throughout the read-modify-write sequence.

- **Thread-Safe Communication Between Coders and Monitor**: The monitor thread runs independently, checking each coder's `last_compile_start` timestamp and `compiles_done` count. These shared fields are protected by `counter_mutex`, ensuring the monitor always reads consistent values. When burnout is detected, the monitor sets `sim->stop = true` (protected by `stop_mutex`). All coder threads check this flag before each state transition, allowing graceful shutdown without race conditions.

- **Example: Dongle Acquisition Flow** (from `acquire_dongle()`):
  1. Lock the dongle's mutex.
  2. Add the coder to the dongle's waiting heap (tracked request).
  3. Loop: check if simulation is stopping (acquire stop_mutex), check if coder is next in queue and dongle is available (via `can_take_dongle()`), if not, wait using `pthread_cond_wait()` or `pthread_cond_timedwait()`.
  4. Once condition is met, set `in_use = true`, pop from the heap, unlock the mutex, and return.
  5. Unlock mutex on any error (simulation stop).
  This ensures no coder can skip the queue, no coder acquires a dongle before its cooldown expires, and all state changes are atomic with respect to the mutex.
