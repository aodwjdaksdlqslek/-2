#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>
#include <Windows.h>
#include <math.h>
#include <conio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void N_sample_init(void);
void N_move_manual(key_t key);
void N_move_random(int i, int dir);
void N_move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�
// �־��� �� �� ������ �Ÿ� ���
double distance(int x1, int y1, int x2, int y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void N_sample_init(void) {
	map_init(15, 40);
	int x, y;
	int ix, iy;
	for (int i = 0; i < n_player; i++) {
		// ���� �ڸ��� ������ �ٽ� ����
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
	}
	// �������� �����ϰ� ��ġ
	for (int i = 0; i < n_item; i++) {
		int ix, iy;  // ������ ��ġ
		// ���� �ڸ��� �����ų� �÷��̾�� ��ġ�� �ٽ� ����
		do {
			ix = randint(1, N_ROW - 2);
			iy = randint(1, N_COL - 2);
		} while (!placable(ix, iy) || back_buf[ix][iy] != ' ');

		back_buf[ix][iy] = 'I';  // 'I'�� ������ ǥ��
	}

	tick = 0;
}

void N_move_manual(key_t key) {
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

	N_move_tail(0, nx, ny);
	
}

// �� �÷��̾ ����� ���������� �̵��ϴ� �Լ�
void move_toward_closest_item(int player) {
	int p = player;
	int target_x=0, target_y=0;  // ������ ��ġ
	int nx, ny;  // �������� ������ ���� �ڸ�

	// �ʱ� �Ÿ��� ���Ѵ�� ����
	double min_distance = DBL_MAX;

	// ��� �����ۿ� ���� �÷��̾���� �Ÿ��� ����Ͽ� ���� ����� ������ ã��
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			if (back_buf[i][j] == 'I') {
				// ������ ��ġ ã��
				double dist = distance(px[p], py[p], i, j);
				if (dist < min_distance) {
					min_distance = dist;
					target_x = i;
					target_y = j;
				}
			}
		}
	}

	// ���������� ���� �̵�
	int dx = target_x - px[p];
	int dy = target_y - py[p];

	// x �������� ���� �̵�
	if (dx > 0) {
		nx = px[p] + 1;
	}
	else if (dx < 0) {
		nx = px[p] - 1;
	}
	else {
		nx = px[p];
	}

	// y �������� �̵�
	if (dy > 0) {
		ny = py[p] + 1;
	}
	else if (dy < 0) {
		ny = py[p] - 1;
	}
	else {
		ny = py[p];
	}

	// �̵� �������� Ȯ���ϰ� �̵�
	if (placable(nx, ny)) {
		N_move_tail(p, nx, ny);

	}
}

// back_buf[][]�� ���
void N_move_tail(int player_index, int nx, int ny) {

	PLAYER* p = &player[player_index];

	if (back_buf[nx][ny] == 'I') {
		// �÷��̾ ������ ��ġ�� �����ϸ� �������� ȹ���ϰ� �ش� ��ġ�� �� ĭ���� ǥ��
		back_buf[nx][ny] = ' ';

		// �������� ȹ���� ��� �߰� �۾� ���� ����
	   // ��: �÷��̾��� ���� ����, ���� ���� ��

	   // �÷��̾� ����ü�� hasitem�� true�� ����
		p->hasitem = true;

		//���̾�α� ǥ��
		dialog(" ������ ȹ��", player_index);
	}

	back_buf[nx][ny] = back_buf[px[player_index]][py[player_index]];
	back_buf[px[player_index]][py[player_index]] = ' ';
	px[player_index] = nx;
	py[player_index] = ny;
}

bool all_items_collected(void) {
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			if (back_buf[i][j] == 'I') {
				// �����ִ� �������� ������ �������� ���� �����ִٰ� �Ǵ�
				return false;
			}
		}
	}
	// ��� �������� ȹ��Ǿ��ٰ� �Ǵ�
	return true;
}

void nightgame(void) {
	N_sample_init();
	system("cls");
	display();
	char message[] = "������ ȹ��";
	//dialog(message);
	while (1) {
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manual(key);
		}

		// �� �÷��̾���� ���������� ���� �̵�
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0) {
				move_toward_closest_item(i);
			}
		}
		display();
		Sleep(10);
		tick += 10;

		// ���� ���� ���� Ȯ��
		if (all_items_collected()) {
			char end_message[] = "��� �������� ȹ��";
			dialog(end_message);
			break;
		}
	}
}