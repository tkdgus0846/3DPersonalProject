#pragma once

#include "Client_Defines.h"
#include "ActorComponent.h"

BEGIN(Client)

class CPlayerParts final : public CActorComponent
{
protected:
	CPlayerParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerParts(const CPlayerParts& rhs);
	virtual ~CPlayerParts() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* ������ �����Ѵ�. */
	static CPlayerParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

