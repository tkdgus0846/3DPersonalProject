#pragma once

/* ������ �Ҵ�Ǿ��ִ� ������ �����ϰ� ���ο� ������ ��ü�Ѵ�. */
/* ������������ ���Ǿ��� ���ҽ����� �����Ѵ�. */
/* ���� ��������ϴ� ������ Tick�Լ��� ȣ���Ѵ�. */
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

	_uint Get_CurLevelIndex() const { return m_iLevelIndex; }


private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	_uint					m_iLevelIndex = { 0 };

public:
	virtual void Free() override;
};

END