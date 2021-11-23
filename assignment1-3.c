#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void PrintCard(int* p, int length)
{
	for (int i = 0; i < length; i++)
	{
		printf("%d   ",p[i]);
	}

}

void PrintAllMemeberCard(int* A, int* B, int* C, int* D, int* cardNum)
{
	printf("A >>");
	PrintCard(A, cardNum[0]);
	printf("\nB >>");
	PrintCard(B, cardNum[1]);
	printf("\nC >>");
	PrintCard(C, cardNum[2]);
	printf("\nD >>");
	PrintCard(D, cardNum[3]);


}

void CreateCardStack(int* cardStack)
{
	for (int i = 0; i < 53; i++) {
		cardStack[i] = (i+3) / 4 ;
	}

}

void CardDistribute(int* A, int* B, int* C, int* D, int* cardStack, int* cardNum)
{
	srand((unsigned int)time(NULL));
	int fourteen = 0;
	for (int i = 0; i < 53; i++) {
		int randomNumber = rand() % 4;

		
		if (fourteen == 0) {
			if (randomNumber == 0) {
				A[cardNum[randomNumber]] = cardStack[i];
				cardNum[randomNumber] += 1;
			}
			else if (randomNumber == 1) {
				B[cardNum[randomNumber]] = cardStack[i];
				cardNum[randomNumber] += 1;
			}
			else if (randomNumber == 2) {
				C[cardNum[randomNumber]] = cardStack[i];
				cardNum[randomNumber] += 1;
			}
			else if (randomNumber == 3) {
				D[cardNum[randomNumber]] = cardStack[i];
				cardNum[randomNumber] += 1;
			}
		}
		else if (fourteen == 1) {
			if (cardNum[randomNumber] > 12) {
				i -= 1;
			}
			else {
				if (randomNumber == 0) {
					A[cardNum[randomNumber]] = cardStack[i];
					cardNum[randomNumber] += 1;
				}
				else if (randomNumber == 1) {
					B[cardNum[randomNumber]] = cardStack[i];
					cardNum[randomNumber] += 1;
				}
				else if (randomNumber == 2) {
					C[cardNum[randomNumber]] = cardStack[i];
					cardNum[randomNumber] += 1;
				}
				else if (randomNumber == 3) {
					D[cardNum[randomNumber]] = cardStack[i];
					cardNum[randomNumber] += 1;
				}
			}

		}
		if (cardNum[randomNumber] == 14) {
			fourteen = 1;
		}

	}

	


}

void ArrayPop(int* arr, int* arrLength, int popIndex)
{
	for (int i = popIndex; i < *arrLength-1; i++) {
		arr[i] = arr[i + 1];
	}
	*arrLength -= 1;
}

void ArrayPush(int* arr, int* arrLength, int num)
{
	
	//*arrLength += 1;
	//arr[*arrLength-1] = num;

	int ths = -1;
	*arrLength += 1;
	for (int i = 0; i < *arrLength; i++) {
		if (num <= arr[i]) { 
			ths = i;
			break;
		}
		ths = i;
	}
	
	for (int i = *arrLength-1; i > ths; i--) {
		arr[i] = arr[i - 1];
	}
	arr[ths] = num;


}

void RemovePairedCard(int* holdingCard, int* cardNum)
{
	int i = 0;
	int popIdx[14];
	int popIdxLength = 0;
	while (i < *cardNum-1) {
		if (holdingCard[i] == holdingCard[i + 1]) {
			//printf("\n This is going to be removed \n %d %d \n", holdingCard[i], holdingCard[i + 1]);
			/*ArrayPop(holdingCard, cardNum, i);
			ArrayPop(holdingCard, cardNum, i);*/
			popIdx[popIdxLength] = i;
			popIdxLength += 1;
			i += 2;
		}
		else { i += 1; }
	}
	//printf("\n popIdxLength \n");
	//for (int i = 0; i < popIdxLength; i++) {
	//	printf("%d ", popIdx[i]);
	//}
	//printf("\n");
	
	for (int i = 0; i < popIdxLength; i++) {
		ArrayPop(holdingCard, cardNum, popIdx[i] - i*2);
		ArrayPop(holdingCard, cardNum, popIdx[i] - i*2);
	}
	

}

