// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

/**
 * @brief Defines the runtime error and warning message strings used by Tiny Basic.
 * @details This file centralizes the human-readable diagnostics emitted by the
 *          parser and runtime components, making error reporting consistent and
 *          easy to maintain across the project.
 */

#include "error.h"

const char ERR_TOO_MANY_LOOP_BLOCKS[]       = "Error: Too many loop blocks\r\n";
const char ERR_REQUIRED_INT[]               = "Error: Required integer type\r\n";
const char ERR_LOOP_STACK_OVERFLOW[]        = "Error: LOOP stack overflow\r\n";
const char ERR_STEP_CANNOT_ZERO[]           = "Error: STEP cannot be zero\r\n";
const char ERR_MISSING_NEXT[]               = "Error: Missing matching NEXT\r\n";
const char ERR_MISSING_UNTIL[]              = "Error: Missing UNTIL\r\n";
const char ERR_MISSING_WHILE[]              = "Error: WEND without WHILE\r\n";
const char ERR_GOSUB_STACK_OVERFLOW[]       = "Error: GOSUB stack overflow\r\n";
const char ERR_MISSING_GOSUB[]              = "Error: RETURN without GOSUB\r\n";
const char ERR_MISSING_ENDIF[]              = "Error: Missing ENDIF\r\n";
const char ERR_MISSING_FOR[]                = "Error: NEXT without matching FOR\r\n";
const char ERR_MISMATCH_NEXT_VAR[]          = "Error: Mismatched NEXT variable\r\n";
const char ERR_MISSING_WEND[]               = "Error: Missing WEND\r\n";
const char ERR_MISSING_REPEAT[]             = "Error: Missing REPEAT\r\n";
const char ERR_EXIT_WITHOUT_LOOP[]          = "Error: EXIT without a loop\r\n";
const char ERR_ENDIF_WITHOUT_IF[]           = "Error: ENDIF without IF\r\n";
const char ERR_DIVISION_BY_ZERO[]           = "Error: Division by zero\r\n";
const char ERR_MISSING_CONDITION_BOUNDRY[]  = "Error: Missing conditional boundary\r\n";
const char ERR_MISSING_TERMINATION[]        = "Error: Missing termination block for line %d\r\n";
const char ERR_UNDEFINED_LINE[]             = "Error: Undefined line %d\r\n";
const char ERR_UNKNOWN_CHAR[]               = "Error: Unknown character '%s'\r\n";
const char ERR_PWM_OUTOF_RANGE[]            = "Error: PWM value %d is out of range\r\n";
const char ERR_INVALID_ANALOG_PIN[]         = "Error: Invalid analog pin: A%d\r\n";
const char ERR_INVALID_PIN[]                = "Error: Invalid pin: %d\r\n";
const char ERR_INVALID_PIN_MODE[]           = "Error: Invalid mode for pin %d\r\n";
const char ERR_INVALID_LINE_NUM[]           = "Error: Invalid line number\r\n";

const char WARN_I2C_RECEIVED_NACK[]         = "Warning: Received NACK from I2C\r\n";