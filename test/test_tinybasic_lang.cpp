#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include <unity.h>

extern "C"
{
#include "platform.h"
}

static char g_output[4096];
static size_t g_output_len = 0;

static void reset_output(void)
{
    g_output[0] = '\0';
    g_output_len = 0;
}

static int capture_print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(g_output + g_output_len,
                            sizeof(g_output) - g_output_len,
                            fmt,
                            args);
    va_end(args);

    if (written < 0)
    {
        return 0;
    }

    if ((size_t)written >= sizeof(g_output) - g_output_len)
    {
        written = (int)(sizeof(g_output) - g_output_len - 1);
    }

    g_output_len += (size_t)written;
    return written;
}

static void capture_float_print(double value)
{
    (void)capture_print("%g", value);
}

static void no_op(void)
{
}

static int no_op_int(void)
{
    return 0;
}

static void no_op_pin(int pin, int mode)
{
    (void)pin;
    (void)mode;
}

static void no_op_digital_write(int pin, int value)
{
    (void)pin;
    (void)value;
}

static int no_op_digital_read(int pin)
{
    (void)pin;
    return 0;
}

static void no_op_pwm(int pin, unsigned char value)
{
    (void)pin;
    (void)value;
}

static void no_op_tone(int freq, int duration)
{
    (void)freq;
    (void)duration;
}

static void no_op_delay(int ms)
{
    (void)ms;
}

static int no_op_analog(int pin)
{
    (void)pin;
    return 0;
}

static unsigned char no_op_i2c_write(unsigned char data)
{
    (void)data;
    return 1;
}

static unsigned char no_op_i2c_read(unsigned char ack)
{
    (void)ack;
    return 0;
}

static unsigned char no_op_spi_transfer(unsigned char output_byte)
{
    (void)output_byte;
    return 0;
}

void setUp(void)
{
    reset_output();

    str_print = capture_print;
    chr_print = NULL;
    float_print = capture_float_print;
    err_print = capture_print;
    warn_print = capture_print;
    int_input = no_op_int;

    platform_delay_ms = no_op_delay;
    platform_analog_read = no_op_analog;
    platform_pin_mode = no_op_pin;
    platform_digital_write = no_op_digital_write;
    platform_digital_read = no_op_digital_read;
    platform_pwm = no_op_pwm;
    platform_play_tone = no_op_tone;
    platform_is_key_pressed = NULL;
    platform_i2c_init = no_op;
    platform_i2c_start = no_op;
    platform_i2c_restart = no_op;
    platform_i2c_write = no_op_i2c_write;
    platform_i2c_read = no_op_i2c_read;
    platform_i2c_stop = no_op;
    platform_spi_init = no_op;
    platform_spi_select = no_op;
    platform_spi_deselect = no_op;
    platform_spi_transfer = no_op_spi_transfer;

    platform_sin = std::sin;
    platform_cos = std::cos;
    platform_tan = std::tan;
    platform_acos = std::acos;
    platform_asin = std::asin;
    platform_atan = std::atan;

    platform_sinh = std::sinh;
    platform_cosh = std::cosh;
    platform_tanh = std::tanh;
    platform_asinh = std::asinh;
    platform_acosh = std::acosh;
    platform_atanh = std::atanh;

    platform_atan2 = std::atan2;

    platform_log = std::log;
    platform_log10 = std::log10;
    platform_exp = std::exp;
    platform_sqrt = std::sqrt;
    platform_floor = std::floor;
    platform_ceil = std::ceil;

    init_parser();
    do_parse((char *)"NEW");
    reset_output();
}

void tearDown(void)
{
}

void test_abs_function_returns_positive_value(void);
void test_sin_function_returns_expected_value(void);
void test_cos_function_returns_expected_value(void);
void test_tan_function_returns_expected_value(void);
void test_log10_function_returns_expected_value(void);
void test_exp_function_returns_expected_value(void);
void test_sqrt_function_returns_expected_value(void);
void test_floor_and_ceil_functions(void);
void test_int_function_truncates_value(void);
void test_acos_function_returns_expected_value(void);
void test_asin_function_returns_expected_value(void);
void test_atan_function_returns_expected_value(void);
void test_sinh_function_returns_expected_value(void);
void test_cosh_function_returns_expected_value(void);
void test_tanh_function_returns_expected_value(void);
void test_asinh_function_returns_expected_value(void);
void test_acosh_function_returns_expected_value(void);
void test_atanh_function_returns_expected_value(void);
void test_atan2_function_returns_expected_value(void);
void test_log_function_returns_expected_value(void);

