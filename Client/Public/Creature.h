#pragma once

#include "GameObject.h"

class CCreature : public CGameObject
{
protected:
	CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreature(const CCreature& rhs);
	virtual ~CCreature() = default;

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
	_int	Get_Attack() const { return m_iAttack; }
	_int	Get_HP() const { return m_iHp; }
	_int	Get_MaxHP() const { return m_iMaxHp; }
	_float	Get_Speed() const { return m_fSpeed; }
	void	Set_Speed(const _float& speed) { m_fSpeed = speed; }

	void	Plus_Speed(_float fSpeed = 1) { m_fSpeed += fSpeed; }

protected:
	_float	m_fSpeed;
	_int	m_iMaxHp;
	_int	m_iHp;
	_int	m_iAttack;
};

