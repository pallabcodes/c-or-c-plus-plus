#include <stdio.h>

// when the no. of recursive calls exceed one time, then it is 'tree recursion'

void fun(int n)
{
    if (n > 0)
    {
        printf("%d ", n);

        // now, as seen below it has 2 recursive calls so it is a 'tree recursion'

        fun(n - 1);
        fun(n - 1);
    }
}

int main()
{
    fun(3);
    return 0;
}