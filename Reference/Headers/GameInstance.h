#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Camera_Manager.h"

/* 0. 클라이언트 엔진의 연결 역활한다. */
/* 0.0 게임인스턴스라는 객체를 통해서 엔진의다양한기능을 활요앟ㄹ 수 있게 한다. */
/* 0.1 진짜 필요한 함수만 선택하여 오픈해준다. */
/* 0.2 한꺼번에 초기화 및 삭제처리 가능.  */
/* 0.3 업데이트 순서관리에 용이하다.  */



BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float TimeDelta);
	void Clear_LevelResources(_uint iLevelIndex);

	void ResizeBuffers(_uint & g_ResizeWidth, _uint & g_ResizeHeight);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte ubyKeyID);
	_byte Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID);
	_long Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

	bool	Key_Pressing(_ubyte ubyKey);
	bool	Key_Down(_ubyte ubyKey);
	bool	Key_Up(_ubyte ubyKey);

	bool	Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID);
	bool	Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID);
	bool	Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID);

public: /* For.Timer_Manager */
	_float		Get_Timer(const _tchar* pTimerTag);
	void		Set_Timer(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clear_CurLevel_Layer(const wstring & pLayerTag);
	void	Clear_ObjectNums();

	// Add 하지 않고 프로토타입을 찾아서 Clone만 해주는 역할.
	CGameObject* Clone_Object(const wstring & pPrototypeTag, wstring & pObjName, void* pArg = nullptr);

	// 프로토타입을 기반으로 Add 해주는 역할.
	CGameObject* Add_GameObject(_uint iLevelIndex, const wstring & pPrototypeTag, const wstring& pLayerTag, wstring & pObjName, void* pArg = nullptr);
	// 프로토타입 기반X 로 Add 해주는 역할.
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring & pLayerTag, wstring & pObjName, CGameObject * object);
	CGameObject* Copy_GameObject(const wstring & layerTag, wstring & objectName);
	HRESULT Delete_GameObject(const wstring & pLayerTag, const wstring & pObjName);
	HRESULT Delete_GameObject_SameName(const wstring & pLayerTag, const wstring & pObjName);
	CGameObject* Find_GameObject(const wstring & pLayerTag, const wstring & pObjName);

	HRESULT Save_CurLevel(const _tchar* curLevelName);
	HRESULT Load_CurLevel(const _tchar* curLevelName, _bool bIsTool = false);

	list<class CGameObject*> Get_All_Objects();

public: /* For.Component_Manager*/ 
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & pPrototypeTag, void* pArg = nullptr);
	vector<pair<wstring, class CComponent*>> Get_Prototypes_ByVector();

public: /* For.PipeLine */
	_float4x4 Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4 Get_CamPosition() const;

public: /* For. Light Manager */
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);
	HRESULT Add_Lights(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CLight::LIGHTDESC & LightDesc);

public: /* For. Collision Manager */
#ifdef _DEBUG
	void	Render_Collider();
	_bool	Get_ColliderRender();
	void	Set_ColliderRender(_bool bRender);
	_bool	Toggle_ColliderRender();
#endif
	void	Check_Collision(COLGROUP ID1, COLGROUP ID2);
	void	Change_ColGroup(CCollider * collider, COLGROUP changeID);
	void	Add_ColGroup(COLGROUP eID, CCollider * pCollider);
	void	Reset_ColGroup();
	HRESULT	Remove_Collider(CCollider * collider, COLGROUP colID);
	
public: /* For. Camera Manager*/
	CCamera* Find_Camera(const wstring & pCamTag);
	void Add_Camera(const wstring & pCamTag, CCamera * pCamera);
	void Reset_Camera();
	CCamera* Get_CurCamera();
	void On_Camera(const wstring & pCamTag);
	void On_Camera(CCamera * pCamera);
	_bool Is_Shacking() const;
	_bool Is_On_Camera(const wstring & pCamTag);
	void On_Shake(void* pArg = nullptr);
	void Off_Shake();

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath);
	HRESULT Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & Position, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), float fRotation = 0.f, const _float2 & vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

public: /* For.Frustum */
	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CCollisionManager*		m_pCollisionManager = { nullptr };
	class CCamera_Manager*			m_pCameraManager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END