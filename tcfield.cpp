#include <stdio.h>
#include "tcfield.h"

TCField::TCField() {
	// フィールド初期化
	t_field *p = field;
	for(int i = 0; i < F_WIDTH * F_HEIGHT; i++) {
		*p = 0x0000000000000000ull;
		p++;
	}

	// 初期盤面セット (面倒臭いから、ベタ書きで)
	for(int x = 0; x < F_WIDTH; x++) {
		field[1 * F_WIDTH + x] = P_A_PAWN;
		field[4 * F_WIDTH + x] = P_B_PAWN;
	}
	field[0 * F_WIDTH + 0] = P_A_ROOK;
	field[0 * F_WIDTH + 5] = P_A_ROOK;
	field[5 * F_WIDTH + 0] = P_B_ROOK;
	field[5 * F_WIDTH + 5] = P_B_ROOK;
	field[0 * F_WIDTH + 1] = P_A_BISHOP;
	field[0 * F_WIDTH + 4] = P_A_BISHOP;
	field[5 * F_WIDTH + 1] = P_B_BISHOP;
	field[5 * F_WIDTH + 4] = P_B_BISHOP;
	field[0 * F_WIDTH + 2] = P_A_QUEEN;
	field[5 * F_WIDTH + 2] = P_B_QUEEN;
	field[0 * F_WIDTH + 3] = P_A_KING;
	field[5 * F_WIDTH + 3] = P_B_KING;

	judge = JUDGE_PLAYING;
}

TCField::TCField(TCField *f) {
	for(int y = 0; y < F_HEIGHT; y++) {
		for(int x = 0; x < F_WIDTH; x++) {
			field[y * F_WIDTH + x] = f -> getField(x, y);
		}
	}
	judge = f -> getJudge();
}

TCField::~TCField() {
}

TCField *TCField::clone() {
	return new TCField(this);
}

// デバッグ出力
void TCField::out() {
	const char str[] = " pnrbqk??PNRBQK?";

	// 段数の最大値を計算
	int max_h = 0;
	t_field *p = field;
	for(int i = 0; i < F_WIDTH * F_HEIGHT; i++) {
		t_field tmp = *p;
		int h = 0;
		while(F_TOPPIECE(tmp) != P_BLANK) { tmp = tmp >> PIECE_BIT; h++; }
		if(max_h < h) { max_h = h; }
		p++;
	}

	// 左手前が a1, 右にいくにつれて abcdef, 上にいくにつれて 123456
	// a6, b6, … , f6, a5, b5, … の順で表示
	t_field *p0 = &(field[F_WIDTH * (F_HEIGHT - 1)]);
	for(int y = F_HEIGHT - 1; y >= 0; y--) {
		t_field *p = p0;
		printf("%d ", y + 1);
		for(int x = 0; x < F_WIDTH; x++) {
			t_field tmp = *p;
			for(int h = (max_h - 1); h >= 1; h--) {
				printf("%c", str[F_TOPPIECE(tmp >> PIECE_BIT * h)]);
			}
			printf("[%c] ", str[F_TOPPIECE(tmp)]);
			p++;
		}
		printf("\n");
		p0 -= F_WIDTH;
	}
	for(int x = 0; x < F_WIDTH; x++) {
		for(int h = 0; h < max_h + 2; h++) { printf(" "); }
		printf("%c", 'a' + x);
	}
	printf("\n");
	printf("\n");
}

// フィールド情報を取得
t_field TCField::getField(int x, int y) {
	return field[y * F_WIDTH + x];
}

// 移動
void TCField::move(int x1, int y1, int x2, int y2) {
	if(x1 < 0 || x1 >= F_WIDTH || y1 < 0 || y1 >= F_HEIGHT || x2 < 0 || x2 >= F_WIDTH || y2 < 0 || y2 >= F_HEIGHT) {
		printf("invalid move!! (%d, %d) -> (%d, %d)\n", x1, y1, x2, y2);
		return;
	}

	t_field *fr = &(field[y1 * F_WIDTH + x1]);
	t_field *to = &(field[y2 * F_WIDTH + x2]);

	t_field p = F_TOPPIECE(*fr);
	if(p == 0x0) {
		printf("invalid move!! (%d, %d) -> (%d, %d)\n", x1, y1, x2, y2);
		return;
	}

	// 移動前に勝敗を判定
	if(F_IS_KING(*fr)) {
		judge = JUDGE_PLAYING;
	}
	if(F_IS_KING(*to)) {
		judge = (F_TOPTURN(*to) == TURN_FIRST ? JUDGE_A_WON : JUDGE_B_WON);
	}

	// 移動
	*fr = (*fr >> PIECE_BIT);
	*to = (*to << PIECE_BIT) | p;
}

// 勝敗を返す
int TCField::getJudge() {
	return judge;
}

// 指定座標のチェック
// 戻り値 … -1 : 無効, 0 : 空白, 1 : 空白以外
int TCField::check(TURN t, t_field *f, int x, int y) {
	// 範囲外の座標チェック
	if(x < 0 || x >= F_WIDTH || y < 0 || y >= F_HEIGHT) { return -1; }
	// 味方のキングの上には乗れない
	if(F_IS_KING(*f) && F_TOPTURN(*f) == t) { return -1; }

	return (F_TOPPIECE(*f) ? 1 : 0);
}

