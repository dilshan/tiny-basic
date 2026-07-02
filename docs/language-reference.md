# Language reference

Tiny Basic for Arduino is a small, line-oriented BASIC dialect meant for microcontrollers. Programs are written as numbered lines or entered directly in immediate mode, and the interpreter supports variables, arithmetic, comparisons, loops, conditionals, and simple hardware commands.

This page summarizes the language as implemented by the interpreter in the project sources.

## Program structure

In this programming environment, code is structured using numbered lines, such as `10 PRINT "HELLO"`, which allows for organized, sequential execution. While these numbered lines are used to build structured programs, users also have the flexibility to enter commands without numbers for immediate execution in interactive mode.

When you are ready to process a stored sequence, you simply initiate the [RUN](run.md) command, which triggers the interpreter to execute the program from the first line. The capacity of this system is defined by a program buffer that varies based on the hardware platform; for instance, on an Arduino Due, the interpreter supports a maximum of 1,024 lines, with each individual line limited to 64 characters.

## Variables and values

Data management is similarly streamlined, with variables identified by single-letter names ranging from `A` to `Z`. This allows for a total of 26 distinct variables, which are capable of storing either integer or floating-point values. When you first reference a variable, it is automatically initialized to a value of zero. To assign data to these variables, you use the [LET](let.md) command, which can be applied explicitly—for example, `LET A = 10` - to store a value for later use in your program.

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

- [LET](let.md) `A = 10`
- `A = 10` is not used in the documented syntax; the interpreter expects [LET](let.md) for assignment.

### Program control

- [RUN](run.md) starts execution of the stored program.
- [END](end.md) stops execution. In immediate mode it ends the current run; in program mode it stops the program.
- [LIST](list.md) displays the stored numbered program.
- [CLEAR](clear.md) resets runtime state without deleting the program buffer.
- [NEW](new.md) clears the program and resets memory.

### Jumping

- [GOTO](goto.md) jumps to a numbered line.
- [GOSUB](gosub.md) calls a subroutine and [RETURN](return.md) returns to the caller.
- The call stack is limited to 32 nested [GOSUB](gosub.md) calls.
- [ON](on.md) selects one of the given line numbers based on the expression value.

## Conditions

The language supports `IF` statements in both single-line and block form.

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

Notes:

- [IF](if.md) tests a boolean expression.
- [THEN](then.md) begins the body of the statement.
- [ELSE](else.md) and [ENDIF](endif.md) are used for multi-line blocks.
- The interpreter builds conditional jump mappings while parsing the program, so blocks must be properly terminated.

## Loops

### FOR / NEXT

```basic
10 FOR A = 1 TO 5
20 PRINT(A)
30 NEXT A
```

- [FOR](for.md) uses a single-letter loop variable.
- The loop variable is initialized to the starting value.
- The loop runs while the value stays within the limit.
- [STEP](step.md) can be used for custom increments or decrements.
- [NEXT](next.md) must match the corresponding [FOR](for.md) variable.

### WHILE / WEND

```basic
10 WHILE A < 5
20 PRINT(A)
30 LET A = A + 1
40 WEND
```

- [WHILE](while.md) repeats while the condition remains true.
- [WEND](wend.md) marks the end of the loop body.

### REPEAT / UNTIL

```basic
10 REPEAT
20 PRINT(A)
30 LET A = A + 1
40 UNTIL A > 5
```

- [REPEAT](repeat.md) starts a loop that continues until the condition in [UNTIL](until.md) becomes true.

### EXIT

- [EXIT](exit.md) leaves the innermost enclosing loop.
- It is useful for breaking out of [FOR](for.md), [WHILE](while.md), or [REPEAT](repeat.md) structures early.

## Loop and stack limitations

The runtime uses fixed-size internal structures:

- Maximum nested `GOSUB` depth: 32
- Maximum nested loop frames: 16
- Maximum program lines: 256
- Maximum line length: 256 characters

If these limits are exceeded, the interpreter reports errors such as `GOSUB stack overflow` or `LOOP stack overflow`.

## Hardware-related functions

Tiny Basic for Arduino also includes functions for interacting with the hardware environment:

- [DELAY](delay.md), [PIN](pin.md), [SET](set.md), [WAIT](wait.md), [GET](get.md), [ANALOG](analog.md)
- [I2C](i2c.md), [SPI](spi.md), [READ](read.md), [WRITE](write.md), [START](start.md), [STOP](stop.md), [RESTART](restart.md), [INIT](init.md)

These are implemented as built-in statements and are documented in the command-specific pages.

## Notes and limitations

- There is no support for arrays or user-defined functions.
- Strings are only used as literals for printing and simple string-based helpers; **they are not full string variables**.
- The language is **intentionally compact and optimized** for small embedded programs rather than full-scale desktop BASIC.

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