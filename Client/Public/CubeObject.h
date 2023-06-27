#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
class CCollider;
END

BEGIN(Client)

class CCubeObject final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_END };
protected:
	CCubeObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCubeObject(const CCubeObject& rhs);
	virtual ~CCubeObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollisionEnter(const Collision* collision) { cout << "Cube Object Enter!" << endl; }
	virtual void OnCollisionStay(const Collision* collision) { cout << "Cube Object Stay!" << endl; }
	virtual void OnCollisionExit(const Collision* collision) { cout << "Cube Object Exit!" << endl; }

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* 원형을 생성한다. */
	static CCubeObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END