#include <stdio.h>

//  Head recursion:  when a recursive fn has a recursive call where last statement or piece of code is not recursive call

void headRecursion(int n)
{
    if (n > 0)
    {
        headRecursion(n - 1);
        printf("%d ", n);
    }
}

void headIteration(int n)
{
    int i = 1;
    while (i <= n)
    {
        printf("%d ", n);
        n++;
    }
}

//  Tail recursion:  when a recursive fn has a recursive call as last statement or piece of code , i.e. known as 'tail recursion'

void tailRecursive(int n)
{
    if (n > 0)
    {
        printf("%d ", n); // printf("%d\n", n);
        tailRecursive(n - 1);
    }
}

// Below while loop works same as the above `tailRecursion`
void tailIteration(int n)
{
    while (n > 0)
    {
        printf("%d ", n);
        n--;
    }
}

void fun(int n)
{
    if (n > 0)
    {
        printf("%d ", n);
        fun(n - 1);
        printf("%d ", n);
    }
}

int main()
{
    int x = 3;
    tailRecursive(x);
    headRecursion(x);

    return 0;
}