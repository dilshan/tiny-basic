# Language reference

Tiny Basic for Arduino is a small, line-oriented BASIC dialect meant for microcontrollers. Programs are written as numbered lines or entered directly in immediate mode, and the interpreter supports variables, arithmetic, comparisons, loops, conditionals, and simple hardware commands.

This page summarizes the language as implemented by the interpreter in the project sources.

## Program structure

In this programming environment, code is structured using numbered lines, such as `10 PRINT "HELLO"`, which allows for organized, sequential execution. While these numbered lines are used to build structured programs, users also have the flexibility to enter commands without numbers for immediate execution in interactive mode.

When you are ready to process a stored sequence, you simply initiate the [RUN](run.md) command, which triggers the interpreter to execute the program from the first line. The capacity of this system is defined by a program buffer that varies based on the hardware platform; for instance, on an Arduino Due, the interpreter supports a maximum of 1,024 lines, with each individual line limited to 64 characters.

## Variables and values

Data management is similarly streamlined, with variables identified by single-letter names ranging from `A` to `Z`. Please note that these variable names are **not case-sensitive**; for instance, `A` and `a` refer to the same variable. This allows for a total of 26 distinct variables, which are capable of storing either integer or floating-point values. When you first reference a variable, it is automatically initialized to a value of zero. To assign data to these variables, you use the [LET](let.md) command, which can be applied explicitly - for example, `LET A = 10` - to store a value for later use in your program.

It is important to note that these specific constraints, such as the single-letter variable limit and the fixed line lengths, are intentionally implemented. By keeping the interpreter’s memory footprint and processing requirements minimal, the design ensures that this language can run efficiently on as many different microcontrollers as possible.

## Expressions and comparisons

The interpreter supports arithmetic, comparisons, bitwise and logical expressions.

### Arithmetic operators

- `+`, `-`, `*`, `/`
- [MOD](mod.md)
- Parentheses may be used for grouping.

### Comparison operators

- `=` equal
- `<` less than
- `>` greater than
- `<=` less than or equal
- `>=` greater than or equal
- `<>` not equal

### Logical operators

- [AND](and.md), [OR](or.md)
- `!` (logical inverse)

### Bitwise operators

- [BAND](band.md)
- [BOR](bor.md)
- [EQV](eqv.md)
- [IMP](imp.md)
- [NAND](nand.md)
- [NOR](nor.md)
- [NOT](not.md)
- [XOR](xor.md)

Expressions are evaluated using numeric values, and comparisons return `0` for false and `1` for true.

## Statements

### Output and input

- [PRINT](print.md) prints expressions and quoted strings.
- [INPUT](input.md) reads integer values from the user and stores them in variables.
- [PRINT](print.md) can also display numbers in hexadecimal, binary, binary-grouped, or octal formats when used with the corresponding format keywords.

### Assignment

- `LET A = 10`
- `A = 10` is not valid syntax; the interpreter expects [LET](let.md) for assignment.

### Program control

To manage the execution and organization of your program, you can utilize a set of fundamental commands. Below is an overview of how to control your program flow and memory state:

#### Program Execution and Control

* [RUN](run.md) is used to initiate the execution of a stored program.
* [END](end.md) halts execution. Its behavior depends on your current environment: in immediate mode, it stops the current run, while in program mode, it brings the execution of the program to a complete stop.

#### Memory and Display Management

* [LIST](list.md) allows you to review your work by displaying the stored, numbered program currently in the buffer.
* [CLEAR](clear.md) is useful for resetting the runtime state—such as clearing variables or stack data—without deleting the actual lines of your program.
* [NEW](new.md) performs a comprehensive reset by clearing the program buffer entirely and wiping the associated memory, effectively providing a blank slate for new development.

### Jumping

To control the flow of execution within your programs, you can use specialized branching and subroutine commands.

### Control Flow and Branching

You can use the [GOTO](goto.md) command to perform an unconditional jump to a specific numbered line, altering the linear path of the code. For more complex logic, the [ON](on.md) command allows you to select a specific target line number based on the evaluated result of an expression, providing a convenient way to implement multi-way branching.

### Subroutine Management

For modularizing your code, [GOSUB](gosub.md) is used to call a subroutine, while the [RETURN](return.md) command ensures that execution transfers back to the statement immediately following the original call. Please note that the system supports a maximum of 32 nested [GOSUB](gosub.md) calls; exceeding this limit on the call stack will result in an error.

## Conditions

The language provides flexible support for conditional logic through `IF` statements, which can be utilized in either a compact single-line format or a more robust block structure. At its core, the [IF](if.md) command evaluates a boolean expression to determine the path of execution, with the [THEN](then.md) command serving to define the beginning of the statement's body.

