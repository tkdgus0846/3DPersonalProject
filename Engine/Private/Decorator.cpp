#include "..\Public\Decorator.h"

CDecorator* CDecorator::Create(function<_bool(CBlackBoard*)> _IsExec)
{
	CDecorator* deco = new CDecorator;
	if (_IsExec != nullptr)
		deco->m_Function = _IsExec;

	return deco;
}

void CDecorator::Free()
{
}
