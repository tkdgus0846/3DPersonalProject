#pragma once

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CRandomChooser : public CBehavior
{
protected:
	CRandomChooser() = default;
	virtual ~CRandomChooser() = default;

public:
	virtual RESULT Run(const _float & TimeDelta, vector<CBehavior*>* LastRunningList = nullptr) override;

public:
	static CRandomChooser* Create();
	virtual void Free();
};

END

