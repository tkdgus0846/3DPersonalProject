#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_LookAt : public CBehavior
{
protected:
	CTask_LookAt() = default;
	virtual ~CTask_LookAt() = default;

public:
	virtual RESULT Run(const _double & TimeDelta, vector<CBehavior*>*LastRunningList = nullptr);
	void Set_TurnTime(const _double& turnTime) { m_TurnTime = turnTime; }

private:
	_double m_TurnTime = { 0.0 };
	_double	m_TimeAcc = { 0.0 };

public:
	static CTask_LookAt* Create();
	virtual void Free();
};

END

