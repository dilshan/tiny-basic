# The Tiny Basic for Arduino In-Hardware Editor

The Tiny Basic for Arduino operates as an in-hardware editor, eliminating the need for a dedicated integrated development environment (IDE). You can enter, view, and execute programs directly through a standard serial terminal.

## 1. Terminal Connection Setup

To connect to the system, use a serial terminal emulation program such as **minicom** (Linux), **PuTTY** (Windows), or **CoolTerm** (macOS).

Configure your terminal connection with the following parameters:

| Parameter | Value | Description |
| --- | --- | --- |
| **Baud Rate** | `9600` | Communication speed
| **Data Bits** | `8` | Standard data word length
| **Parity** | `None` (`N`) | No parity bit
| **Stop Bits** | `1` | Single stop bit

Once successfully connected, the system will display the `>` prompt, indicating it is ready to receive commands.

## 2. Operating Modes

The interpreter operates in two distinct modes depending on how commands are entered into the prompt.

### Immediate Mode

In **Immediate Mode**, instructions are entered without line numbers. The system executes the command as soon as you press **Enter**. This mode is ideal for testing functions, evaluating expressions, or managing workspace memory without permanently saving code.

**Example:**

```basic
> PRINT("Hello World")
Hello World

```

*[The system immediately evaluates the instruction and prints the output.]*

#### Workspace Management Commands

Use the following immediate mode commands to manage your environment:

* **`LIST`**: Displays the current program stored in memory.
* **`RUN`**: Executes the program currently stored in memory.
* **`NEW`**: Deletes the current program and clears the program memory.
* **`CLEAR`**: Resets all variables and loop counters while preserving the stored program code.

### Programming Mode

**Programming Mode** is triggered by prefixing any instruction with a line number. Instead of executing immediately, the system stores the line in RAM. Tiny Basic for Arduino automatically organizes stored instructions in sequential numerical order.

## 3. Line Editing & Program Management

Because lines are stored sequentially by number, editing programs in memory is straightforward. It is recommended to use line increments of 10 (e.g., 10, 20, 30) when initially writing code to leave space for future insertions.

### Inserting Code

To add a new step between existing instructions, enter a new instruction using a line number that falls between the existing lines. The system automatically reorders the program.

* **Existing Program:**
```basic
10 LET A = 0
20 LET A = A + 10
30 PRINT(A)

```

* **Action:** Type `25 LET A = 5` and press **Enter**.

* **Result:**
```basic
10 LET A = 0
20 LET A = A + 10
25 LET A = 5
30 PRINT(A)

```

### Replacing Code

To modify an existing instruction, enter the target line number followed by the new command. This completely overwrites the previous entry stored at that line number.

* **Action:** Type `20 LET A = A + 50` and press **Enter**.

* **Result:**
```basic
10 LET A = 0
20 LET A = A + 50
30 PRINT(A)

```

### Deleting Lines

To remove a line from memory, enter the target line number followed immediately by pressing **Enter** (a blank line).

* **Action:** Type `20` and press **Enter**.

* **Result:**
```basic
10 LET A = 0
30 PRINT(A)

```

## 4. Bulk Loading Mode & Program Storage

Tiny Basic for Arduino stores programs entirely in system RAM. By avoiding onboard flash or EEPROM for storage, the interpreter ensures consistent behavior across different microcontrollers without write-cycle limitations, storage size discrepancies, or wear-leveling concerns.

Because RAM is volatile and typing lengthy programs over a serial stream can be tedious, **manually typing long programs directly into the terminal is not recommended**.

### Recommended Workflow for Large Programs:

1. Write and save your BASIC program using an external text editor on your host computer.
2. In the terminal, enter the [LOAD](https://www.google.com/search?q=load.md) command.
3. Paste your complete code directly into the terminal window.
4. To backup or retrieve your modified code from the hardware, issue the [LIST](list.md) command. Copy the output printed in the terminal and save it back to your host computer.

## 5. Breaking Infinite Loops

When testing code, execution may occasionally get trapped in an infinite loop where the program runs indefinitely.

Tiny Basic for Arduino includes a built-in hardware safety interrupt to recover from runaway programs. If your program becomes unresponsive or enters an infinite loop:

1. Press the **Esc** key on your keyboard.
2. The environment will immediately detect the interrupt, break out of the execution cycle safely, and return control to the command prompt (`>`).