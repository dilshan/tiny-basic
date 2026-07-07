# Building Tiny Basic for Arduino

This comprehensive guide walks you through setting up your environment, building, and flashing the *Tiny Basic Compiler* project using [PlatformIO](https://platformio.org/).

## Prerequisites & System Requirements

While PlatformIO runs on Windows, macOS, and Linux, **Linux is highly recommended** for building this project.

The project's compilation pipeline utilizes an extra pre-build script (`parser_util.py`) that relies on *Yacc* and *Bison* to generate parser code. These tools are natively supported and easiest to configure in a Linux environment.

Before proceeding, ensure you have the required parser tools installed on your host system:

```bash
# On Debian/Ubuntu-based systems
sudo apt-get update
sudo apt-get install yacc bison build-essential

```

## Step 1: Install PlatformIO

You can manage and build this project either through a graphical interface via VS Code or purely from the command line interface (CLI).

### Option A: Via Visual Studio Code (Recommended Graphical Setup)

1. Download and install [Visual Studio Code](https://code.visualstudio.com/).
2. Open Visual Studio Code, navigate to the *Extensions* tab on the left sidebar (or press `Ctrl+Shift+X`).
3. Search for "PlatformIO IDE" and click *Install*.
4. Once installed, restart Visual Studio Code if prompted. You will see a small *Ant icon* appear on the left activity bar.

### Option B: Via Command Line (Core CLI Setup)

If you prefer working entirely in the terminal, install the Python-based PlatformIO Core:

```bash
# Using pip (requires Python 3.6+)
pip install -U platformio

# Alternative installer script for standalone environments
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py | python3

```

## Step 2: Open and Understand the Project

1. Clone or download [Tiny Basic for Arduino](https://github.com/dilshan/tiny-basic) compiler source code repository.
2. Open the project root folder in Visual Studio Code (`File > Open Folder...`) or navigate to it in your terminal.

The project structure is driven by the configuration in your `platformio.ini` file, which specifies three distinct deployment environments:

| Environment (`env`) | Target Board / Platform | Purpose |
| --- | --- | --- |
| **`due`** *(Default)* | Arduino Due (`atmelsam`) | Microcontroller target with ARM Cortex-M3 architecture. |
| **`uno_r4_wifi`** | Arduino Uno R4 WiFi (`renesas-ra`) | Microcontroller target with Renesas RA4M1 architecture. |
| **`native`** | Host Machine OS (`native`) | Native execution for local debugging and profiling. |

*Note: Every environment automatically invokes the `parser_util.py` Python script prior to compilation to handle Yacc/Bison code generation.*

## Step 3: Build the Project

### Using Visual Studio Code

1. Click the *PlatformIO (Ant)* icon on the sidebar.
2. Under the *Project Tasks* menu, expand the target environment you wish to compile (e.g., `env:due`, `env:uno_r4_wifi`, or `env:native`).
3. Click *Build*.
4. Alternatively, press `Ctrl+Alt+B` (or `Cmd+Option+B` on Mac) to trigger a build for the default environment (`due`).

### Using the CLI

Run the following commands from the root directory of the project:

```bash
# Build the default environment (Arduino Due)
pio run

# Build a specific target environment
pio run -e due
pio run -e uno_r4_wifi
pio run -e native

```

## Step 4: Flash / Upload Firmware

Flashing applies only to the hardware target environments (e.g.: `due` or `uno_r4_wifi`). Ensure your target development board is connected to your computer via USB before executing these steps.

### Using Visual Studio Code

1. Click the *PlatformIO* icon on the sidebar.
2. Expand your active target environment (e.g., `env:due`).
3. Click *Upload*
4. *(Optional)* Click *Monitor* to open the serial terminal and watch the compiler runtime output.

### Using the CLI

Execute the run command with the upload target flag:

```bash
# Upload to the default Arduino Due environment
pio run -t upload

# Upload to a specific environment
pio run -e uno_r4_wifi -t upload

```

PlatformIO will automatically scan your system's `COM/tty` ports, detect the connected board, match it against the required flash tool chain, and upload the compiled binary.

## Running the Native Binary

If you built the project under the `env:native` profile for local testing on your Linux host machine, you do not flash it. Instead, you can find and run the generated executable inside the local build directory:

```bash
# Run the compiled native executable
./.pio/build/native/program

```

or, execute the `tinybasic.sh` file located at the root directory of the project repository. 
