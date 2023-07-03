#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_MoveForward : public CBehavior
{
protected:
	CTask_MoveForward() = default;
	virtual ~CTask_MoveForward() = default;

public:
	virtual RESULT Run(const _double & TimeDelta, vector<CBehavior*>*LastRunningList = nullptr);

public:
	static CTask_MoveForward* Create();
	virtual void Free();
};

END

