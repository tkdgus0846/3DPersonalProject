#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_FollowTarget : public CBehavior
{
protected:
	CTask_FollowTarget() = default;
	virtual ~CTask_FollowTarget() = default;

public:
	virtual RESULT Run(const _double & TimeDelta, vector<CBehavior*>* LastRunningList = nullptr);
	void Set_WaitTime(const _double& waitTime) { m_WaitTime = waitTime; }

private:
	_double m_WaitTime = { 0.0 };
	_double	m_TimeAcc = { 0.0 };

public:
	static CTask_FollowTarget* Create();
	virtual void Free();
};

END

