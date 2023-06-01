#pragma once
#include "ImGuiWindow.h"

BEGIN(Engine)
class CGameObject;
class CComponent;
END

enum InspectorMode
{
	IM_GAMEOBJECT,
	IM_TRANSFORM,
	IM_COLLIDER,
	IM_TEXTURE,
	IM_SHADER,
	IM_ANIMATION,

	IM_DEFAULT,
	IM_END
};

class CInspectorWindow : public CImGuiWindow
{

public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CInspectorWindow* Create(const WINDOWDESC& windowDesc);

private:
	virtual void	Rendering() override;

	void			Mode_GameObject();
	void			Mode_Transform();
	void			Mode_Collider();
	void			Mode_Texture();
	void			Mode_Shader();
	void			Mode_Animation();
	void			Mode_Default();

	void			Show_ComponentList();
	void			Add_Component_Function();
	void			Delete_Component_Function();


private:
	InspectorMode	m_eInspectorMode;
	CComponent*		m_pCurComponent = { nullptr };
	_int			selectedComponentIndex = { -1 };
	vector<pair<const _tchar*, class CComponent*>> m_PrototypesVec;
	vector<string>			m_PrototypesStrVec;
	char searchBuffer[MAX_PATH] = "";
};

