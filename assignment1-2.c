#pragma warning(disable : 4996)
#include <stdio.h>

int main(void)

{
    int sum = 0;
    int temp = 0;
    int op;
    while (sum <= 100) {
        int num1, num2;
        printf("Write First Num and Second Num:");
        scanf("%d %d", &num1,&num2);
        printf("\n");

        printf("Operater List - [1] Plus / [2] Minus / [3] Multiply / [4] Divide \n");
        printf("Write Operator Num: ");
        scanf("%d", &op);
        printf("\n");




        if (op == 1) { temp = num1 + num2; }
        else if (op == 2) { temp = num1 - num2; }
        else if (op == 3) { temp = num1 * num2; }
        else if (op == 4) { temp = num1 / num2; }


        sum += temp;
        printf("Answer : %d / Sum of Answer :%d\n", temp, sum);
        printf("\n");
    }
    printf("\nSum is larger thane 100 - Break!");
    return 0;
}
