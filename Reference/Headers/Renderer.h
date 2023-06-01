#pragma once

#include "Component.h"

/* ��ü���� ������ ȣ�����ֱ� ����. */
/* ���� �׸��� ���� ������� ��ü���� �׷��ֱ�����. �� ����� ������Ʈ �Ŵ����� ȥ���ϱ⿣ ���ô�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

protected:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(const CComponent& rhs);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:	
	list<class CGameObject*>				m_RenderObjects[RENDER_END];

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END