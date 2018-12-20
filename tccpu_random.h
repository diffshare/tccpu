#include <stdlib.h>
#include <time.h>
#include "tccpubase.h"

class TCCPU_Random : public TCCPUBase {
public:
	void init();
	MOVE turn(TCField *field, TURN turn);
};
