#pragma once

/* °´Ã¼µéÀ» º¸°üÇÏ³®. */
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObjects(class CGameObject* pGameObject);
	list<class CGameObject*>* Get_GameObjectsList() { return &m_GameObjects; }
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

	CGameObject* Find_Object(const wstring& objName);
	HRESULT Delete_GameObject(const wstring& pObjName);
	HRESULT Clear_Layer();

private:
	list<class CGameObject*>				m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END