void test_immediate_let_and_print(void)
{
    char line1[] = "LET A = 4 + 3";
    char line2[] = "PRINT(A)";

    do_parse(line1);
    do_parse(line2);

    TEST_ASSERT_TRUE(strstr(g_output, "7") != NULL);
}

void test_numbered_program_is_stored_and_listed(void)
{
    char line1[] = "10 LET A = 2";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "LIST";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "10 LET A = 2") != NULL);
    TEST_ASSERT_TRUE(strstr(g_output, "20 PRINT(A)") != NULL);
    TEST_ASSERT_TRUE(strstr(g_output, "30 END") != NULL);
}

void test_run_executes_conditional_assignment(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 IF A = 1 THEN LET A = 2";
    char line3[] = "30 PRINT(A)";
    char line4[] = "40 END";
    char line5[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);

    TEST_ASSERT_TRUE(strstr(g_output, "2") != NULL);
}

void test_run_executes_conditional_assignment_false_branch(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 IF A = 2 THEN LET A = 3";
    char line3[] = "30 PRINT(A)";
    char line4[] = "40 END";
    char line5[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_run_executes_conditional_assignment_with_else_branch(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 IF A = 2 THEN";
    char line3[] = "30   LET A = 3";
    char line4[] = "40 ELSE";
    char line5[] = "50   LET A = 4";
    char line6[] = "60 PRINT(A)";
    char line7[] = "70 END";
    char line8[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);
    do_parse(line8);

    TEST_ASSERT_TRUE(strstr(g_output, "4") != NULL);
}

void test_run_executes_conditional_assignment_with_else_branch_and_nested_if(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 IF A = 2 THEN";
    char line3[] = "30   LET A = 3";
    char line4[] = "40 ELSE";
    char line5[] = "50   IF A = 1 THEN";
    char line6[] = "60     LET A = 4";
    char line7[] = "70   ENDIF";
    char line8[] = "80 ENDIF";
    char line9[] = "90 PRINT(A)";
    char line10[] = "100 END";
    char line11[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);
    do_parse(line8);
    do_parse(line9);
    do_parse(line10);
    do_parse(line11);

    TEST_ASSERT_TRUE(strstr(g_output, "4") != NULL);
}

void test_run_executes_conditional_assignment_with_else_branch_and_nested_if_false_branch(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 IF A = 2 THEN";
    char line3[] = "30   LET A = 3";
    char line4[] = "40 ELSE";
    char line5[] = "50   IF A = 2 THEN";
    char line6[] = "60     LET A = 4";
    char line7[] = "70   ENDIF";
    char line8[] = "80 ENDIF";
    char line9[] = "90 PRINT(A)";
    char line10[] = "100 END";
    char line11[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);
    do_parse(line8);
    do_parse(line9);
    do_parse(line10);
    do_parse(line11);
    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_run_executes_conditional_assignment_with_else_branch_and_nested_if_with_else_branch(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 IF A = 2 THEN";
    char line3[] = "30   LET A = 3";
    char line4[] = "40 ELSE";
    char line5[] = "50   IF A = 1 THEN";
    char line6[] = "60     LET A = 4";
    char line7[] = "70   ELSE";
    char line8[] = "80     LET A = 5";
    char line9[] = "90   ENDIF";
    char line10[] = "100 ENDIF";
    char line11[] = "110 PRINT(A)";
    char line12[] = "120 END";
    char line13[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);
    do_parse(line8);
    do_parse(line9);
    do_parse(line10);
    do_parse(line11);
    do_parse(line12);
    do_parse(line13);

    TEST_ASSERT_TRUE(strstr(g_output, "4") != NULL);
}

void test_run_executes_while_loop_with_conditional_assignment(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 WHILE A < 3";
    char line3[] = "30   LET A = A + 1";
    char line4[] = "40 WEND";
    char line5[] = "50 PRINT(A)";
    char line6[] = "60 END";
    char line7[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_run_executes_while_loop_with_fail_condition(void)
{
    char line1[] = "10 LET A = 10";
    char line2[] = "20 WHILE A < 3";
    char line3[] = "30   LET A = A + 1";
    char line4[] = "40 WEND";
    char line5[] = "50 PRINT(A)";
    char line6[] = "60 END";
    char line7[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);

    TEST_ASSERT_TRUE(strstr(g_output, "10") != NULL);
}

void test_run_executes_for_loop_with_conditional_assignment(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 FOR I = 1 TO 3";
    char line3[] = "30   LET A = A + 1";
    char line4[] = "40 NEXT I";
    char line5[] = "50 PRINT(A)";
    char line6[] = "60 END";
    char line7[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_run_executes_for_loop_with_conditional_assignment_and_step(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 FOR I = 1 TO 5 STEP 2";
    char line3[] = "30   LET A = A + 1";
    char line4[] = "40 NEXT I";
    char line5[] = "50 PRINT(A)";
    char line6[] = "60 END";
    char line7[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_run_executes_for_loop_with_conditional_assignment_and_step_negative(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 FOR I = 5 TO 1 STEP -2";
    char line3[] = "30   LET A = A + 1";
    char line4[] = "40 NEXT I";
    char line5[] = "50 PRINT(A)";
    char line6[] = "60 END";
    char line7[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_run_executes_repeat_loop_with_conditional_assignment(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 REPEAT";
    char line3[] = "30   LET A = A + 1";
    char line4[] = "40 UNTIL A >= 3";
    char line5[] = "50 PRINT(A)";
    char line6[] = "60 END";
    char line7[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_run_executes_repeat_loop_with_conditional_assignment_and_nested_if(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 REPEAT";
    char line3[] = "30   IF A < 2 THEN";
    char line4[] = "40     LET A = A + 1";
    char line5[] = "50   ELSE";
    char line6[] = "60     LET A = A + 2";
    char line7[] = "70   ENDIF";
    char line8[] = "80 UNTIL A >= 5";
    char line9[] = "90 PRINT(A)";
    char line10[] = "100 END";
    char line11[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);
    do_parse(line8);
    do_parse(line9);
    do_parse(line10);
    do_parse(line11);

    TEST_ASSERT_TRUE(strstr(g_output, "6") != NULL);
}

void test_run_executes_gosub_and_return(void)
{
    char line1[] = "10 GOSUB 100";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "100 LET A = 42";
    char line5[] = "110 RETURN";
    char line6[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);

    TEST_ASSERT_TRUE(strstr(g_output, "42") != NULL);
}

void test_run_executes_on_gosub_and_return(void)
{
    char line1[] = "10 LET A = 1";
    char line2[] = "20 ON A, 50, 70, 90";
    char line3[] = "30 PRINT(A)";
    char line4[] = "40 END";
    char line5[] = "50 LET A = 2";
    char line6[] = "60 RETURN";
    char line7[] = "70 LET A = 3";
    char line8[] = "80 RETURN";
    char line9[] = "90 LET A = 4";
    char line10[] = "100 RETURN";
    char line11[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);
    do_parse(line7);
    do_parse(line8);
    do_parse(line9);
    do_parse(line10);
    do_parse(line11);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_logical_and_both_true(void)
{
    char line1[] = "10 LET A = (1 > 0) AND (2 > 1)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_logical_and_one_false(void)
{
    char line1[] = "10 LET A = (1 > 0) AND (2 < 1)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_logical_and_both_false(void)
{
    char line1[] = "10 LET A = (1 < 0) AND (2 < 1)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_logical_or_both_true(void)
{
    char line1[] = "10 LET A = (1 > 0) OR (2 > 1)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_logical_or_one_true(void)
{
    char line1[] = "10 LET A = (1 > 0) OR (2 < 1)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_logical_or_both_false(void)
{
    char line1[] = "10 LET A = (1 < 0) OR (2 < 1)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_logical_not(void)
{
    char line1[] = "10 LET A = NOT 0";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "-1") != NULL);
}

void test_logical_and_short_circuit(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 LET B = (0 > 1) AND (A = 1)";
    char line3[] = "30 PRINT(B)";
    char line4[] = "40 PRINT(A)";
    char line5[] = "50 END";
    char line6[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_bitwise_and_basic(void)
{
    char line1[] = "10 LET A = 12 BAND 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "8") != NULL);
}

void test_bitwise_or_basic(void)
{
    char line1[] = "10 LET A = 12 BOR 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "14") != NULL);
}

void test_bitwise_xor_basic(void)
{
    char line1[] = "10 LET A = 12 XOR 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "6") != NULL);
}

void test_bitwise_nand_basic(void)
{
    char line1[] = "10 LET A = 12 NAND 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "-9") != NULL);
}

void test_bitwise_nor_basic(void)
{
    char line1[] = "10 LET A = 12 NOR 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "-15") != NULL);
}

void test_bitwise_eqv_basic(void)
{
    char line1[] = "10 LET A = 12 EQV 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "-7") != NULL);
}

void test_bitwise_imp_basic(void)
{
    char line1[] = "10 LET A = 12 IMP 10";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "-5") != NULL);
}

void test_bitwise_and_with_zero(void)
{
    char line1[] = "10 LET A = 15 BAND 0";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_bitwise_or_with_zero(void)
{
    char line1[] = "10 LET A = 15 BOR 0";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "15") != NULL);
}

void test_bitwise_xor_same_values(void)
{
    char line1[] = "10 LET A = 15 XOR 15";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_left_shift_basic(void)
{
    char line1[] = "10 LET A = LSHIFT(1, 4)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "16") != NULL);
}

void test_left_shift_multiple(void)
{
    char line1[] = "10 LET A = LSHIFT(3, 2)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "12") != NULL);
}

void test_right_shift_basic(void)
{
    char line1[] = "10 LET A = RSHIFT(16, 2)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "4") != NULL);
}

void test_right_shift_multiple(void)
{
    char line1[] = "10 LET A = RSHIFT(12, 2)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_logical_and_with_arithmetic(void)
{
    char line1[] = "10 LET A = (5 + 3 > 5) AND (10 - 2 < 20)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_logical_or_with_arithmetic(void)
{
    char line1[] = "10 LET A = (5 + 3 < 5) OR (10 - 2 < 20)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_bitwise_and_with_shift(void)
{
    char line1[] = "10 LET A = LSHIFT(1, 2) BAND LSHIFT(1, 3)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_bitwise_or_with_shift(void)
{
    char line1[] = "10 LET A = LSHIFT(1, 2) BOR LSHIFT(1, 3)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "12") != NULL);
}

void test_multiple_logical_operators(void)
{
    char line1[] = "10 LET A = (1 < 5) AND (2 < 3) OR (4 > 5)";
    char line2[] = "20 PRINT(A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_bitwise_operators_in_conditional(void)
{
    char line1[] = "10 LET A = 0";
    char line2[] = "20 LET B = 5 BAND 3";
    char line3[] = "30 IF B > 0 THEN LET A = 1";
    char line4[] = "40 PRINT(A)";
    char line5[] = "50 END";
    char line6[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);
    do_parse(line5);
    do_parse(line6);

    TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_abs_function_returns_positive_value(void)
{
  char line1[] = "10 LET A = ABS(-7)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "7") != NULL);
}

void test_sin_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = SIN(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_cos_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = COS(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_tan_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = TAN(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_log10_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = LOG10(100)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "2") != NULL);
}

void test_exp_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = EXP(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_sqrt_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = SQRT(16)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "4") != NULL);
}

void test_floor_and_ceil_functions(void)
{
  char line1[] = "10 LET A = FLOOR(2.9)";
  char line2[] = "20 LET B = CEIL(2.1)";
  char line3[] = "30 PRINT(A)";
  char line4[] = "40 PRINT(B)";
  char line5[] = "50 END";
  char line6[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);
  do_parse(line5);
  do_parse(line6);

  TEST_ASSERT_TRUE(strstr(g_output, "2") != NULL);
  TEST_ASSERT_TRUE(strstr(g_output, "3") != NULL);
}

void test_int_function_truncates_value(void)
{
  char line1[] = "10 LET A = INT(2.9)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "2") != NULL);
}

void test_acos_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ACOS(1)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_asin_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ASIN(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_atan_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ATAN(1)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0.785") != NULL);
}

void test_sinh_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = SINH(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_cosh_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = COSH(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "1") != NULL);
}

void test_tanh_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = TANH(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_asinh_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ASINH(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_acosh_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ACOSH(1)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_atanh_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ATANH(0)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_atan2_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = ATAN2(1, 1)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0.785") != NULL);
}

void test_log_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = LOG(1)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "0") != NULL);
}

void test_deg_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = DEG(3.141592653589793)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "180") != NULL);
}

void test_rad_function_returns_expected_value(void)
{
  char line1[] = "10 LET A = RAD(180)";
  char line2[] = "20 PRINT(A)";
  char line3[] = "30 END";
  char line4[] = "RUN";

  do_parse(line1);
  do_parse(line2);
  do_parse(line3);
  do_parse(line4);

  TEST_ASSERT_TRUE(strstr(g_output, "3.14159") != NULL);
}

void test_formatter_binary(void)
{
    char line1[] = "10 LET A = 10";
    char line2[] = "20 PRINT(BIN, A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "1010") != NULL);
}

void test_formatter_hexadecimal(void)
{
    char line1[] = "10 LET A = 127";
    char line2[] = "20 PRINT(HEX, A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "7f") != NULL);
}

void test_formatter_octal(void)
{
    char line1[] = "10 LET A = 127";
    char line2[] = "20 PRINT(OCT, A)";
    char line3[] = "30 END";
    char line4[] = "RUN";

    do_parse(line1);
    do_parse(line2);
    do_parse(line3);
    do_parse(line4);

    TEST_ASSERT_TRUE(strstr(g_output, "177") != NULL);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_immediate_let_and_print);
    RUN_TEST(test_numbered_program_is_stored_and_listed);

    RUN_TEST(test_run_executes_conditional_assignment);
    RUN_TEST(test_run_executes_conditional_assignment_false_branch);
    RUN_TEST(test_run_executes_conditional_assignment_with_else_branch);
    RUN_TEST(test_run_executes_conditional_assignment_with_else_branch_and_nested_if);
    RUN_TEST(test_run_executes_conditional_assignment_with_else_branch_and_nested_if_false_branch);

    RUN_TEST(test_run_executes_while_loop_with_conditional_assignment);
    RUN_TEST(test_run_executes_while_loop_with_fail_condition);
    RUN_TEST(test_run_executes_for_loop_with_conditional_assignment);
    RUN_TEST(test_run_executes_for_loop_with_conditional_assignment_and_step);
    RUN_TEST(test_run_executes_for_loop_with_conditional_assignment_and_step_negative);
    RUN_TEST(test_run_executes_repeat_loop_with_conditional_assignment);
    RUN_TEST(test_run_executes_repeat_loop_with_conditional_assignment_and_nested_if);

    RUN_TEST(test_run_executes_gosub_and_return);
    RUN_TEST(test_run_executes_on_gosub_and_return);

    RUN_TEST(test_logical_and_both_true);
    RUN_TEST(test_logical_and_one_false);
    RUN_TEST(test_logical_and_both_false);
    RUN_TEST(test_logical_or_both_true);
    RUN_TEST(test_logical_or_one_true);
    RUN_TEST(test_logical_or_both_false);
    RUN_TEST(test_logical_not);
    RUN_TEST(test_logical_and_short_circuit);

    RUN_TEST(test_bitwise_and_basic);
    RUN_TEST(test_bitwise_or_basic);
    RUN_TEST(test_bitwise_xor_basic);
    RUN_TEST(test_bitwise_nand_basic);
    RUN_TEST(test_bitwise_nor_basic);
    RUN_TEST(test_bitwise_eqv_basic);
    RUN_TEST(test_bitwise_imp_basic);
    RUN_TEST(test_bitwise_and_with_zero);
    RUN_TEST(test_bitwise_or_with_zero);
    RUN_TEST(test_bitwise_xor_same_values);

    RUN_TEST(test_left_shift_basic);
    RUN_TEST(test_left_shift_multiple);
    RUN_TEST(test_right_shift_basic);
    RUN_TEST(test_right_shift_multiple);

    RUN_TEST(test_logical_and_with_arithmetic);
    RUN_TEST(test_logical_or_with_arithmetic);
    RUN_TEST(test_bitwise_and_with_shift);
    RUN_TEST(test_bitwise_or_with_shift);
    RUN_TEST(test_multiple_logical_operators);
    RUN_TEST(test_bitwise_operators_in_conditional);

    RUN_TEST(test_abs_function_returns_positive_value);

    RUN_TEST(test_log10_function_returns_expected_value);
    RUN_TEST(test_exp_function_returns_expected_value);
    RUN_TEST(test_sqrt_function_returns_expected_value);
    RUN_TEST(test_floor_and_ceil_functions);
    RUN_TEST(test_int_function_truncates_value);

    RUN_TEST(test_sin_function_returns_expected_value);
    RUN_TEST(test_cos_function_returns_expected_value);
    RUN_TEST(test_tan_function_returns_expected_value);
    RUN_TEST(test_acos_function_returns_expected_value);
    RUN_TEST(test_asin_function_returns_expected_value);
    RUN_TEST(test_atan_function_returns_expected_value);
    RUN_TEST(test_sinh_function_returns_expected_value);
    RUN_TEST(test_cosh_function_returns_expected_value);
    RUN_TEST(test_tanh_function_returns_expected_value);
    RUN_TEST(test_asinh_function_returns_expected_value);
    RUN_TEST(test_acosh_function_returns_expected_value);
    RUN_TEST(test_atanh_function_returns_expected_value);
    RUN_TEST(test_atan2_function_returns_expected_value);
    RUN_TEST(test_log_function_returns_expected_value);

    RUN_TEST(test_deg_function_returns_expected_value);
    RUN_TEST(test_rad_function_returns_expected_value);

    RUN_TEST(test_formatter_binary);
    RUN_TEST(test_formatter_hexadecimal);
    RUN_TEST(test_formatter_octal);
    
    return UNITY_END();
}
