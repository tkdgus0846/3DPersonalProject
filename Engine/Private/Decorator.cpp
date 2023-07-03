#include "..\Public\Decorator.h"

CDecorator* CDecorator::Create(_bool(*_IsExec)())
{
	CDecorator* deco = new CDecorator;
	if (_IsExec != nullptr)
		deco->Bind_Func(_IsExec);

	return new CDecorator;
}

void CDecorator::Free()
{
}
