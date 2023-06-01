#pragma once

#include "Base.h"

/* 객체들의 원형을 보관한다. */
/* 게임내에 출현하는 객체들을 내가 나누고 싶은 기준에 따라 구분지어(CLayer) 보관한다. */
/* 객체들의 Tick, Late_Tick을 무한히 호출 해준다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

	
public:/*실제 사용할 레벨의 갯수만큼 미리 공간을 할당하낟. */
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pObjName, void* pArg);
	void Clear_LevelResources(_uint iLevelIndex);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

public:
	unordered_map<const _tchar*, class CLayer*>* Get_LayersMapPtr() { return m_pLayers; }

private: /* 원형객체들을 생성하여 보관하고 있는다. */
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;

private: /* 사본객체들을 레이어로 구분하여 보관한다. */
	_uint												m_iNumLevels = { 0 };
	unordered_map<const _tchar*, class CLayer*>*		m_pLayers = { nullptr };
	typedef unordered_map<const _tchar*, class CLayer*>	LAYERS;

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END