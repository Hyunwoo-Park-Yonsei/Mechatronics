#pragma warning(disable : 4996)
#include <stdio.h>

int main(void)

{

    int num1;
    printf("������ �Է��ϼ���: ");
    scanf("%d", &num1);
    printf("\n");
    int* ptr = &num1;
    printf("������ �� : %d \n", num1);
    printf("������ �ּ� : %p \n", &num1);
    printf("������ ����Ű�� �������� �� : %p \n", ptr);
    printf("������ ����Ű�� �������� �ּ� : %p \n", &ptr);
    printf("������ ����Ű�� �����Ͱ� ����Ű�� ������ �� : %d \n", *ptr);

    printf("\n������ 1�� ���غ��ϴ�.");
  
    num1++;

    printf("������ �� : %d \n", num1);
    printf("������ �ּ� : %p \n", &num1);
    printf("������ ����Ű�� �������� �� : %p \n", ptr);
    printf("������ ����Ű�� �������� �ּ� : %p \n", &ptr);
    printf("������ ����Ű�� �����Ͱ� ����Ű�� ������ �� : %d \n", *ptr);


    printf("\n�����Ͱ� ����Ű�� �ּҿ� 1�� ���غ��ϴ�.");
    ptr += 1;

    printf("������ �� : %d \n", num1);
    printf("������ �ּ� : %p \n", &num1);
    printf("������ ����Ű�� �������� �� : %p \n", ptr);
    printf("������ ����Ű�� �������� �ּ� : %p \n", &ptr);
    printf("������ ����Ű�� �����Ͱ� ����Ű�� ������ �� : %d \n", *ptr);

    return 0;
}
