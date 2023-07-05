#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_Wait : public CBehavior
{
protected:
	CTask_Wait() = default;
	virtual ~CTask_Wait() = default;

public:
	virtual RESULT Run(const _float & TimeDelta, vector<CBehavior*>* LastRunningList = nullptr);
	void Set_WaitTime(const _float& waitTime) { m_WaitTime = waitTime; }

private:
	_float m_WaitTime = { 0.0 };
	_float	m_TimeAcc = { 0.0 };

public:
	static CTask_Wait* Create();
	virtual void Free();
};

END

