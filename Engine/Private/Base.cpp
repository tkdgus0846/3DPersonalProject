#include "..\Public\Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{

	/* ������Ű�� ������ ���� �����Ѵ�. */
	return ++m_dwRefCnt;	
}

_ulong CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		/* �����ϳ�. */
		delete this;

		return 0;
	}
	
	else /* ���ҽ�Ű�� ���� ���� �����Ѵ�. */	
		return m_dwRefCnt--;	
}

