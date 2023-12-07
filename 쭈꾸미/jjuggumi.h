#ifndef _JJUGGUMI_H_
#define _JJUGGUMI_H_

#include <Windows.h>
#include <stdbool.h>

#define PLAYER_MAX		10
#define ITEM_MAX		10

typedef struct { 
	char name[100]; 
	int intel_buf, str_buf, stamina_buf; 
	bool hasitem; // 아이템이 소지되었는지 여부 
} ITEM; 

ITEM item[ITEM_MAX];

typedef struct {
	int id; 
	char name[100];
	
	// 능력치:지능, 힘, 스태미나
	int intel, str,stamina;
	
	// 현재상태
	bool is_alive;  // 탈락했으면false
	bool hasitem;   // 아이템이있으면true
	ITEM item;      // 아이템1개장착가능
	bool wants_to_exchange;  // 교환 여부
} PLAYER;

PLAYER player[PLAYER_MAX];
int n_player, n_alive, n_item;
int tick;  // 시계

// 미니게임
void sample(void);
void mugunghwa(void);
void nightgame(void);
void juldarigi(void);
void jebi(void);

int randint(int low, int high);


#endif

