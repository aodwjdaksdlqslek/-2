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

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
// 주어진 두 점 사이의 거리 계산
double distance(int x1, int y1, int x2, int y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void N_sample_init(void) {
	map_init(15, 40);
	int x, y;
	int ix, iy;
	for (int i = 0; i < n_player; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
	}
	// 아이템을 랜덤하게 배치
	for (int i = 0; i < n_item; i++) {
		int ix, iy;  // 아이템 위치
		// 같은 자리가 나오거나 플레이어와 겹치면 다시 생성
		do {
			ix = randint(1, N_ROW - 2);
			iy = randint(1, N_COL - 2);
		} while (!placable(ix, iy) || back_buf[ix][iy] != ' ');

		back_buf[ix][iy] = 'I';  // 'I'로 아이템 표시
	}

	tick = 0;
}

void N_move_manual(key_t key) {
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

	N_move_tail(0, nx, ny);
	
}

// 각 플레이어가 가까운 아이템으로 이동하는 함수
void move_toward_closest_item(int player) {
	int p = player;
	int target_x=0, target_y=0;  // 아이템 위치
	int nx, ny;  // 움직여서 다음에 놓일 자리

	// 초기 거리를 무한대로 설정
	double min_distance = DBL_MAX;

	// 모든 아이템에 대해 플레이어와의 거리를 계산하여 가장 가까운 아이템 찾기
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			if (back_buf[i][j] == 'I') {
				// 아이템 위치 찾음
				double dist = distance(px[p], py[p], i, j);
				if (dist < min_distance) {
					min_distance = dist;
					target_x = i;
					target_y = j;
				}
			}
		}
	}

	// 아이템으로 향해 이동
	int dx = target_x - px[p];
	int dy = target_y - py[p];

	// x 방향으로 먼저 이동
	if (dx > 0) {
		nx = px[p] + 1;
	}
	else if (dx < 0) {
		nx = px[p] - 1;
	}
	else {
		nx = px[p];
	}

	// y 방향으로 이동
	if (dy > 0) {
		ny = py[p] + 1;
	}
	else if (dy < 0) {
		ny = py[p] - 1;
	}
	else {
		ny = py[p];
	}

	// 이동 가능한지 확인하고 이동
	if (placable(nx, ny)) {
		N_move_tail(p, nx, ny);

	}
}

// back_buf[][]에 기록
void N_move_tail(int player_index, int nx, int ny) {

	PLAYER* p = &player[player_index];

	if (back_buf[nx][ny] == 'I') {
		// 플레이어가 아이템 위치에 도달하면 아이템을 획득하고 해당 위치에 빈 칸으로 표시
		back_buf[nx][ny] = ' ';

		// 아이템을 획득한 경우 추가 작업 수행 가능
	   // 예: 플레이어의 상태 변경, 점수 증가 등

	   // 플레이어 구조체의 hasitem을 true로 설정
		p->hasitem = true;

		//다이얼로그 표시
		dialog(" 아이템 획득", player_index);
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
				// 남아있는 아이템이 있으면 아이템이 아직 남아있다고 판단
				return false;
			}
		}
	}
	// 모든 아이템이 획득되었다고 판단
	return true;
}

void nightgame(void) {
	N_sample_init();
	system("cls");
	display();
	char message[] = "아이템 획득";
	//dialog(message);
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manual(key);
		}

		// 각 플레이어들이 아이템으로 향해 이동
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0) {
				move_toward_closest_item(i);
			}
		}
		display();
		Sleep(10);
		tick += 10;

		// 게임 종료 조건 확인
		if (all_items_collected()) {
			char end_message[] = "모든 아이템을 획득";
			dialog(end_message);
			break;
		}
	}
}