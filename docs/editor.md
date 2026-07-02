# The Tiny Basic In-Hardware Editor

The Tiny Basic system for Arduino is designed for simplicity, removing the need for a dedicated IDE. It functions as an in-hardware editor, allowing you to enter, view, and execute programs directly through a serial terminal. To connect, use software like *minicom* (Linux), *PuTTY* (Windows), or *CoolTerm* (macOS) with settings configured to `8N1` (8 data bits, no parity, 1 stop bit) at `9600 baud`. Once connected, the `>` prompt indicates the system is ready.

## Immediate Mode

In *Immediate Mode*, the system executes commands as soon as you press Enter. This is useful for testing functions or managing the workspace without saving code to memory.

**Example:** To test a function, type the following and press Enter:
`> PRINT("Hello World")`
*The system immediately responds: `Hello World`*

Other management commands include:

- `LIST`: Displays the current program stored in memory.
- `RUN`: Executes the program currently in memory.
- `NEW`: Deletes the current program and clears memory.
- `CLEAR`: Resets variables and loop counters while keeping the program code intact.

## Programming Mode and Line Editing

*Programming Mode* is triggered by prefixing your instructions with a line number. Tiny Basic stores these lines in numerical order, which makes inserting or replacing code highly intuitive.

### Inserting Code

If you have a program and realize you need to add a step, simply use a line number that falls between your existing lines.

**Original Program:**
```basic
10 LET A = 0
20 LET A = A + 10
30 PRINT(A)
```
**Action:** Type `25 LET A = 5`

**Result:** The system automatically reorders the lines:
```basic
10 LET A = 0
20 LET A = A + 10
25 LET A = 5
30 PRINT(A)
```

### Replacing Code

To modify an instruction, enter the existing line number followed by the new command. The system will automatically overwrite the previous entry.

**Original Program:**
```basic
10 LET A = 0
20 LET A = A + 10
30 PRINT(A)
```
**Action:** Type `20 LET A = A + 50`

**Result:** Line 20 is updated. The program now executes as:
```basic
10 LET A = 0
20 LET A = A + 50
30 PRINT(A)
```

## Deleting Lines

To delete a line, enter the line number followed by a blank line.

**Original Program:**
```basic
10 LET A = 0
20 LET A = A + 10
30 PRINT(A)
```
**Action:** Type `20` and press Enter.

**Result:** Line 20 is deleted. The program now executes as:
```basic
10 LET A = 0
30 PRINT(A)
```

## Recover from infinite loops

If the program enters an infinite loop, you can break out of it by pressing **Esc** key.