For more complex logic, you can utilize multi-line blocks incorporating [ELSE](else.md) and [ENDIF](endif.md). It is important to ensure these blocks are properly terminated; the interpreter proactively builds conditional jump mappings while parsing the program, meaning that any failure to correctly close a block will result in structural errors.

### Single-line form

```basic
10 IF A > 0 THEN PRINT "POSITIVE"
```

### Block form

```basic
10 IF A > 0 THEN
20 PRINT("POSITIVE")
30 ELSE
40 PRINT("NON-POSITIVE")
50 ENDIF
```

## Loops

### FOR / NEXT

To manage repetitive tasks and iteration, the language uses the [FOR](for.md) command, which relies on a single-letter loop variable to track progress. When the loop initializes, this variable is set to your specified starting value. The loop will continue to execute as long as the variable's value remains within the defined limit.

You can further control the iteration process by using the [STEP](step.md) command, which allows you to define custom increments or decrements for the loop variable, providing flexibility beyond standard unit-based counting. Finally, the [NEXT](next.md) command is required to close the loop; it is critical that this command explicitly matches the variable used in the corresponding [FOR](for.md) statement to ensure the interpreter tracks the nesting structure correctly.

```basic
10 FOR A = 1 TO 5
20 PRINT(A)
30 NEXT A
```

### WHILE / WEND

For scenarios where you need to repeat a sequence of instructions based on a logical state, you can use the [WHILE](while.md) command. This command evaluates a condition at the start of the loop and continues to execute the enclosed statements as long as that condition remains **true**.

The loop body is clearly defined by the [WEND](wend.md) command, which marks the termination of the block and signals the interpreter to return to the [WHILE](while.md) statement to re-evaluate the condition. This structure is ideal for situations where the number of iterations is not known in advance and depends entirely on the dynamic state of your program.

```basic
10 WHILE A < 5
20 PRINT(A)
30 LET A = A + 1
40 WEND
```

### REPEAT / UNTIL

To handle iterations where you need the loop body to execute at least once regardless of the initial condition, you can use the [REPEAT](repeat.md) command. This statement serves as the entry point for a loop that continues to run until the condition specified by the [UNTIL](until.md) command evaluates to **true**. Unlike structures that check conditions at the start, this approach ensures that your code block always performs its task at least one time before checking if it should terminate.

```basic
10 REPEAT
20 PRINT(A)
30 LET A = A + 1
40 UNTIL A > 5
```

### EXIT

When you need to terminate an iteration prematurely, the [EXIT](exit.md) command provides a clean way to leave the innermost enclosing loop. This is particularly useful for breaking out of [FOR](for.md), [WHILE](while.md), or [REPEAT](repeat.md) structures before the standard termination condition has been met.

By using this command, you can efficiently exit a loop as soon as a specific condition is satisfied or an error is encountered, allowing your program to skip remaining iterations and proceed directly to the code immediately following the loop block.

## Loop and stack limitations

The runtime environment is designed around fixed-size internal structures that manage the state of your programs. Specifically, it maintains a limit of 32 for the maximum nested `GOSUB` depth and a capacity of 16 for nested loop frames.

These constraints are important to keep in mind during development, as exceeding them will trigger specific runtime errors. If your program attempts to nest too many subroutines, the interpreter will report a "*GOSUB stack overflow*" while surpassing the capacity for nested loops will result in a "*LOOP stack overflow*". Ensuring your code adheres to these boundaries is essential for stable execution.

## Hardware-related functions

Tiny Basic for Arduino also includes functions for interacting with the hardware environment:

- [DELAY](delay.md), [PIN](pin.md), [SET](set.md), [WAIT](wait.md), [GET](get.md), [ANALOG](analog.md), [SOUND](sound.md), [PWM](pwm.md)
- [I2C](i2c.md), [SPI](spi.md), [READ](read.md), [WRITE](write.md), [START](start.md), [STOP](stop.md), [RESTART](restart.md), [INIT](init.md)

These are implemented as built-in statements and are documented in the command-specific pages.

## Notes and limitations

This programming language is **intentionally compact and optimized**, specifically designed for small embedded applications rather than the complexities of full-scale desktop BASIC. Because of this focus on efficiency, the language does not include support for arrays or user-defined functions. Furthermore, while strings can be used as literals for printing and within simple string-based helper functions, **they are not implemented as full string variables**, reflecting the language's stripped-down architecture.

## Example

```basic
10 LET A = 1
20 FOR B = 1 TO 3
30 PRINT(A)
40 LET A = A + 1
50 NEXT B
60 END
```

This example shows simple assignment, a `FOR` loop, printing, and termination.