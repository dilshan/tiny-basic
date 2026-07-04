// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#include "error.h"

const char ERR_TOO_MANY_LOOP_BLOCKS[]       = "Error: Too many loop blocks\n";
const char ERR_REQUIRED_INT[]               = "Error: Required integer type\n";
const char ERR_LOOP_STACK_OVERFLOW[]        = "Error: LOOP stack overflow\n";
const char ERR_STEP_CANNOT_ZERO[]           = "Error: STEP cannot be zero\n";
const char ERR_MISSING_NEXT[]               = "Error: Missing matching NEXT\n";
const char ERR_MISSING_UNTIL[]              = "Error: Missing UNTIL\n";
const char ERR_MISSING_WHILE[]              = "Error: WEND without WHILE\n";
const char ERR_GOSUB_STACK_OVERFLOW[]       = "Error: GOSUB stack overflow\n";
const char ERR_MISSING_GOSUB[]              = "Error: RETURN without GOSUB\n";
const char ERR_MISSING_ENDIF[]              = "Error: Missing ENDIF\n";
const char ERR_MISSING_FOR[]                = "Error: NEXT without matching FOR\n";
const char ERR_MISMATCH_NEXT_VAR[]          = "Error: Mismatched NEXT variable\n";
const char ERR_MISSING_WEND[]               = "Error: Missing WEND\n";
const char ERR_MISSING_REPEAT[]             = "Error: Missing REPEAT\n";
const char ERR_EXIT_WITHOUT_LOOP[]          = "Error: EXIT without a loop\n";
const char ERR_ENDIF_WITHOUT_IF[]           = "Error: ENDIF without IF\n";
const char ERR_DIVISION_BY_ZERO[]           = "Error: Division by zero\n";
const char ERR_MISSING_CONDITION_BOUNDRY[]  = "Error: Missing conditional boundary\n";
const char ERR_MISSING_TERMINATION[]        = "Error: Missing termination block for line %d\n";
const char ERR_UNDEFINED_LINE[]             = "Error: Undefined line %d\n";
const char ERR_UNKNOWN_CHAR[]               = "Error: Unknown character '%s'\n";
const char ERR_PWM_OUTOF_RANGE[]            = "Error: PWM value %d is out of range\n";
const char ERR_INVALID_ANALOG_PIN[]         = "Error: Invalid analog pin: A%d\n";
const char ERR_INVALID_PIN[]                = "Error: Invalid pin: %d\n";
const char ERR_INVALID_PIN_MODE[]           = "Error: Invalid mode for pin %d\n";

const char WARN_I2C_RECEIVED_NACK[]         = "Warning: Received NACK from I2C\n";