#include <stdio.h>
#include <string.h>
#include "tccpu_human.h"

MOVE TCCPU_Human::turn(TCField *field, TURN turn) {
	while(1) {
		char buf[256];
		char cx1, cx2;
		int y1, y2;
		printf("> ");
		fgets(buf, 256, stdin);
		if(strlen(buf) <= 2) {
			// 空行を入力したら投了
			return {-1, -1, -1, -1};
		} else if(
			   sscanf(buf, "%c%d,%c%d",  &cx1, &y1, &cx2, &y2) > 0
			|| sscanf(buf, "%c%d, %c%d", &cx1, &y1, &cx2, &y2) > 0
		) {
			// 「a1」形式から座標形式に変換
			int x1 = cx1 - 'a', x2 = cx2 - 'a';
			y1 -= 1; y2 -= 1;

			if(field -> check(x1, y1, x2, y2, turn)) {
				return {x1, y1, x2, y2};
			} else {
				printf("invalid move? (%d, %d) -> (%d, %d)\n", x1, y1, x2, y2);
			}
		}
	}
}
