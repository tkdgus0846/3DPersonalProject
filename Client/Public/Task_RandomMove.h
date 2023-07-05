#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_RandomMove : public CBehavior
{
protected:
	CTask_RandomMove() = default;
	virtual ~CTask_RandomMove() = default;

public:
	virtual RESULT Run(const _float & TimeDelta, vector<CBehavior*>*LastRunningList = nullptr);
	void Set_MoveTime(const _float& moveTime) { m_MoveTime = moveTime; }

private:
	_float	m_TimeAcc = { 0.0 };
	_float m_MoveTime = { 0.0 };
	_float3 m_RandomDir = { 0.f,0.f,0.f };

public:
	static CTask_RandomMove* Create();
	virtual void Free();
};

END

