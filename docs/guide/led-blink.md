# Blinking an LED with Tiny Basic

This tutorial demonstrates how to blink an on-board LED using Tiny Basic on the Arduino Due.

## Step 1: Write the Program

Enter the following code in the Tiny Basic terminal:

```basic
10 PIN(13, OUT)
20 WHILE TRUE
30   SET(13, HIGH)
40   DELAY(500)
50   SET(13, LOW)
60   DELAY(500)
70 WEND
```

## Step 2: Run the Program

Type `RUN` in the Tiny Basic terminal to execute the program.

Press Escape (Esc) key to halt the program.

## Step 3: Improved Version of LED Blinking Program

The number of lines in above program can reduce using butwise operaton based logic.

Type `New` and enter the following code in the Tiny Basic terminal:

```basic
10 PIN(13, OUT)
20 WHILE TRUE
30   LET A = NOT A
40   SET(13, A)
50 WEND
```
