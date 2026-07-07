# Language Reference

Tiny Basic for Arduino is a compact, line-oriented BASIC dialect optimized for resource-constrained microcontrollers. The interpreter supports variables, arithmetic, logical expressions, structured conditionals, loops, and direct hardware manipulation.

## 1. Program Structure & Memory

Programs are structured using numbered lines (e.g., `10 PRINT "HELLO"`) for sequential execution, or entered without line numbers for immediate evaluation in interactive mode.

* **Execution:** Issue the [RUN](run.md) command to begin execution from the lowest stored line number.
* **Line Limits:** Individual lines are limited to **64 characters**.
* **Program Buffer:** Total capacity is hardware-dependent (e.g., an Arduino Due supports a maximum of **1,024 lines**).

## 2. Variables & Data Types

To maintain a minimal memory footprint, the language enforces intentional design constraints on variable storage:

* **Naming:** Exactly 26 single-letter variable names available (`A` through `Z`).
* **Case Insensitivity:** Names are not case-sensitive (`A` and `a` reference the same memory location).
* **Data Types:** Variables dynamically store either integers or floating-point values.
* **Initialization:** All variables are automatically initialized to `0` upon first reference.
* **Assignment:** The [LET](let.md) keyword is **mandatory** for value assignment (e.g., `LET A = 10`). Direct assignment (`A = 10`) is invalid syntax.

* **Arrays & Strings:** The language does **not** support arrays, user-defined functions, or string variables. Strings are only supported as string literals for printing or within simple string-helper functions.

## 3. Operators & Expressions

Expressions are evaluated using numeric values. All comparison operators return `1` for **true** and `0` for **false**.

### Arithmetic Operators

* `+`, `-`, `*`, `/`: Standard arithmetic.
* [MOD](mod.md): Modulus (remainder).
* `( )`: Parentheses for grouping and precedence.

### Comparison Operators

| Operator | Description |
| --- | --- |
| `=` | Equal to |
| `<=` | Less than or equal to |
| `<` | Less than |
| `>=` | Greater than or equal to |
| `>` | Greater than |
| `<>` | Not equal to |

### Logical & Bitwise Operators

* **Logical:** [AND](and.md), [OR](or.md), `!` (logical inverse).
* **Bitwise:** [BAND](band.md), [BOR](bor.md), [XOR](xor.md), [NOT](not.md), [NAND](nand.md), [NOR](nor.md), [EQV](eqv.md), [IMP](imp.md).

## 4. Core Statements & Control Flow

### Input / Output & System Management

* **[PRINT](print.md)**: Outputs expressions, string literals, or formatted numbers (hexadecimal, binary, binary-grouped, or octal using format keywords).
* **[INPUT](input.md)**: Reads integer values from the terminal and stores them in a specified variable.
* **[RUN](run.md)**: Initiates execution of the program currently stored in the buffer.
* **[END](end.md)**: Halts execution (stops the current run in immediate mode; terminates program execution in programming mode).
* **[LIST](list.md)**: Prints the numbered program currently stored in the buffer.
* **[CLEAR](clear.md)**: Resets runtime state (clears variables and stack data) without deleting stored program lines.
* **[NEW](new.md)**: Completely wipes the program buffer and associated memory.
* **[LOAD](load.md)**: Enters interactive entry mode to type or paste code line-by-line.

### Branching & Subroutines

* **[GOTO](goto.md)**: Performs an unconditional jump to the specified line number.
* **[ON](on.md)**: Implements multi-way branching by selecting a target line number based on an evaluated expression.
* **[GOSUB](gosub.md)**: Jumps to a subroutine.
* **[RETURN](return.md)**: Returns execution from a subroutine to the statement immediately following the originating `GOSUB`.

## 5. Conditional Logic

The [IF](if.md) command evaluates a boolean expression to direct program flow. Block forms must be properly terminated with [ENDIF](endif.md); because conditional jump mappings are built during parsing, unclosed blocks will cause structural runtime errors.

```basic
// Single-line syntax[cite: 2]
10 IF A > 0 THEN PRINT "POSITIVE"

// Multi-line block syntax[cite: 2]
10 IF A > 0 THEN
20 PRINT("POSITIVE")
30 ELSE
40 PRINT("NON-POSITIVE")
50 ENDIF

```

## 6. Iteration & Loops

### FOR / NEXT

Iterates using a single-letter control variable. The optional [STEP](step.md) parameter defines custom increments or decrements. The closing [NEXT](next.md) statement must explicitly state the matching loop variable.

```basic
10 FOR A = 1 TO 5 STEP 1
20 PRINT(A)
30 NEXT A

```

### WHILE / WEND

Evaluates the condition at the *start* of the loop. Enclosed statements execute continuously while the condition evaluates to true.

```basic
10 WHILE A < 5
20 PRINT(A)
30 LET A = A + 1
40 WEND

```

### REPEAT / UNTIL

Evaluates the condition at the *end* of the loop, ensuring the body executes at least once. Iteration continues until the [UNTIL](until.md) condition evaluates to true.

```basic
10 REPEAT
20 PRINT(A)
30 LET A = A + 1
40 UNTIL A > 5

```

### EXIT

The [EXIT](exit.md) provides an immediate, clean exit from the innermost enclosing `FOR`, `WHILE`, or `REPEAT` loop, transferring execution directly to the statement following the loop block.

```basic
10 FOR I = 1 TO 10
20   IF I = 5 THEN EXIT
30   PRINT(I)
40 NEXT I
```

## 7. Runtime Architecture & Stack Limits

To ensure stable execution on embedded hardware, the runtime environment enforces strict structural boundaries:

| Structure | Maximum Depth | Triggered Error |
| --- | --- | --- |
| **Subroutines (`GOSUB`)** | 32 nested calls | `GOSUB stack overflow` |
| **Loop Frames** | 16 nested loops | `LOOP stack overflow` |

## 8. Hardware I/O Functions

The dialect includes built-in statements for direct microcontroller hardware and peripheral interaction. Refer to individual command documentation for specific syntax and parameter requirements:

* **General I/O & Timing:** [PIN](pin.md), [SET](set.md), [GET](get.md), [DELAY](delay.md), [WAIT](wait.md), [ANALOG](analog.md), [PWM](pwm.md), [SOUND](sound.md)
* **Protocol & Bus Communication:** [I2C](i2c.md), [SPI](spi.md), [READ](read.md), [WRITE](write.md), [START](start.md), [STOP](stop.md), [RESTART](restart.md), [INIT](init.md)

## 9. Comprehensive Reference Example

The following program demonstrates variable initialization, assignment, loop mechanics, console output, and program termination:

```basic
10 LET A = 1
20 FOR B = 1 TO 3
30 PRINT(A)
40 LET A = A + 1
50 NEXT B
60 END

```