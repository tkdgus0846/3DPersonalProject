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
	void Bind_Func(function<_bool(CBlackBoard*)> _IsExec) { m_Function = _IsExec; }
	_bool Is_Exec(CBlackBoard* pBlackBoard) { return m_Function(pBlackBoard); }

	static CDecorator* Create(function<_bool(CBlackBoard*)> _IsExec = nullptr);
	
private:
	function<_bool(CBlackBoard*)> m_Function = nullptr;

	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END