#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer;
class CModel;
class CNavigation;
END

class CDummyObject final : public CGameObject, public ISerializable
{
	friend class CInspectorWindow;
	friend class CObjectWindow;
	friend class CMapWindow;
	friend class CAnimWindow;

protected:
	CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummyObject(const CDummyObject& rhs);
	virtual ~CDummyObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTransform*				m_pTransformCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer*				m_pVIBufferCom = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };

	// �ͷ��� �귯�� ���� ������
	CTexture*				m_pBrushTexture = { nullptr };
	_float					m_BrushSize = { 10.f };
	_float4					m_BrushPos = { 10000.f,10000.f,10000.f,1.f };

	// �Ʒ��� �������� Ŭ�� �Ѱ��� �ʿ䰡 ������ ������ ����ϱ� ���ؼ� ����� �������̴�.
	wstring					m_ShaderPrototypeName = { L"" };
	wstring					m_ModelPrototypeName = { L"" };
	wstring					m_TexturePrototypeName = { L"" };
	wstring					m_VIBufferPrototypeName = { L"" };
	wstring					m_NavigationPrototypeName = { L"" };

	BoundingSphere			m_PickingSphere;

public:
	BoundingSphere* Get_PickingSphere(); 

	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* ������ �����Ѵ�. */
	static CDummyObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// ISerializable��(��) ���� ��ӵ�
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) final;
	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final;
};
