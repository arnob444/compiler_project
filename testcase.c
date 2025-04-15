#include <stdio.h>
// Function to calculate the sum of two integers
int calculateSum(int num1, int num2)
{
    int _sum = num1 + num2;
    return _sum;
}
/*
   This is a multiline comment.
   It can span multiple lines.
*/

int main()
{
    int num1 = 10;
    int num2 = 20;
    int result;  

    result = calculateSum(num1, num2);
    printf("The sum of %d and %d is: %d\n", num1, num2, result);
    return 0;
}