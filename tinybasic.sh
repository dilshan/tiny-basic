#!/bin/bash

PROGRAM_PATH="./.pio/build/native/program"

if [ ! -f "$PROGRAM_PATH" ]; then
    echo "Error: Compiled executable not found."
    echo "Please compile the project first using PlatformIO:"
    echo "  pio run -e native"
    exit 1
fi

"$PROGRAM_PATH" "$@"