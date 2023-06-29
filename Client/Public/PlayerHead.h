#pragma once

#include "Client_Defines.h"
#include "ActorComponent.h"

BEGIN(Client)

class CPlayerHead final : public CActorComponent
{
protected:
	CPlayerHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerHead(const CPlayerHead& rhs);
	virtual ~CPlayerHead() = default;

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
	static CPlayerHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

