#pragma once

#include "Base.h"

BEGIN(Engine)
class CCollider;
class CGameObject;

class CCollisionManager : public CBase
{
	DECLARE_SINGLETON(CCollisionManager)

private:
	explicit CCollisionManager();
	virtual ~CCollisionManager();

public:
	// ���������� �ݸ����� üũ���ִ� �Լ�.
	void		Check_Collision(COLGROUP ID1, COLGROUP ID2);

	// �ݶ��̴����� ������ ���ִ� �Լ�.
	void		Change_ColGroup(CCollider* collider, COLGROUP changeID);

	void		Add_ColGroup(COLGROUP eID, CCollider* pCollider);
	//void		Add_Collider(CGameObject* gameObject);

	HRESULT		Remove_Collider(CCollider* collider, COLGROUP colID);
	HRESULT		Remove_Collider(CGameObject* gameObject);
	void		Reset_ColGroup();

	//void		Clear_ColliderList(_int index);

#ifdef _DEBUG
	void		Render_Colliders();
	_bool		GetIsRender() const { return m_bIsRender; }
	void		Set_ColliderRender(_bool bRender) { m_bIsRender = bRender; }
	_bool		Toggle_ColliderRender() { return m_bIsRender = (m_bIsRender == true) ? false : true; }
#endif

private:
	list<CCollider*>*	Get_ColliderList(COLGROUP colID);
	void				Find_Remove_Collider(CGameObject* gameObject, COLGROUP colID);

private:
	list<CCollider*>	m_ColliderList[COL_END];


private:
#ifdef _DEBUG
	_bool				m_bIsRender = { true };
#endif
	

	// CBase��(��) ���� ��ӵ�
	virtual void Free() override;

};

END