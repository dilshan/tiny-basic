# Writing Your First "Hello World" Program

This guide will help you write and run your first "Hello World" program using Tiny Basic on the Arduino Due platform. It assumes that Tiny Basic for Arduino is already [flashed onto the Arduino Due board](building.md).

## Step 1: Connect the Arduino Due to Your Computer

1. Connect the Arduino Due:
    
    Use a USB cable to connect the *Programming Port* (labeled "PROGRAMMING" on the Arduino Due) to your computer. Ensure the cable supports both power and data transfer. The Programming Port is the USB port closest to the DC power jack.

2. Identify the Serial Port:
    
    On Linux, open a terminal and run the following command to list connected devices:
     ```bash
     ls /dev/tty*
     ```
    Look for a device like `/dev/ttyACM0` or `/dev/ttyUSB0`. This is your Arduino Due's serial port.

## Step 2: Access the Serial Interface

1. Open a Serial Terminal:
    Use a terminal emulator like `minicom`, `screen`, or `picocom`. For example, to use `screen`, run:
     ```bash
     screen /dev/ttyACM0 9600
     ```
    Replace `/dev/ttyACM0` with the serial port identified earlier.

2. Configure the Serial Terminal:
    * Set the baud rate to `9600`.
    * Disable hardware flow control if prompted.

3. Verify the Connection:
   
     Press `Enter` in the terminal. You should see the Tiny Basic prompt (`>`).

## Step 3: Write the "Hello World" Program

1. Enter the Program:
    Type the following Tiny Basic code into the terminal:
     ```basic
     10 PRINT("HELLO, WORLD!")
     20 END
     ```

## Step 4: Run the Program

1. Execute the Program; at the Tiny Basic prompt, type:
    ```basic
    RUN
    ```
    You should see the output:
    ```
    HELLO, WORLD!
    ```

2. Modify or Debug: 

    * Use the `LIST` command to view the program:
     ```basic
     LIST
     ```

    * Edit lines by re-entering them with the same line number.

Congratulations! You've successfully written and executed your first Tiny Basic program on the Arduino Due.

## Additional Resources

- [Tiny Basic Language Reference](../manual/language-reference.md)
- [Tiny Basic Editor](../manual/editor.md)
- [Arduino Due Setup Guide](https://docs.arduino.cc/hardware/due)
- [Serial Terminal Configuration](https://linux.die.net/man/1/screen)