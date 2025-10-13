### Assignment 1 — FreeRTOS Queue and Task Priority Demo

This assignment demonstrates inter-task communication with a FreeRTOS queue and dynamic task priority management on an STM32 HAL-based project.

- **Producer task (`ExampleTask1`)**: Sends a `Data_t` struct to a queue every 500 ms with `dataID = 1` and `DataValue` that cycles 0 → 4.

- **Consumer task (`ExampleTask2`)**: Receives items from the queue, prints them, and reacts to values:
  - **DataValue = 0**: temporarily increases its own priority by +2.
  - **DataValue = 1**: restores priority back down by 1 if previously increased.
  - **DataValue = 2**: self-deletes.
  - If `dataID == 0`: self-deletes immediately.

#### Assignment Highlights
- Uses a FreeRTOS queue of length 5 with element type Data_t.
- Starts two tasks.
- Prints received items using printf (route to ITM and sends via SWO -> STLink -> USB -> PC).
 - Target MCU used to ensure the code is error free: STM32L433RCTP, MSI clock configured to 4 MHz (RCC_MSIRANGE_6).


#### Expected Output
You should see lines like the following every 500 ms while `ExampleTask2` is active:

```
ID: 1, Data: 0
ID: 1, Data: 1
ID: 1, Data: 2
```

Behavior notes:
- On `DataValue = 0`, `ExampleTask2` boosts its priority by +2.
- On `DataValue = 1`, it reduces priority by 1 if it was previously boosted.
- On `DataValue = 2`, `ExampleTask2` calls `vTaskDelete(NULL)` and stops printing thereafter.


#### Assignment 1 Preview

![Embedded Assignment Overview](Assignment%201/EmbeddedAssignment.jpeg](/Assignment%201/EmbeddedAssignment.jpeg)

