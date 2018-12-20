#include "tccpu_random.h"

void TCCPU_Random::init() {
	srand((unsigned int) time(NULL));
}

MOVE TCCPU_Random::turn(TCField *field, TURN turn) {
	MOVE m[1024];
	int count = field -> listup(m, turn);

	if(count > 0) {
		// 見つかった手のうち 1 つをランダムで返す
		return m[rand() % count];
	} else {
		// 手が見つからなかったときギブアップ (通常はあり得ないはず…)
		m[0].x1 = -1; m[0].y1 = -1; m[0].x2 = -1; m[0].y2 = -1;
		return m[0];
	}
}