// 合法手リストに追加
void TCField::addMoveList(MOVE *m, int x1, int y1, int x2, int y2) {
	(*m).x1 = x1;
	(*m).y1 = y1;
	(*m).x2 = x2;
	(*m).y2 = y2;
}

// 合法手リストに追加 …を同じ方向に繰り返す
MOVE *TCField::addMoveListLoop(MOVE *m, TURN t, t_field *f, int x1, int y1, int dx, int dy) {
	int x2 = x1 + dx, y2 = y1 + dy;
	int df = dy * F_WIDTH + dx;
	f += df;
	int c;

	while((c = check(t, f, x2, y2)) != -1) {
		addMoveList(m++, x1, y1, x2, y2);
		if(c == 1) { break; }
		x2 += dx;
		y2 += dy;
		f += df;
	}

	return m; // NOTE: リストの末尾 +1 のアドレスを返す
}

// 合法手を列挙
int TCField::listup(MOVE *movelist, TURN turn) {
	MOVE *m = movelist;

	int x = 0, y = 0;
	t_field *f = field;

	while(1) {
		if(F_TOPTURN(*f) == turn) {
			switch(F_TOPPIECE(*f)) {
			case P_A_PAWN: case P_B_PAWN:
				{
					int iy = (turn == TURN_FIRST ? 1 : 4);
					int dy = (turn == TURN_FIRST ? 1 : -1);
					int df = (turn == TURN_FIRST ? F_WIDTH : -F_WIDTH);

					// 斜め
					if(check(turn, (f + df - 1), x - 1, y + dy) == 1) {
						addMoveList(m++, x, y, x - 1, y + dy);
					}
					if(check(turn, (f + df + 1), x + 1, y + dy) == 1) {
						addMoveList(m++, x, y, x + 1, y + dy);
					}
					// 正面
					if(check(turn, (f + df), x, y + dy) != 0) { break; }
					addMoveList(m++, x, y, x, y + dy);
					if(y != iy || check(turn, (f + 2 * df), x, y + 2 * dy) != 0) { break; }
					addMoveList(m++, x, y, x, y + 2 * dy);
				}
				break;

			case P_A_ROOK: case P_B_ROOK:
				m = addMoveListLoop(m, turn, f, x, y,  1,  0);
				m = addMoveListLoop(m, turn, f, x, y,  0,  1);
				m = addMoveListLoop(m, turn, f, x, y, -1,  0);
				m = addMoveListLoop(m, turn, f, x, y,  0, -1);
				break;

			case P_A_BISHOP: case P_B_BISHOP:
				m = addMoveListLoop(m, turn, f, x, y,  1,  1);
				m = addMoveListLoop(m, turn, f, x, y, -1,  1);
				m = addMoveListLoop(m, turn, f, x, y, -1, -1);
				m = addMoveListLoop(m, turn, f, x, y,  1, -1);
				break;

			case P_A_QUEEN: case P_B_QUEEN:
				m = addMoveListLoop(m, turn, f, x, y,  1,  0);
				m = addMoveListLoop(m, turn, f, x, y,  1,  1);
				m = addMoveListLoop(m, turn, f, x, y,  0,  1);
				m = addMoveListLoop(m, turn, f, x, y, -1,  1);
				m = addMoveListLoop(m, turn, f, x, y, -1,  0);
				m = addMoveListLoop(m, turn, f, x, y, -1, -1);
				m = addMoveListLoop(m, turn, f, x, y,  0, -1);
				m = addMoveListLoop(m, turn, f, x, y,  1, -1);
				break;

			case P_A_KING: case P_B_KING:
				if(check(turn, (f - F_WIDTH - 1), x - 1, y - 1) != -1) { addMoveList(m++, x, y, x - 1, y - 1); }
				if(check(turn, (f - F_WIDTH    ), x    , y - 1) != -1) { addMoveList(m++, x, y, x    , y - 1); }
				if(check(turn, (f - F_WIDTH + 1), x + 1, y - 1) != -1) { addMoveList(m++, x, y, x + 1, y - 1); }
				if(check(turn, (f           - 1), x - 1, y    ) != -1) { addMoveList(m++, x, y, x - 1, y    ); }
				if(check(turn, (f           + 1), x + 1, y    ) != -1) { addMoveList(m++, x, y, x + 1, y    ); }
				if(check(turn, (f + F_WIDTH - 1), x - 1, y + 1) != -1) { addMoveList(m++, x, y, x - 1, y + 1); }
				if(check(turn, (f + F_WIDTH    ), x    , y + 1) != -1) { addMoveList(m++, x, y, x    , y + 1); }
				if(check(turn, (f + F_WIDTH + 1), x + 1, y + 1) != -1) { addMoveList(m++, x, y, x + 1, y + 1); }
				break;
			}
		}

		x++;
		if(x >= F_WIDTH) { x = 0; y++; }
		if(y >= F_HEIGHT) { break; }
		f++;
	}

	// 見つかった手数を返す (NOTE: 面倒くさいのでポインタ同士の引き算で)
	return (m - movelist);
}

// 合法手かどうかチェック
bool TCField::check(int x1, int y1, int x2, int y2, TURN turn) {
	MOVE m[1024];
	int c = listup(m, turn);
	for(int i = 0; i < c; i++) {
		if(x1 == m[i].x1 && y1 == m[i].y1 && x2 == m[i].x2 && y2 == m[i].y2) {
			return true;
		}
	}

	return false;
}
