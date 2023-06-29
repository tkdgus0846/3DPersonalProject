#pragma once

#include "Base.h"
#include "Camera.h"

BEGIN(Engine)

// 틱에서 Add 해주고 포스트렌더에서 빼줌.
// 카메라가 달린 객체들은 맨 위쪽에 __super::Tick을 불러주자
// On_Camera의 호출 시점 ? 무조건 레이트 틱이 되어야하나? 
class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager();

public:
	CCamera* Find_Camera(const wstring& pCamTag);
	void Add_Camera(const wstring& pCamTag, CCamera* pCamera);
	void Remove_Camera(const wstring& pCamTag);
	_bool Is_On_Camera(const wstring& pCamTag);

	void Reset_Camera();

public:
	CCamera* Get_CurCamera() { return m_pCurCamera; }
	void On_Camera(const wstring& pCamTag);
	void On_Camera(CCamera* pCamera);
	void On_Shake(CCamera::SHAKE_TYPE eType, const _float& fForce = 1.5f, const _float& fTime = 5.f);

private:
	CCamera* m_pCurCamera;
	unordered_map<wstring, CCamera*>	m_CameraList;

private:
	virtual void Free(void) override;
};

END
