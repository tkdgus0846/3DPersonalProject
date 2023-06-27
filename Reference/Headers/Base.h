#pragma once

#include "Engine_Defines.h"

/* IUnknown ���۷��� ī��Ʈ�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	_ulong AddRef(); /* ���۷��� ī��Ʈ�� ������Ű�ϴ� .*/
	_ulong Release(); /* refcnt != 0 ���۷��� �ƴ�Ʈ�� ���ҽ�Ų��. or RefCnt == 0 �����ϳ�. */
	_ulong GetRef() { return m_dwRefCnt; }

protected: /* ���۷��� ī��Ʈ (��������) */
	_ulong			m_dwRefCnt = { 0 };

public:
	virtual void Free() = 0;
};

END