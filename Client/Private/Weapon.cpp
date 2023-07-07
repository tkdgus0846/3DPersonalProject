#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "Player.h"
#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActorComponent(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CActorComponent(rhs)
{
}

void CWeapon::Attack_Collision_Enter(const Collision* collision)
{
	CCollider* myCollider = m_pPlayer->Get_Collider(CPlayer::COLLIDER_ATTACK);

	CCreature* creature = (CCreature*)collision->OtherGameObject;
	if (creature && myCollider == collision->MyCollider)
	{
		creature->Get_Damaged(m_Damage);
	}
}

void CWeapon::Attack_Collision_Stay(const Collision* collision)
{
	
}

void CWeapon::Attack_Collision_Exit(const Collision* collision)
{
}
