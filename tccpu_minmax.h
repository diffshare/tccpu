#include "tccpubase.h"

// 3 だと一瞬、4 だと 1 秒ちょっと、5 だと 1 分弱
#define MAX_DEPTH 4

class TCCPU_MinMax : public TCCPUBase {
protected:
	int eval_table[16][16]; // 評価テーブル
	int score;

	int calc(TCField *field, MOVE move);
	int maxOfMin(TCField *field, int depth, MOVE **p_move);
	int minOfMax(TCField *field, int depth, MOVE **p_move);
	int move_s(TCField *field, MOVE move);
public:
	TCCPU_MinMax();
	~TCCPU_MinMax();

	void init();
	MOVE turn(TCField *field, TURN turn);
	void turnend(TCField *field, MOVE move);
};