void Game(int* A, int* B, int* C, int* D, int* cardNum, int* playerOrder)
{
	srand((unsigned int)time(NULL));
	int randomNumber = rand() % 4;
	int gameOrder[4];
	int order;
	int playerNum = 4;
	int step = 1;
	for (int i = 0; i < 4; i++)
	{
		order = randomNumber + i;
		if (order > 3) { order -= 4; }
		gameOrder[i] = order;
		
	}


	printf("\n");
	while (playerNum > 1) {
		for (int i = 0; i < playerNum; i++) {
			//다음 순서 사람꺼 뽑기
			if (gameOrder[i] == 0 && cardNum[0] > 0) {
				printf("[ Step %d]        ", step);
				step++;
				if(cardNum[1] > 0){
					int randomCard = rand() % cardNum[1];
					printf("A picked %d from B \n", B[randomCard]);
					ArrayPush(A, &cardNum[0], B[randomCard]);
					ArrayPop(B, &cardNum[1], randomCard);
					RemovePairedCard(A, &cardNum[0]);
				}
				else if (cardNum[2] > 0) {
					int randomCard = rand() % cardNum[2];
					printf("A picked %d from C \n", C[randomCard]);
					ArrayPush(A, &cardNum[0], C[randomCard]);
					ArrayPop(C, &cardNum[2], randomCard);
					RemovePairedCard(A, &cardNum[0]);
				}
				else {
					int randomCard = rand() % cardNum[3];
					printf("A picked %d from D \n", D[randomCard]);
					ArrayPush(A, &cardNum[0], D[randomCard]);
					ArrayPop(D, &cardNum[3], randomCard);
					RemovePairedCard(A, &cardNum[0]);
				}

			}
			else if (gameOrder[i] == 1 && cardNum[1] > 0) {
				printf("[ Step %d]        ", step);
				step++;
				if (cardNum[2] > 0) {
					int randomCard = rand() % cardNum[2];
					printf("B picked %d from C \n", C[randomCard]);
					ArrayPush(B, &cardNum[1], C[randomCard]);
					ArrayPop(C, &cardNum[2], randomCard);
					RemovePairedCard(B, &cardNum[1]);
				}
				else if (cardNum[3] > 0) {
					int randomCard = rand() % cardNum[3];
					printf("B picked %d from D \n", D[randomCard]);
					ArrayPush(B, &cardNum[1], D[randomCard]);
					ArrayPop(D, &cardNum[3], randomCard);
					RemovePairedCard(B, &cardNum[1]);
				}
				else {
					int randomCard = rand() % cardNum[0];
					printf("B picked %d from A \n", A[randomCard]);
					ArrayPush(B, &cardNum[1], A[randomCard]);
					ArrayPop(A, &cardNum[0], randomCard);
					RemovePairedCard(B, &cardNum[1]);
				}
			}
			else if (gameOrder[i] == 2 && cardNum[2] > 0) {
				printf("[ Step %d]        ", step);
				step++;
				if (cardNum[3] > 0) {
					int randomCard = rand() % cardNum[3];
					printf("C picked %d from D \n", D[randomCard]);
					ArrayPush(C, &cardNum[2], D[randomCard]);
					ArrayPop(D, &cardNum[3], randomCard);
					RemovePairedCard(C, &cardNum[2]);
				}
				else if (cardNum[0] > 0) {
					int randomCard = rand() % cardNum[0];
					printf("C picked %d from A \n", A[randomCard]);
					ArrayPush(C, &cardNum[2], A[randomCard]);
					ArrayPop(A, &cardNum[0], randomCard);
					RemovePairedCard(C, &cardNum[2]);
				}
				else {
					int randomCard = rand() % cardNum[1];
					printf("C picked %d from B\n", B[randomCard]);
					ArrayPush(C, &cardNum[2], B[randomCard]);
					ArrayPop(B, &cardNum[1], randomCard);
					RemovePairedCard(C, &cardNum[2]);
				}

			}
			else if (gameOrder[i] == 3 && cardNum[3] > 0) {
				printf("[ Step %d]        ", step);
				step++;
				if (cardNum[0] > 0) {
					int randomCard = rand() % cardNum[0];
					printf("D picked %d from A \n", A[randomCard]);
					ArrayPush(D, &cardNum[3], A[randomCard]);
					ArrayPop(A, &cardNum[0], randomCard);
					RemovePairedCard(D, &cardNum[3]);
				}
				else if (cardNum[1] > 0) {
					int randomCard = rand() % cardNum[1];
					printf("D picked %d from B \n", B[randomCard]);
					ArrayPush(D, &cardNum[3], B[randomCard]);
					ArrayPop(B, &cardNum[1], randomCard);
					RemovePairedCard(D, &cardNum[3]);
				}
				else {
					int randomCard = rand() % cardNum[2];
					printf("D picked %d from C \n", C[randomCard]);
					ArrayPush(D, &cardNum[3], C[randomCard]);
					ArrayPop(C, &cardNum[2], randomCard);
					RemovePairedCard(D, &cardNum[3]);
				}

			}
			int t = playerNum;
			for (int j = 0; j < t; j++) {
				if (cardNum[gameOrder[j]] == 0) {
					playerOrder[4 - playerNum] = gameOrder[j];
					ArrayPop(gameOrder, &playerNum, j);
					
				}
			}


			PrintAllMemeberCard(A, B, C, D, cardNum);
			printf("\n\n");
		}
	}
	playerOrder[3] = gameOrder[0];
}

