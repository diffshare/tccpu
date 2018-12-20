#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tccpu_minmax.h"

TCCPU_MinMax::TCCPU_MinMax() {
	const int e[38] = {
		0, -5, -5, -20, 10,  30,  30,  50,
		0, -1, -1, -10,  5,  10,  10,  40,
		0, -1,     -10,  5,  10,  10,  40,
		0, -1, -1,       1,   5,   5,  40,
		0, -5, -5, -10,  0, -30, -30, -50
	};
	int i = 0;
	for(int j = 0; j < 8; j++) {
		for(int k = 0; k < 16; k++) {
			if(
				   j == 0 || j == 2 || j == 7
				|| k == 0 || k == 2 || k == 7 || k == 8 || k == 10 || k == 15
				|| (j == 4 && k == 4) || (j == 5 && k == 5) || (j == 6 && k == 6)
			) { // ナイトと未使用箇所
				eval_table[j][k] = 0;
				eval_table[j + 8][k] = 0;
			} else if(k == 6 || k == 14) { // キング
				eval_table[j][k] = (k < 8 ? -65535 : 65535);
				eval_table[j + 8][k] = (k < 8 ? -65535 : 65535);
			} else {
				eval_table[j][k] = e[i];
				eval_table[j ^ 8][k ^ 8] = -e[i];
				i++;
			}
		}
	}
	srand((unsigned int) time(NULL));
}

TCCPU_MinMax::~TCCPU_MinMax() {
}

void TCCPU_MinMax::init() {
	score = 0;
}

// スコア (相対値) を返す
int TCCPU_MinMax::calc(TCField *field, MOVE move) {
	int s = 0;
	t_field fr = field -> getField(move.x1, move.y1);
	t_field to = field -> getField(move.x2, move.y2);
	TURN t = F_TOPTURN(to); // 動かした駒の先手 or 後手
	int *ev = eval_table[F_TOPPIECE(to)];

	// FROM
	while(F_TOPPIECE(fr) != P_BLANK) {
		s -= ev[F_TOPPIECE(fr)];
		if(F_TOPTURN(fr) == t) { break; }
		fr = fr >> PIECE_BIT;
	}

	// TO
	to = to >> PIECE_BIT; // 動かした駒は一旦取り除く
	while(F_TOPPIECE(to) != P_BLANK) {
		s += ev[F_TOPPIECE(to)];
		if(F_TOPTURN(to) == t) { break; }
		to = to >> PIECE_BIT;
	}

	return s;
}

int TCCPU_MinMax::move_s(TCField *field, MOVE move) {
	field -> move(move); // 移動
	return calc(field, move); // 移動後のスコア (相対値) を返す
}

int TCCPU_MinMax::maxOfMin(TCField *field, int depth, MOVE **p_move) {
	int max = -100000;
	MOVE *move_org = (p_move ? *p_move : NULL);

	// 移動可能な手を全て列挙
	MOVE m[1024];
	int count = field -> listup(m, TURN_FIRST);

	for(int i = 0; i < count; i++) {
		// 移動
		int s = move_s(field, m[i]);
		if(s > 60000) {
			// キングを取る手が見つかったら探索打ち切り
			field -> move_inv(m[i]);
			if(p_move) { *move_org = m[i]; *p_move = move_org + 1; }
			return s;
		}
		if(depth > 0) {
			s += minOfMax(field, depth - 1, NULL);
		}
		if(max < s) {
			max = s;
			if(p_move) { *p_move = move_org; }
		}
		if(p_move && max == s) {
			**p_move = m[i];
			(*p_move)++;
		}

		// 移動前に戻す
		field -> move_inv(m[i]);
	}

	return max; // 評価値の最大値を返す
}

int TCCPU_MinMax::minOfMax(TCField *field, int depth, MOVE **p_move) {
	int min = 100000;
	MOVE *move_org = (p_move ? *p_move : NULL);

	// 移動可能な手を全て列挙
	MOVE m[1024];
	int count = field -> listup(m, TURN_SECOND);

	for(int i = 0; i < count; i++) {
		// 移動
		int s = move_s(field, m[i]);
		if(s < -60000) {
			// キングを取る手が見つかったら探索打ち切り
			field -> move_inv(m[i]);
			if(p_move) { *move_org = m[i]; *p_move = move_org + 1; }
			return s;
		}
		if(depth > 0) {
			s += maxOfMin(field, depth - 1, NULL);
		}
		if(min > s) {
			min = s;
			if(p_move) { *p_move = move_org; }
		}
		if(p_move && min == s) {
			**p_move = m[i];
			(*p_move)++;
		}

		// 移動前に戻す
		field -> move_inv(m[i]);
	}

	return min; // 評価値の最小値を返す
}

MOVE TCCPU_MinMax::turn(TCField *field, TURN turn) {
	MOVE move[1024];
	MOVE *m = move;

	if(turn == TURN_FIRST) {
		maxOfMin(field, MAX_DEPTH, &m);
	} else {
		minOfMax(field, MAX_DEPTH, &m);
	}

	if(m - move > 0) {
		return move[0];
	} else {
		// 手が見つからなかったときギブアップ (通常はあり得ないはず)
		m[0].x1 = -1; m[0].y1 = -1; m[0].x2 = -1; m[0].y2 = -1;
		return m[0];
	}
}

void TCCPU_MinMax::turnend(TCField *field, MOVE move) {
	printf("sore : %d -> ", score);
	score += calc(field, move);
	printf("%d\n", score);
}
