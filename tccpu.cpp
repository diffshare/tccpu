#include <stdio.h>
#include "tcfield.h"
#include "tccpu_human.h"
#include "tccpu_random.h"
#include "tccpu_minmax.h"

void usage() {
	printf("usage: [-q] player1 player2\n");
	printf("-q\n");
	printf("    quiet mode\n");
	printf("player1, player2\n");
	printf("    h : human\n");
	printf("    m : minmax\n");
	printf("    r : random\n");

	return;
}

int main(int argc, char *argv[]) {
	int quiet = false;
	int pl[2];
	int i = 1, p = 0;

	// 引数チェック
	while(i < argc && argv[i][0] == '-') {
		if(argv[i][1] == 'q') { quiet = true; }
		i++;
	}
	if(argc - i < 2) { usage(); return 1; }
	while(i < argc) {
		switch(argv[i][0]) {
		case 'h': pl[p++] = 0; break; // human
		case 'r': pl[p++] = 1; break; // random
		case 'm': pl[p++] = 2; break; // minmax
		default:  usage(); return 1;
		}
		i++;
	}

	// フィールド・プレーヤークラスを初期化
	TCField *tcfield = new TCField();

	TCCPUBase *tccpu[PLAYERS];
	for(int i = 0; i < PLAYERS; i++) {
		switch(pl[i]) {
		case 0: tccpu[i] = new TCCPU_Human(); break;
		case 1: tccpu[i] = new TCCPU_Random(); break;
		case 2: tccpu[i] = new TCCPU_MinMax(); break;
		}
		tccpu[i] -> init();
	}

	// 試合開始
	if(!quiet) { tcfield -> out(); }
	TURN turn = TURN_FIRST;
	while(1) {
		MOVE m;
		do {
			TCField *f = tcfield -> clone();
			m = tccpu[turn] -> turn(f, turn);
			delete f;
			if(m.x1 < 0) { break; }
		} while(!(tcfield -> check(m, turn)));
		if(m.x1 < 0) { break; }

		tcfield -> move(m);
		if(!quiet && pl[turn] != 0) { printf("%c%d -> %c%d\n", 'a' + m.x1, m.y1 + 1, 'a' + m.x2, m.y2 + 1); }
		if(!quiet) { tcfield -> out(); }
		if(tcfield -> getJudge() != JUDGE_PLAYING) { break; }

		turn = (turn == TURN_FIRST ? TURN_SECOND : TURN_FIRST);
		for(int i = 0; i < PLAYERS; i++) {
			TCField *f = tcfield -> clone();
			tccpu[i] -> turnend(f, m);
			delete f;
		}
	}

	// 後始末
	for(int i = 0; i < PLAYERS; i++) { delete tccpu[i]; }
	delete tcfield;

	return 0;
}
