#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;

class ENGINE_DLL CActorComponent : public CGameObject
{
public:
	typedef struct tagParentDesc
	{
		_float4x4			OffsetMatrix;
		_float4x4			PivotMatrix;
		const _float4x4*	pCombindTransformationMatrix;
		const _float4x4*	pParentWorldMatrix;

		const _tchar*	    ModelName;
		_float4x4			LocalOffsetMatrix;

	}PARENTDESC;

protected:
	CActorComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActorComponent(const CActorComponent& rhs);
	virtual ~CActorComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	void			Set_Enabled(_bool bEnable) { m_bEnabled = bEnable; }
	_bool			Get_Enabled() { return m_bEnabled; }

protected:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };	
	CRenderer*				m_pRendererCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };

	PARENTDESC				m_ParentDesc;
	_float4x4				m_WorldMatrix;

	_bool					m_bEnabled = { true };


public:
	virtual HRESULT Add_Components() { return S_OK; }
	virtual HRESULT SetUp_ShaderResources() { return S_OK; }

public:
	virtual void Free() override;
};

END