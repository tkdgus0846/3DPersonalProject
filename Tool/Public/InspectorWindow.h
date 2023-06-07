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
	IM_VIBUFFER,
	IM_RENDERER,

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
	void			Mode_Renderer();
	void			Mode_Default();

	/*컴포넌트 인스펙터 창*/
	void			Show_ComponentList();
	void			Add_Component_Function();
	void			Delete_Component_Function();
	void			Delete_Button();

	/*디폴트 인스펙터 창*/
	void			Add_GameObject_Function();
	void			Delete_GameObject_Function();

	/* 마우스 피킹 */
	HRESULT			Place_Object();


private:
	InspectorMode	m_eInspectorMode;
	CComponent*		m_pCurComponent = { nullptr };
	_int			selectedComponentIndex = { -1 };
	vector<pair<const _tchar*, class CComponent*>> m_PrototypesVec;
	vector<string>			m_PrototypesStrVec;

	/* 문자열 입력 버퍼들*/
	_char m_ComponentSearchBuffer[FILE_NAME_SIZE] = "";
	_char m_ObjectAddNameBuffer[FILE_NAME_SIZE] = "";

	/* 트랜스폼 관련 변수들*/
	_float translation[3] = { 0.f, 0.f, 0.f };
	_float rotation[3] = { 0.f, 0.f, 0.f };
	_float scale[3] = { 1.f, 1.f, 1.f };
	_bool m_bPlaceObject = { false };
	_bool m_bScaleLock = { false };
};

