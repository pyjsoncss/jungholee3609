#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int count(int x, char A) {
	int row = 0, col = 0, diagonal1 = 0, diagonal2 = 0, max = 0;
	char board[3][3];
	for (int i = 0; i < 3; i++)
	{
		if (board[x][i] == A) {
			row++;
		}
		if (board[i][x] == A) {
			col++;
		}
		if (board[i][i] == A ) {
			diagonal1++;
		}
		for (int j = 0; j < 3; j++){
		if (board[i][j] == A && i + j == 2 ) {
			diagonal2++;
		}
				}
	}
	if (row > max)
		max = row;
	if (col > max)
		max = col;
	if (diagonal1 > max)
		max = diagonal1;
	if (diagonal2 > max)
		max = diagonal2;

	return max;
	
}


int main(void) {

	char A;
	char board[3][3];
	int x, y, k, i;
	// 보드를 초기화한다.
	for (x = 0; x < 3; x++)
		for (y = 0; y < 3; y++) board[x][y] = ' ';
	// 사용자로부터 위치를 받아서 보드에 표시한다.
	for (k = 0; k < 9; k++) {
		if (k % 2 == 0) {
		repeat:
			printf("(x, y) 좌표: ");
			scanf(" %d %d", &x, &y);
			if (board[x][y] == ' ') {
				board[x][y] = '0';
			}
			else {
				printf("빈 좌표를 입력하시오");
				goto repeat;
			}
		}


		if (k % 2 == 1) {
			printf("컴퓨터 차례\n");
			for (x = 0; x < 3; x++) {
				for (y = 0; y < 3; y++) {

					if (board[x][y] == ' ') {

						board[x][y] = 'X';
						goto pass;
					}
				}
			}

		}
	pass:
		// 보드를 화면에 그린다.
		for (i = 0; i < 3; i++) {
			printf("--|--|--\n");
			printf("%c | %c| %c \n", board[i][0], board[i][1], board[i][2]);
		}
		printf("--|--|--\n");
		for (i = 0; i < 3; i++){
			if (board[i][0]=='X' && board[i][1] == 'X' && board[i][2] == 'X') 
				printf("X 승리");
			
			else if (board[0][i] == 'X' && board[1][i] == 'X' && board[2][i] == 'X')
			printf("X 승리");

			else if(board[0][0]=='X' && board[1][1]=='X' && board[2][2]=='X')
			printf("X 승리");

			else if (board[0][2] == 'X' && board[1][1] == 'X' && board[2][0] == 'X')
				printf("X 승리");

		}



		
	}
	
		return 0;

	
}
