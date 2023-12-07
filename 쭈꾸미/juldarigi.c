#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>


#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void J_sample_init(void);
void J_move_manual(key_t key);
void J_move_random(int i, int dir);
void J_move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�

void J_sample_init(void) {
	//map_init(3, 31);
	for (int i = 0; i < ROW_MAX; i++) {
		for (int j = 0; j < COL_MAX; j++) {
			back_buf[i][j] = front_buf[i][j] = ' ';
		}
	}
	N_ROW = 3;
	N_COL = 31;
	for (int i = 0; i < N_ROW; i++) {
		// ���Թ� �̷��� �� �� �ִµ� �Ϻη� �� ����������
		back_buf[i][0] = back_buf[i][N_COL - 1] = '*';

		for (int j = 1; j < N_COL - 1; j++) {
			back_buf[i][j] = (i == 0 || i == N_ROW - 1) ? '*' : ' ';
			if (back_buf[i][15] == '*') {
				back_buf[i][15] = ' ';
			}
		}
	}
	// ��� �� ĭ�� ��(---)�� ǥ��
	for (int i = 1; i < N_ROW-1; i++) {
		back_buf[i][N_COL / 2 - 1] = '-';
		back_buf[i][N_COL / 2] = '-';
		back_buf[i][N_COL / 2 + 1] = '-';
	}

	// �÷��̾���� �ʱ� ��ġ�� �̸� ����
	int initial_positions[PLAYER_MAX][2] = {
		{(1, N_ROW - 2), (1, N_COL / 2 - 2)},   // �÷��̾� 0
		{(1, N_ROW - 2), (N_COL / 2 + 2, N_COL - 14)},   // �÷��̾� 1
		{(1, N_ROW - 2), (1, N_COL / 2 - 3)},   // �÷��̾� 2
		{(1, N_ROW - 2), (N_COL / 2 + 3, N_COL - 13)},   // �÷��̾� 3
		{(1, N_ROW - 2), (1, N_COL / 2 - 4)},   // �÷��̾� 4
		{(1, N_ROW - 2), (N_COL / 2 + 4, N_COL - 12)},   // �÷��̾� 5
		{(1, N_ROW - 2), (1, N_COL / 2 - 5)},   // �÷��̾� 6
		{(1, N_ROW - 2), (N_COL / 2 + 5, N_COL - 11)},   // �÷��̾� 7
		{(1, N_ROW - 2), (1, N_COL / 2 - 6)},   // �÷��̾� 8
		{(1, N_ROW - 2), (N_COL / 2 + 6, N_COL - 10)},   // �÷��̾� 9
		
	};

	for (int i = 0; i < n_player; i++) {
		px[i] = initial_positions[i][0];
		py[i] = initial_positions[i][1];
	

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
	}

	tick = 0;
}

void J_move_manual(key_t key) {
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

	J_move_tail(0, nx, ny);
}

// 0 <= dir < 4�� �ƴϸ� ����
void J_move_random(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�

	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	

	do {
		nx = px[p] + randint(-1, 1);
		ny = py[p] + randint(-1, 1);
	} while (!placable(nx, ny));

	J_move_tail(p, nx, ny);
}

// back_buf[][]�� ���
void J_move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void J_draw(void) {
	for (int row = 0; row < N_ROW; row++) {
		for (int col = 0; col < N_COL; col++) {
			if (front_buf[row][col] != back_buf[row][col]) {
				front_buf[row][col] = back_buf[row][col];
				printxy(front_buf[row][col], row, col);
			}
		}
	}
}

void J_print_status(void) {
	int left_strength = 0;
	int right_strength = 0;
	// �� �÷��̾��� ���� ���� ���� ������ ���� ������ ����
	for (int i = 0; i < n_player; i++) {
		if (i % 2 == 0) { // ¦�� �ε����� ���� ��
			left_strength += player[i].str;
		}
		else { // Ȧ�� �ε����� ������ ��
			right_strength += player[i].str;
		}
	}
	// ��ȿ ���� �� ���
	int total_strength = right_strength- left_strength;

	printf("str:    %d.0\n", total_strength);
	printf("\n");
	printf("no. of players left: %d\n", n_alive);
	for (int p = 0; p < n_player; p++) {
		printf("player %2d: %5s\n",
			p, player[p].is_alive ? "alive" : "DEAD");
	}
}

void J_display(void) {
	J_draw();

	gotoxy(N_ROW + 4, 0);  // �߰��� ǥ���� ������ ������ �ʰ� ����â ������ �� ������ ���
	J_print_status();
}

void juldarigi(void) {
	J_sample_init();
	system("cls");
	J_display();
	char message[] = "message ����";
	//dialog(message);
	while (1) {
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			J_move_manual(key);
		}

		J_display();
		Sleep(10);
		tick += 10;
	}
}