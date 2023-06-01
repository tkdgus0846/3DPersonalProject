#pragma once

#include "Base.h"

/* ��ü���� ������ �����Ѵ�. */
/* ���ӳ��� �����ϴ� ��ü���� ���� ������ ���� ���ؿ� ���� ��������(CLayer) �����Ѵ�. */
/* ��ü���� Tick, Late_Tick�� ������ ȣ�� ���ش�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

	
public:/*���� ����� ������ ������ŭ �̸� ������ �Ҵ��ϳ�. */
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pObjName, void* pArg);
	void Clear_LevelResources(_uint iLevelIndex);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

public:
	unordered_map<const _tchar*, class CLayer*>* Get_LayersMapPtr() { return m_pLayers; }

private: /* ������ü���� �����Ͽ� �����ϰ� �ִ´�. */
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;

private: /* �纻��ü���� ���̾�� �����Ͽ� �����Ѵ�. */
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