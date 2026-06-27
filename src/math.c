int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
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