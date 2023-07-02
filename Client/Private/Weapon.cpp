#include "stdafx.h"
#include "..\Public\Weapon.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActorComponent(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CActorComponent(rhs)
{
}
