#pragma once

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CSequence : public CBehavior
{
protected:
	CSequence() = default;
	virtual ~CSequence() = default;

public:
	virtual RESULT Run(const _float & TimeDelta, vector<CBehavior*>* LastRunningList = nullptr);

public:
	static CSequence* Create();
	virtual void Free();
};

END