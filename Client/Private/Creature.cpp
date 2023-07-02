#include "stdafx.h"
#include "..\Public\Creature.h"

CCreature::CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCreature::CCreature(const CCreature& rhs)
	: CGameObject(rhs)
{
}
