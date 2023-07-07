#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_Attack : public CBehavior
{
protected:
	CTask_Attack(const string& varName);
	virtual ~CTask_Attack() = default;

public:
	virtual RESULT Run(const _float & TimeDelta, vector<CBehavior*>* LastRunningList = nullptr);

public:
	static CTask_Attack* Create(const string& varName);
	virtual void Free();

private:
	string m_VarName;
};

END

