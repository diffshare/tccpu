#ifndef __TCFIELD_H__
#define __TCFIELD_H__

// 駒の種類
typedef unsigned int PIECE;
#define PIECE_BIT   4

#define P_BLANK      0
#define P_A_PAWN     1
//#define P_A_KNIGHT   2
#define P_A_ROOK     3
#define P_A_BISHOP   4
#define P_A_QUEEN    5
#define P_A_KING     6
#define P_B_PAWN     9
//#define P_B_KNIGHT  10
#define P_B_ROOK    11
#define P_B_BISHOP  12
#define P_B_QUEEN   13
#define P_B_KING    14

// 盤面
#define F_WIDTH     6
#define F_HEIGHT    6

// 各升の情報
#ifndef __uint64
typedef unsigned long long int __uint64;
#endif
// 各升の情報 (一番上の駒が下位 4 ビット、その下の駒が次の 4 ビット…)
typedef __uint64 t_field;
// 一番上の駒を取得
#define F_TOPPIECE(f) ((f) & 0xFull)
// 一番上の駒の手番 (先手 or 後手) を取得
#define F_TOPTURN(f) ((f) >> 3 & 0x1ull)
// 一番上の駒がキングかどうか
#define F_IS_KING(f) (((f) & 0x7ull) == P_A_KING)

// プレーヤ
#define PLAYERS 2
typedef int TURN;
#define TURN_FIRST  0
#define TURN_SECOND 1

#define JUDGE_PLAYING 0 // 試合中
#define JUDGE_A_WON 1 // 先手の勝ち
#define JUDGE_B_WON -1 // 後手の勝ち

// 駒の移動情報
typedef struct {
	int x1, y1, x2, y2;
} MOVE;



class TCField {
private:
	int check(TURN t, t_field *f, int x, int y);
	void addMoveList(MOVE *movelist, int x1, int y1, int x2, int y2);
	MOVE *addMoveListLoop(MOVE *movelist, TURN t, t_field *f, int x, int y, int dx, int dy);

protected:
	t_field field[F_WIDTH * F_HEIGHT];
	int judge;

public:
	TCField();
	TCField(TCField *f);
	~TCField();

	TCField *clone();
	void out();
	t_field getField(int x, int y);
	void move(int x1, int y1, int x2, int y2);
	inline void move(MOVE m) { move(m.x1, m.y1, m.x2, m.y2); }
	inline void move_inv(MOVE m) { move(m.x2, m.y2, m.x1, m.y1); }
	int getJudge(); // 勝敗を返す
	int listup(MOVE *movelist, TURN turn); // 合法手を列挙
	bool check(int x1, int y1, int x2, int y2, TURN turn); // 合法手かどうかチェック
	inline bool check(MOVE move, TURN turn) { check(move.x1, move.y1, move.x2, move.y2, turn); }
};

#endif