void PrintPlayerOrder(int* playerOrder) {
	char playerAlphabet[4] = { "ABCD" };
	for (int i = 0; i < 4; i++) {

		printf("%dst      %c Player \n", i+1,playerAlphabet[playerOrder[i]]);
	}

}
int main()
{
	// 변수 설정
	int cardStack[53] = { 0, };
	int A[14] = { 0, }, B[14] = { 0, }, C[14] = { 0, }, D[14] = { 0, };
	int cardHoldingNum[4] = { 0, };
	int playerOrder[4] = {0,};

	// 카드팩 생성 (조커: 0, A: 1, J: 11, Q: 12, K: 13)
	CreateCardStack(cardStack);
	// 플레이어에게 카드 무작위 분배 (한 사람: 14장, 나머지 사람: 13장)
	printf(">>>>> Card Distribution <<<<<\n\n");
	CardDistribute(A, B, C, D, cardStack, cardHoldingNum);
	// 무작위로 수령한 플레이어 카드 출력
	PrintAllMemeberCard(A, B, C, D, cardHoldingNum);

	// 짝이 맞는 카드 버리기
	printf("\n\n>>>>> Remove Paired Cards <<<<<\n\n");
	RemovePairedCard(A, &cardHoldingNum[0]);
	RemovePairedCard(B, &cardHoldingNum[1]);
	RemovePairedCard(C, &cardHoldingNum[2]);
	RemovePairedCard(D, &cardHoldingNum[3]);
	// 짝이 맞는 카드를 버린 후 플레이어 카드 출력
	PrintAllMemeberCard(A, B, C, D, cardHoldingNum);
	// 게임 진행
	printf("\n\n>>>>> Game Start <<<<<\n\n");
	Game(A, B, C, D, cardHoldingNum, playerOrder);


	 //게임 순위 출력
	printf("\n>>>>> Game End <<<<<\n\n");
	PrintPlayerOrder(playerOrder);
	return 0;
}