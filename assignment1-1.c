#pragma warning(disable : 4996)
#include <stdio.h>

int main(void)

{

    int num1;
    printf("정수를 입력하세요: ");
    scanf("%d", &num1);
    printf("\n");
    int* ptr = &num1;
    printf("정수의 값 : %d \n", num1);
    printf("정수의 주소 : %p \n", &num1);
    printf("정수를 가리키는 포인터의 값 : %p \n", ptr);
    printf("정수를 가리키는 포인터의 주소 : %p \n", &ptr);
    printf("정수를 가리키는 포인터가 가리키는 변수의 값 : %d \n", *ptr);

    printf("\n정수에 1을 더해봅니다.");
  
    num1++;

    printf("정수의 값 : %d \n", num1);
    printf("정수의 주소 : %p \n", &num1);
    printf("정수를 가리키는 포인터의 값 : %p \n", ptr);
    printf("정수를 가리키는 포인터의 주소 : %p \n", &ptr);
    printf("정수를 가리키는 포인터가 가리키는 변수의 값 : %d \n", *ptr);


    printf("\n포인터가 가리키는 주소에 1을 더해봅니다.");
    ptr += 1;

    printf("정수의 값 : %d \n", num1);
    printf("정수의 주소 : %p \n", &num1);
    printf("정수를 가리키는 포인터의 값 : %p \n", ptr);
    printf("정수를 가리키는 포인터의 주소 : %p \n", &ptr);
    printf("정수를 가리키는 포인터가 가리키는 변수의 값 : %d \n", *ptr);

    return 0;
}
