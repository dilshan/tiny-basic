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

!!! tip
    Press the *Escape (Esc)* key at any time to halt execution.

## Step 3: Optimized Version of the Blinking Program

We can significantly reduce the number of lines in the program by replacing the explicit `HIGH` and `LOW` statements with a logical invert trick to toggle a single variable.

Type `NEW` to clear the previous script, and enter the following code in your terminal:

```basic
10 PIN(13, OUT)
20 WHILE TRUE
30   LET A = !A
40   SET(13, A)
50   DELAY(500)
60 WEND

```

!!! info
    In Tiny BASIC, `A = !A` is a logical operator that strictly toggles the value of A between `1` (True) and `0` (False), which is perfect for blinking an LED because hardware pins expect exactly `1` (ON) or `0` (OFF). On the other hand, `A = NOT A` is a bitwise operator that flips every binary bit, causing A to toggle between `0` and `-1`.

### How the Optimization Works

Instead of manually turning the pin on and off over multiple lines, line 30 continually reverses the state of variable `A` (switching it from `TRUE` to `FALSE`). Line 40 feeds this toggled value straight to the pin. Note that we keep the `DELAY(500)` at line 50; without it, the loop would execute too quickly for the human eye to see the flashing!

## See also

- [SET](../manual/set.md)
- [PIN](../manual/pin.md)
- [LET](../manual/let.md)
- [DELAY](../manual/delay.md)
- [WHILE](../manual/while.md)
- [WEND](../manual/wend.md)
- [Tiny Basic Language Reference](../manual/language-reference.md)
- [Arduino Due](../platforms/due.md)
