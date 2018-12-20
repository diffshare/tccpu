#ifndef __TCCPUBASE_H__
#define __TCCPUBASE_H__

#include "tcfield.h"

class TCCPUBase {
public:
	virtual void init();
	virtual MOVE turn(TCField *field, TURN turn) = 0;
	virtual void turnend(TCField *field, MOVE move);
	virtual void end();
};

#endif
