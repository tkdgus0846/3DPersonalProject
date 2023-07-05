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
	// 진행하고 있던 노드를 파악해서 찾아가자. Running이라면.
	vector<CBehavior*> m_LastRunningList;

public:
	static CRootNode* Create();
	virtual void Free();
};

END