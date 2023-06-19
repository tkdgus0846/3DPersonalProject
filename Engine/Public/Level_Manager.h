#pragma once

/* 이전에 할당되어있던 레벨을 삭제하고 새로운 레벨로 교체한다. */
/* 이전레벨에서 사용되었던 리소스들을 해제한다. */
/* 현재 보여줘야하는 레벨의 Tick함수를 호출한다. */
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