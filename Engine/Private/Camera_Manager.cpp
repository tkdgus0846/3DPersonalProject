#include "Camera_Manager.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	:m_pCurCamera(nullptr)
{
}

CCamera_Manager::~CCamera_Manager()
{

}

CCamera* CCamera_Manager::Find_Camera(const wstring& pCamTag)
{
	auto iter = m_CameraList.find(pCamTag);
	if (iter == m_CameraList.end()) return nullptr;

	return iter->second;
}

void CCamera_Manager::On_Camera(const wstring& pCamTag)
{
	CCamera* pCamera = Find_Camera(pCamTag);

	if (pCamera == nullptr) return;
	

	if (m_pCurCamera)
	{
		if (m_pCurCamera == pCamera) return;

		Safe_Release(m_pCurCamera);
		m_pCurCamera->Off_Camera();
	}
		

	pCamera->On_Camera();
	m_pCurCamera = pCamera;

	
	Safe_AddRef(m_pCurCamera);
	
}

void CCamera_Manager::On_Camera(CCamera* pCamera)
{
	pCamera->On_Camera();
	m_pCurCamera = pCamera;

	if (m_pCurCamera->GetOwner() != nullptr)
		Safe_AddRef(m_pCurCamera);
}

void CCamera_Manager::On_Shake(void* pArg)
{
	for (auto& item : m_CameraList)
	{
		if (m_pCurCamera != item.second)
			m_pCurCamera->Off_Shake();
	}
	m_pCurCamera->On_Shake(pArg);
}

void CCamera_Manager::Off_Shake()
{
	m_pCurCamera->Off_Shake();
}

_bool CCamera_Manager::Is_Shacking() const
{
	return m_pCurCamera->Is_Shacking();
}

void CCamera_Manager::Add_Camera(const wstring& pCamTag, CCamera* pCamera)
{
	if (Find_Camera(pCamTag))
		return;

	Safe_AddRef(pCamera);
	
	m_CameraList.emplace(pCamTag, pCamera);
	if (m_pCurCamera == pCamera)
		Safe_AddRef(m_pCurCamera);
}

void CCamera_Manager::Remove_Camera(const wstring& pCamTag)
{
	CCamera* pCamera = nullptr;
	if (pCamera = Find_Camera(pCamTag))
		return;

	Safe_Release(pCamera);
	m_CameraList.erase(pCamTag);
}

_bool CCamera_Manager::Is_On_Camera(const wstring& pCamTag)
{
	return m_CameraList[pCamTag]->m_bCameraOn;
}

void CCamera_Manager::Reset_Camera()
{
	for (auto& cameraPair : m_CameraList)
	{
		Safe_Release(cameraPair.second);
	}
	m_CameraList.clear();

	Safe_Release(m_pCurCamera);
}

void CCamera_Manager::Free(void)
{
	for (auto& item : m_CameraList)
	{
		Safe_Release(item.second);
	}

	m_CameraList.clear();
}