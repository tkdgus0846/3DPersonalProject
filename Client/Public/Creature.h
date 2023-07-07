#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CCreature : public CGameObject
{
protected:
	CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreature(const CCreature& rhs);
	virtual ~CCreature() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void	Get_Damaged(_int Damage)
	{
		if (GetDead()) return;

		m_iHp -= Damage;
		if (m_iHp <= 0)
		{
			m_iHp = 0;
			SetDead();
		}
	}
	_int	Get_HP() const { return m_iHp; }
	_int	Get_MaxHP() const { return m_iMaxHp; }

protected:
	void	Set_HP(_int iHP) { m_iHp = iHP; m_iMaxHp = iHP; }
	_float	Compute_NavMesh_Height();
	void	ClimbNavMesh();
	void	Navigation_CurIndex();

protected:
	virtual HRESULT		Add_Animations() PURE;
	virtual void		Select_AnimationKey() PURE;


protected:
	_int	m_iMaxHp;
	_int	m_iHp;
	_bool	m_bClimbNavMesh = { true };
	_int	m_CurIndex = { -1 };

	// 생명체들은 트랜스폼콤퍼넌트랑 네비게이션컴포넌트가 있다는 가정하에 하는것이다.
	// 네비게이션 컴은 있어도되고 없어도 되지만 없으면 관련 함수들은 불러주면안된다.

	CTransform*		m_pTransformCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
};

END
