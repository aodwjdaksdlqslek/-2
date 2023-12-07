#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>


#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void JB_sample_init(void);
void JB_move_manual(key_t key);
void JB_move_random(int i, int dir);
void JB_move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기

void JB_sample_init(void) {
	//map_init(3, 31);
	for (int i = 0; i < ROW_MAX; i++) {
		for (int j = 0; j < COL_MAX; j++) {
			back_buf[i][j] = front_buf[i][j] = ' ';
		}
	}
	N_ROW = 5;
	N_COL = 21;
	for (int i = 0; i < N_ROW; i++) {
		// 대입문 이렇게 쓸 수 있는데 일부러 안 가르쳐줬음
		back_buf[i][0] = back_buf[i][N_COL - 1] = '*';

		for (int j = 1; j < N_COL - 1; j++) {
			back_buf[i][j] = (i == 0 || i == N_ROW - 1) ? '*' : ' ';
		}
	}
	

	// 플레이어들의 초기 위치를 미리 정의
	int initial_positions[PLAYER_MAX][2] = {
		{(1, N_ROW - 3), (1, N_COL - 19)},   // 플레이어 0
		{(1, N_ROW - 3), (1, N_COL - 17)},   // 플레이어 1
		{(1, N_ROW - 3), (1, N_COL - 15)},   // 플레이어 2
		{(1, N_ROW - 3), (1, N_COL - 13)},   // 플레이어 3
		{(1, N_ROW - 3), (1, N_COL - 11)},   // 플레이어 4
		{(1, N_ROW - 3), (1, N_COL - 9)},   // 플레이어 5
		{(1, N_ROW - 3), (1, N_COL - 7)},   // 플레이어 6
		{(1, N_ROW - 3), (1, N_COL - 5)},   // 플레이어 7
		{(1, N_ROW - 3), (1, N_COL - 3)},   // 플레이어 8
		{(1, N_ROW - 3), (1, N_COL - 1)},   // 플레이어 9

	};

	for (int i = 0; i < n_player; i++) {
		px[i] = initial_positions[i][0];
		py[i] = initial_positions[i][1];


		back_buf[px[i]][py[i]] = '?';
	}

	tick = 0;
}

void JB_move_manual(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	JB_move_tail(0, nx, ny);
}

// 0 <= dir < 4가 아니면 랜덤
void JB_move_random(int player, int dir) {
	int p = player;  // 이름이 길어서...
	int nx, ny;  // 움직여서 다음에 놓일 자리

	// 움직일 공간이 없는 경우는 없다고 가정(무한 루프에 빠짐)	

	do {
		nx = px[p] + randint(-1, 1);
		ny = py[p] + randint(-1, 1);
	} while (!placable(nx, ny));

	JB_move_tail(p, nx, ny);
}

// back_buf[][]에 기록
void JB_move_tail(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void JB_draw(void) {
	for (int row = 0; row < N_ROW; row++) {
		for (int col = 0; col < N_COL; col++) {
			if (front_buf[row][col] != back_buf[row][col]) {
				front_buf[row][col] = back_buf[row][col];
				printxy(front_buf[row][col], row, col);
			}
		}
	}
}

void JB_print_status(void) {
	printf("round 1, turn: player %d", n_player);
}

void JB_display(void) {
	JB_draw();

	gotoxy(N_ROW + 4, 0);  // 추가로 표시할 정보가 있으면 맵과 상태창 사이의 빈 공간에 출력
	JB_print_status();
}

void jebi(void) {
	JB_sample_init();
	system("cls");
	JB_display();
	char message[] = "message 내용";
	//dialog(message);
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			JB_move_manual(key);
		}

		JB_display();
		Sleep(10);
		tick += 10;
	}
}