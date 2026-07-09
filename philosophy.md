# Project Philosophy

## Minimalism and Constraints

The *Tiny Basic for Arduino* project is rooted in the philosophy of radical minimalism. The core mission is to provide a lightweight BASIC interpreter optimized to run on popular, resource-constrained hardware platforms like the Arduino. This project primarily targets a younger audience and hobbyists who are just starting to explore the integration of electronics and software. We believe that simplicity, paired with low-cost hardware, lowers the barrier to entry-allowing a much wider audience to explore computing, experiment freely, and see immediate, tangible results.

## Core Architecture Goals

* **Zero External Dependencies:** The core interpreter engine must compile cleanly using standard toolchains without relying on third-party libraries, ensuring an identical experience across environments.

* **Predictable Execution:** Memory allocation must be static or highly bounded. Avoid dynamic allocations (`malloc`/`new`) in the execution loop to guarantee stable, predictable performance on 8-bit microcontrollers and embedded systems.

* **Portability First:** The architecture abstracts hardware-specific I/O (such as serial terminal communication, display outputs, and storage) into simple, interchangeable Hardware Abstraction Layer (HAL) hooks.

* **Designed as an Educational Tool:** The language structure and internal codebase must remain as simple as possible so a broad audience can understand the system with minimal friction. Because this is fundamentally an educational support tool, overly complex operators or convoluted grammar rules are intentionally excluded.

## Syntax and Compliance

We adhere closely to the classic 1975 Li-Chen Wang syntax specification, featuring standard commands like `PRINT`, `INPUT`, `LET`, `GOTO`, `GOSUB`, `RETURN`, `IF`, `CLEAR`, `LIST`, and `RUN`. Extensions to the core language should only be introduced if they map directly to physical hardware control (e.g., `I2C`, `SPI`, digital/analog pin manipulation) and must be strictly modularized.