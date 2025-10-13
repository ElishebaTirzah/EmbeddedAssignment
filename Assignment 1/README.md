### Assignment 1 — FreeRTOS Queue and Task Priority Demo

This assignment demonstrates inter-task communication with a FreeRTOS queue and dynamic task priority management on an STM32 HAL-based project.

- **Producer task (`ExampleTask1`)**: Sends a `Data_t` struct to a queue every 500 ms with `dataID = 1` and `DataValue` that cycles 0 → 4.
- **Consumer task (`ExampleTask2`)**: Receives items from the queue, prints them, and reacts to values:
  - **DataValue = 0**: temporarily increases its own priority by +2.
  - **DataValue = 1**: restores priority back down by 1 if previously increased.
  - **DataValue = 2**: self-deletes.
  - If `dataID == 0`: self-deletes immediately.

#### Project Highlights
- Uses a FreeRTOS queue (`Queue1`) of length 5 with element type `Data_t`.
- Starts two tasks and the scheduler; asserts if creation fails.
- Prints received items using `printf` (route to SWO/ITM or UART as configured in your BSP).

#### Build and Flash
Use whichever setup you generated the project with (e.g., STM32CubeIDE or Makefiles). Typical STM32CubeIDE flow:
1. Open the workspace in STM32CubeIDE.
2. Build the project (Project → Build or Ctrl+B).
3. Connect your board via ST-Link.
4. Run/Debug to flash (green Run/Debug button).

If you use a UART for `printf`, open a serial terminal at the configured baud rate; for SWO, open the SWV ITM Console.

#### Expected Output
You should see lines like the following every 500 ms while `ExampleTask2` is active:

```
ID: 1, Data: 0
ID: 1, Data: 1
ID: 1, Data: 2
ID: 1, Data: 3
ID: 1, Data: 4
```

Behavior notes:
- On `DataValue = 0`, `ExampleTask2` boosts its priority by +2.
- On `DataValue = 1`, it reduces priority by 1 if it was previously boosted.
- On `DataValue = 2`, `ExampleTask2` calls `vTaskDelete(NULL)` and stops printing thereafter.

#### Configuration Tips
- Ensure `heap` size in `FreeRTOSConfig.h` is sufficient; scheduler start failures often indicate low heap.
- Verify your `printf` backend (retarget to UART or enable SWO/ITM in your debug config).
- Adjust task stack sizes (currently 128 words) and priorities as needed for your target MCU.

#### File Map (excerpt)
- `main.c`: Task creation, queue setup, scheduler start, and task implementations.

#### License
STMicroelectronics HAL license applies as noted in file headers; this assignment code is provided as-is.