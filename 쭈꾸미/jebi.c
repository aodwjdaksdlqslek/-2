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

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�

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
		// ���Թ� �̷��� �� �� �ִµ� �Ϻη� �� ����������
		back_buf[i][0] = back_buf[i][N_COL - 1] = '*';

		for (int j = 1; j < N_COL - 1; j++) {
			back_buf[i][j] = (i == 0 || i == N_ROW - 1) ? '*' : ' ';
		}
	}
	

	// �÷��̾���� �ʱ� ��ġ�� �̸� ����
	int initial_positions[PLAYER_MAX][2] = {
		{(1, N_ROW - 3), (1, N_COL - 19)},   // �÷��̾� 0
		{(1, N_ROW - 3), (1, N_COL - 17)},   // �÷��̾� 1
		{(1, N_ROW - 3), (1, N_COL - 15)},   // �÷��̾� 2
		{(1, N_ROW - 3), (1, N_COL - 13)},   // �÷��̾� 3
		{(1, N_ROW - 3), (1, N_COL - 11)},   // �÷��̾� 4
		{(1, N_ROW - 3), (1, N_COL - 9)},   // �÷��̾� 5
		{(1, N_ROW - 3), (1, N_COL - 7)},   // �÷��̾� 6
		{(1, N_ROW - 3), (1, N_COL - 5)},   // �÷��̾� 7
		{(1, N_ROW - 3), (1, N_COL - 3)},   // �÷��̾� 8
		{(1, N_ROW - 3), (1, N_COL - 1)},   // �÷��̾� 9

	};

	for (int i = 0; i < n_player; i++) {
		px[i] = initial_positions[i][0];
		py[i] = initial_positions[i][1];


		back_buf[px[i]][py[i]] = '?';
	}

	tick = 0;
}

void JB_move_manual(key_t key) {
	// �� �������� ������ �� x, y�� delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // ������ ����(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// �������� ���� �ڸ�
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	JB_move_tail(0, nx, ny);
}

// 0 <= dir < 4�� �ƴϸ� ����
void JB_move_random(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�

	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	

	do {
		nx = px[p] + randint(-1, 1);
		ny = py[p] + randint(-1, 1);
	} while (!placable(nx, ny));

	JB_move_tail(p, nx, ny);
}

// back_buf[][]�� ���
void JB_move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
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

	gotoxy(N_ROW + 4, 0);  // �߰��� ǥ���� ������ ������ �ʰ� ����â ������ �� ������ ���
	JB_print_status();
}

void jebi(void) {
	JB_sample_init();
	system("cls");
	JB_display();
	char message[] = "message ����";
	//dialog(message);
	while (1) {
		// player 0�� ������ ������(4����)
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