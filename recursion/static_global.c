#include <stdio.h>

int add(int n)
{
    static int x = 0;
    if (n > 0)
    {
        x++;
        return add(n - 1) + x;
    }

    return 0;
}

int main()
{
    int x = 5;
    int result = add(x); // add a breakpoint here to debug

    printf("%d\n", result);

    return 0;
}