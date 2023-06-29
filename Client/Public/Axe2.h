#pragma once

#include "Client_Defines.h"
#include "ActorComponent.h"

BEGIN(Client)

class CAxe2 final : public CActorComponent
{
protected:
	CAxe2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAxe2(const CAxe2& rhs);
	virtual ~CAxe2() = default;

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
	static CAxe2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

