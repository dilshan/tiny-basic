// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

#define PI 3.14159265358979323846

/**
 * @brief Return the highest value from different pairs of numbers.
 * @param a Specifies a value to be compared.
 * @param b Specified a value to be compared. 
 * @return Return a value representing the highest of two numbers.
 */
double max(double a, double b)
{
    return (a > b) ? a : b;
}

/**
 * @brief Return the lowest value from different pairs of numbers.
 * @param a Specifies a value to be compared.
 * @param b Specified a value to be compared. 
 * @return Return a value representing the lowest of two numbers.
 */
double min(double a, double b)
{
    return (a < b) ? a : b;
}

/**
 * @brief Returns the value of x to the power of y (x^y).
 * @param base The base (x).
 * @param exp The exponent (y). 
 * @return A number representing the value of x to the power of y (x^y).
 */
int power(int base, int exp)
{
    int result = 1;

    while (exp > 0)
    {
        if (exp & 1)
        {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }

    return result;
}

/**
 * @brief Converts angle units from radians to degrees.
 * @param radians Angle in radians.
 * @return The converted value in degrees.
 */
double rad_to_deg(double radians)
{
    return radians * (180.0 / PI);
}

/**
 * @brief Converts angle units from degrees to radians.
 * @param degrees Angle in degrees.
 * @return The converted value in radians.
 */
double deg_to_rad(double degrees)
{
    return degrees * (PI / 180.0);
}