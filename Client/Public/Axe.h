#pragma once

#include "Client_Defines.h"
#include "ActorComponent.h"

BEGIN(Client)

class CAxe final : public CActorComponent
{
protected:
	CAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAxe(const CAxe& rhs);
	virtual ~CAxe() = default;

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
	static CAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

