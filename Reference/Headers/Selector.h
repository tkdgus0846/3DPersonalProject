#pragma once

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CSelector : public CBehavior
{
protected:
	CSelector() = default;
	virtual ~CSelector() = default;

public:
	virtual RESULT Run(const _double & TimeDelta, vector<CBehavior*>* LastRunningList = nullptr) override;

public:
	static CSelector* Create();
	virtual void Free();
};

END

