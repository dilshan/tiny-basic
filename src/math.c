#define PI 3.14159265358979323846

double max(double a, double b)
{
    return (a > b) ? a : b;
}

double min(double a, double b)
{
    return (a < b) ? a : b;
}

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

double rad_to_deg(double radians)
{
    return radians * (180.0 / PI);
}

double deg_to_rad(double degrees)
{
    return degrees * (PI / 180.0);
}