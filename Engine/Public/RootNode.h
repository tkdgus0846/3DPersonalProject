#pragma once

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CRootNode : public CBehavior
{
protected:
	CRootNode() = default;
	virtual ~CRootNode() = default;

public:
	virtual RESULT Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList = nullptr);

private:
	// �����ϰ� �ִ� ��带 �ľ��ؼ� ã�ư���. Running�̶��.
	vector<CBehavior*> m_LastRunningList;

public:
	static CRootNode* Create();
	virtual void Free();
};

END