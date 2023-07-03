#pragma once

#include "Base.h"


BEGIN(Engine)

class CBlackBoard;

class ENGINE_DLL CDecorator : public CBase
{
protected:
	CDecorator() = default;
	virtual ~CDecorator() = default;

public:
	void Bind_Func(_bool(*_IsExec)()) { IsExec = _IsExec; }
	_bool Is_Exec() { return IsExec(); }
	void Set_BlackBoard(CBlackBoard* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

	static CDecorator* Create(_bool(*_IsExec)() = nullptr);
	
private:
	_bool(*IsExec)() = nullptr;
	CBlackBoard* m_pBlackBoard = { nullptr };

	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END