// 2023-2 ������α׷��� ����: �޲ٹ� ����
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "jjuggumi.h"

#define	DATA_FILE	"jjuggumi.dat"

int jjuggumi_init(void);

int intro(void);

int ending(void);

// low �̻� high ���� ������ �߻���Ű�� �Լ�
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

int jjuggumi_init() {
	srand((unsigned int)time(NULL));

	FILE* fp;
	fopen_s(&fp, DATA_FILE, "r");
	if (fp == NULL) {
		return -1;// -1 �����ϸ�����Լ�����ó���ϰ�����
	}

	// �÷��̾����load
	fscanf_s(fp, "%d", &n_player);
	for (int i = 0; i < n_player; i++) {
		// �����ȹ���(����ü������, ��������������)
		PLAYER* p = &player[i];

		// ���Ͽ����������ִ��б�
		fscanf_s(fp, "%s%d%d",
			p->name, (unsigned int)sizeof(p->name),
			&(p->intel), &(p->str));
		p->stamina = 100; // 100%

		// �������
		p->is_alive = true;
		p->hasitem = false;
	}

	// �����۵�����load
	fscanf_s(fp, "%d", &n_item);
	// �÷��̾� ������ 1�� ���� ����
	n_item = (n_player > 1) ? n_player - 1 : 0;

	for (int i = 0; i < n_item; i++) {
		fscanf_s(fp, "%s%d%d%d",
			item[i].name, (unsigned int)sizeof(item[i].name),
			&(item[i].intel_buf),
			&(item[i].str_buf),
			&(item[i].stamina_buf));
	}

	fclose(fp);

	printf("�÷��̾� ��: ");
	scanf_s("%d", &n_player);

	n_alive = n_player;
	for (int i = 0; i < n_player; i++) {
		player[i].is_alive = true;
		player[i].hasitem = false;
	}
	return 0;
}

int intro(void) {
	printf("    _  _                                   _ \n");
	printf("   (_)(_)                                 (_)\n");
	printf("    _  _ _   _  __ _  __ _ _   _ _ __ ___  _ \n");
	printf("   | || | | | |/ _` |/ _` | | | | '_ ` _  | |\n");
	printf("   | || | |_| | (_| | (_| | |_| | | | | | | |\n");
	printf("   | || | __,_| __, | __, | __,_|_| |_| |_|_|\n");
	printf("  _/ |/ |       __/ | __/ |                  \n");
	printf(" |__/__/       |___/ |___/                   \n");
	Sleep(1000);
	system("cls");
	return 0;
}

int ending(void) {
	
	printf("�����: %d", n_alive);
	return 0;
}

int main(void) {
	intro();
	jjuggumi_init();
	//sample();
	mugunghwa();
	nightgame();
	juldarigi();
	jebi();
	ending();
	return 0;
}
