#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <Windows.h>
#include <conio.h>

#include <process.h>

#define DIR_UP      0
#define DIR_DOWN   1
#define DIR_LEFT   2
#define DIR_RIGHT   3

unsigned int _stdcall Loop_with_Thread(void* args);
unsigned int _stdcall M_Move_User(LPVOID args);
void M_sample_init(void);
void M_move_manual(key_t key);
void M_move_random(int i, int dir);
void M_move_tail(int i, int nx, int ny);
char M_status_string[100] = "";
bool M_is_vulnerable(int p, int x, int y);
// user_exit, eval_time, tmp_tick, tick, finished
int status_args[5] = { 0, 0, 0, 0, 0 };
int player_status[PLAYER_MAX] = { 0, };
CRITICAL_SECTION cs;
int check_status(int p);

int check_status(int stat) {
    int tmp = 0;
    for (int i = 0; i < n_player; i++) {
        if (player_status[i] == stat) { tmp++; }
    }
    return tmp;
}

void M_gotoxy(int row, int col) {
    COORD pos = { col,row };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�

void M_sample_init(void) {
    map_init(15, 40);
    for (int i = 0; i < 3; i++) { back_buf[6 + i][1] = '#'; }
    for (int i = 0; i < n_player; i++) {
        px[i] = 3 + i;
        py[i] = 38;
        period[i] = randint(50, 100);

        back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
    }

    tick = 0;
}

void M_move_manual(key_t key) {
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

    if (status_args[1] == 1) {
        if (M_is_vulnerable(0, px[0], py[0])) {
            player[0].is_alive = false;
            player_status[0] = 1;
            back_buf[px[0]][py[0]] = ' ';
            n_alive--;
            return;
        }
        else { M_move_tail(0, nx, ny); }
    }
    else { M_move_tail(0, nx, ny); }
}

// 0 <= dir < 4�� �ƴϸ� ����
void M_move_random(int p, int dir) {
    if (!player[p].is_alive || player_status[p] != 0) { return; }
    int nx, ny;  // �������� ������ ���� �ڸ�

    // ������ Ȯ���� ����
    int move_prob = randint(1, 100);
    if (move_prob <= 70) {
        // 70% Ȯ���� �������� �̵�
        nx = px[p];
        ny = py[p] - 1;
    }
    else if (move_prob <= 80) {
        // 10% Ȯ���� ���� �̵�
        nx = px[p] - 1;
        ny = py[p];
    }
    else if (move_prob <= 90) {
        // 10% Ȯ���� �Ʒ��� �̵�
        nx = px[p] + 1;
        ny = py[p];
    }
    else {
        // 10% Ȯ���� ���ڸ��� �ӹ�����
        nx = px[p];
        ny = py[p];
    }
    // ��ġ�� �ʴ��� Ȯ��
    if (!placable(nx, ny)) {
        return;
    }

    if (status_args[1] == 1) {
        if (M_is_vulnerable(p, px[0], py[0])) {
            player[p].is_alive = false;
            player_status[p] = 1;
            back_buf[px[p]][py[p]] = ' ';
            n_alive--;
            return;
        }
        else { M_move_tail(p, nx, ny); }
    }
    else { M_move_tail(p, nx, ny); }
}

bool M_is_vulnerable(int p, int x, int y) {
    char tmp = '*';
    for (int i = 0; i < N_COL; i++) {
        tmp = back_buf[x][i];
        if ((tmp >= '0') && (tmp <= '0' + n_player)) {
            if (i == x || (tmp == '0' + p && player_status[p] == 0)) { return true; }
            else { return false; }
        }
    }
    return false;
}

// back_buf[][]�� ���
void M_move_tail(int p, int nx, int ny) {

    if ((nx == 5 && ny == 1) || (nx == 9 && ny == 1) || (nx == 6 && ny == 2) || (nx == 7 && ny == 2) || (nx == 8 && ny == 2)) {
        back_buf[nx][ny] = ' ';  // �ش� ��ġ�� �ִ� �÷��̾ ������� ����ϴ�.
        player_status[p] = 2;
    }
    else {
        back_buf[nx][ny] = back_buf[px[p]][py[p]];
    }
    back_buf[px[p]][py[p]] = ' ';
    px[p] = nx;
    py[p] = ny;
}

void M_Younghi_sample(void) {
    for (int i = 0; i < 3; i++) {
        M_gotoxy(6 + i, 1);
        printf("#");
    }
}

void M_draw(void) {
    for (int row = 0; row < N_ROW; row++) {
        for (int col = 0; col < N_COL; col++) {
            if (front_buf[row][col] != back_buf[row][col]) {
                front_buf[row][col] = back_buf[row][col];
                printxy(front_buf[row][col], row, col);
            }
        }
    }
}

void M_print_status(void) {
    printf("no. of players left: %d\n", n_alive);
    printf("                  intl       str       stm\n");
    for (int p = 0; p < n_player; p++) {
        printf("player %2d: %5s   %d(+%d)     %d(+%d)     %d\n", 
            p, player[p].is_alive ? "alive" : "DEAD",
            player[p].intel, item[p].intel_buf,
            player[p].str,item[p].str_buf,
            player[p].stamina);
    }
}

void say_mugunghwa(void) {
    M_gotoxy(N_ROW, 0);
    const char* text1 = "�� �� ȭ �� �� ";
    const char* text2 = "�� �� �� �� ��.";

    for (int i = 0; text1[i] != '\0'; i++) {
        M_gotoxy(N_ROW, i);
        printf("%c", text1[i]);
        Sleep(500);
    }

    for (int i = 0; text2[i] != '\0'; i++) {
        M_gotoxy(N_ROW, i);
        printf("%c", text2[i]);
        Sleep(200);
    }

    for (int i = 0; i < 3; i++) {
        M_gotoxy(6 + i, 1);
        printf("@");
    }


    Sleep(3000);
    M_gotoxy(N_ROW, 0);
    printf("                                ");

}



/* Simultaneous movement generating */
unsigned int _stdcall Loop_with_Thread(void* args) {
    bool tmp = false;
    const char* str_0 = "�� �� ȭ �� �� ";
    const char* str_1 = "�� �� �� �� ��.";
    int len = (int)strlen(str_0);

    while (n_alive > 1 && check_status(1) + check_status(2) < n_player) {
        for (int i = 0; str_0[i] != '\0'; i++) {
            //EnterCriticalSection(&cs);
            if (str_0[i] == ' ') {
                M_status_string[i] = str_0[i];
            }
            else if (tmp == true) {
                M_status_string[i - 1] = str_0[i - 1];
                M_status_string[i] = str_0[i];
                tmp = false;
            }
            else {
                tmp = true;
            }
            //LeaveCriticalSection(&cs);
            Sleep(500);
        }

        for (int i = 0; str_1[i] != '\0'; i++) {
            //EnterCriticalSection(&cs);
            //M_status_string[len + 1 + i] = str_1[i];
            if (str_0[i] == ' ') {
                M_status_string[len + i] = str_1[i];
            }
            else if (tmp == true) {
                M_status_string[len + i - 1] = str_1[i - 1];
                M_status_string[len + i] = str_1[i];
                tmp = false;
            }
            else {
                tmp = true;
            }
            //LeaveCriticalSection(&cs);
            Sleep(200);
        }

        for (int i = 0; i < 3; i++) { back_buf[6 + i][1] = '@'; }
        status_args[1] = 1;
        Sleep(3000);
        status_args[1] = 0;
        for (int i = 0; i < 3; i++) { back_buf[6 + i][1] = '#'; }

        for (int i = 0; i < 99; i++) { M_status_string[i] = ' '; }
        M_status_string[99] = '\0';
    }
    ExitThread(0);
}


void M_display(void) {
    M_draw();
    //M_Younghi_sample();
    gotoxy(N_ROW, 0);
    //EnterCriticalSection(&cs);
    printf("%s", M_status_string);
    //LeaveCriticalSection(&cs);
    gotoxy(N_ROW + 4, 0);  // �߰��� ǥ���� ������ ������ �ʰ� ����â ������ �� ������ ���
    M_print_status();
}
unsigned int _stdcall M_Move_User(LPVOID args) {
    while (player[0].is_alive) {
        // player 0�� ������ ������(4����)
        if (_kbhit()) {
            key_t key = get_key();
            if (key == K_QUIT) {
                status_args[0] = 1;
                break;
            }
            else if (key != K_UNDEFINED) {
                M_move_manual(key);
            }
        }
    }
    ExitThread(0);
}
void mugunghwa(void) {
    InitializeCriticalSection(&cs);
    memset(M_status_string, '\0', 100 * sizeof(char));
    M_sample_init();
    system("cls");

    M_display();
    HANDLE hThread = CreateThread(NULL, 0, Loop_with_Thread, NULL, 0, NULL);
    HANDLE userThread = CreateThread(NULL, 0, M_Move_User, NULL, 0, NULL);
    while (status_args[0] == 0 && n_alive > 1) {
        // player 1 ���ʹ� �������� ������(8����)
        if (status_args[1] == 0) {
            for (int i = 1; i < n_player; i++) {
                if (tick % period[i] == 0) {
                    M_move_random(i, -1);
                }
            }
        }
        M_display();

        //status_args[2] = tick;
        Sleep(10);
        tick += 10;
        if (check_status(1) + check_status(2) >= n_alive) { break; }
    }
    CloseHandle(hThread);
    CloseHandle(userThread);
}