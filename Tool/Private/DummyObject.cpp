#include "stdafx.h"
#include "DummyObject.h"
#include "GameInstance.h"
#include "Calculator.h"

CDummyObject::CDummyObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummyObject::CDummyObject(const CDummyObject & rhs)
	: CGameObject(rhs)
	, m_ShaderPrototypeName(rhs.m_ShaderPrototypeName)
	, m_ModelPrototypeName(rhs.m_ModelPrototypeName)
	, m_TexturePrototypeName(rhs.m_TexturePrototypeName)
	, m_VIBufferPrototypeName(rhs.m_VIBufferPrototypeName)
{
	// 컴포넌트 들을 싹다 복사해올 방법?
	for (auto item : m_Components)
	{
		wstring compName = item.first;

		if (compName.compare(RENDERER_W) == 0)
		{
			m_pRendererCom = dynamic_cast<CRenderer*>(item.second);
		}
		else if (compName.compare(TRANSFORM_W) == 0)
		{
			m_pTransformCom = dynamic_cast<CTransform*>(item.second);
		}
		else if (compName.compare(VIBUFFER_W) == 0)
		{
			m_pVIBufferCom = dynamic_cast<CVIBuffer*>(item.second);
		}
		else if (compName.compare(SHADER_W) == 0)
		{
			m_pShaderCom = dynamic_cast<CShader*>(item.second);
		}
		else if (compName.compare(TEXTURE_W) == 0)
		{
			m_pTextureCom = dynamic_cast<CTexture*>(item.second);
		}
		else if (compName.compare(MODEL_W) == 0)
		{
			m_pModelCom = dynamic_cast<CModel*>(item.second);
		}
	}
}

HRESULT CDummyObject::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	return S_OK;
}

void CDummyObject::Tick(_double TimeDelta)
{
 	__super::Tick(TimeDelta);

	if (m_pModelCom != nullptr && m_pModelCom->Get_NumAnimations() > 0)
		m_pModelCom->Play_Animation(TimeDelta);
}

void CDummyObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDERGROUP)m_eRenderGroup, this);
}

HRESULT CDummyObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom == nullptr)
	{
		if (m_pShaderCom != nullptr)
			m_pShaderCom->Begin(m_iPassNum);

		if (m_pVIBufferCom != nullptr)
			m_pVIBufferCom->Render();
	}
	else
	{
		if (m_pShaderCom != nullptr)
		{
			_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

			for (size_t i = 0; i < iNumMeshes; i++)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

				m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
				/*m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);*/

				m_pShaderCom->Begin(m_iPassNum);

				m_pModelCom->Render(i);
			}
		}
		
	}

#ifdef _DEBUG
	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->Render_Navigation();
#endif
	

	// 객체의 로컬축을 나타내자.

	return S_OK;
}

BoundingSphere* CDummyObject::Get_PickingSphere()
{ 
	CTransform* transform = m_pTransformCom;

	if (transform == nullptr) return nullptr;

	_vector translation = transform->Get_State(CTransform::STATE_POSITION);

	_float3 spherePos;
	XMStoreFloat3(&spherePos, translation);

	m_PickingSphere.Center = spherePos;
	m_PickingSphere.Radius = 1.f;

	return &m_PickingSphere; 
}

HRESULT CDummyObject::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC desc = CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f));
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Transform"),
		TEXT("Transform"), (CComponent**)&m_pTransformCom, &desc)))
		return E_FAIL;

	///* For.Com_VIBuffer */
	//if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	TEXT("VIBuffer"), (CComponent**)&m_pVIBufferCom, this)))
	//	return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	TEXT("Shader"), (CComponent**)&m_pShaderCom, this)))
	//	return E_FAIL;

	//
	///* For.Com_Texture */
	//if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
	//	TEXT("Texture"), (CComponent**)&m_pTextureCom, this)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::SetUp_ShaderResources()
{	
	_bool bContinue = true;
	if (m_pModelCom == nullptr && (m_pTextureCom == nullptr || m_pVIBufferCom == nullptr))
	{
		bContinue = false;
	}

	if (m_pShaderCom == nullptr)
		bContinue = false;
	
	if (bContinue == false) 
		return E_FAIL;

	_float4x4 myMatrix = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &myMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (m_pModelCom == nullptr)
	{
		m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture");

		if (m_iPassNum == 1)
		{
			
		}
	}
	
	myMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", &myMatrix);

	myMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", &myMatrix);
	
	_float4 vCamPos = pGameInstance->Get_CamPosition();
	m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4));
	
	_float3 lightPos = _float3(500.f, 500.f, 500.f);
	m_pShaderCom->Bind_Float3("g_LightPosition", &lightPos);

	return S_OK;
}

CDummyObject * CDummyObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDummyObject*	pInstance = new CDummyObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummyObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject * CDummyObject::Clone(void * pArg)
{
	CDummyObject*	pInstance = new CDummyObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummyObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummyObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pBrushTexture);
	Safe_Release(m_pNavigationCom);

	__super::Free();
}

ParsingData* CDummyObject::Save_Data(HANDLE handle, ParsingData* data)
{
	ObjectParsingData* objectData = new ObjectParsingData;
	DWORD dwByte;
	_tchar name[MAX_PATH];


	// 위치 저장
	if (m_pTransformCom)
	{
		lstrcpy(name, TRANSFORM_W);
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
		m_pTransformCom->Save_Data(handle, objectData);
	}
	//if (m_pRendererCom)
	//{
	//	lstrcpy(name, RENDERER_W);
	//	WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
	//	m_pRendererCom->Save_Data(handle, objectData);
	//}
	if (m_pShaderCom)
	{
		lstrcpy(name, SHADER_W);
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);

		lstrcpy(name, m_ShaderPrototypeName.c_str());
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
	}
	// 무슨 모델이였는지 저장
	if (m_pModelCom)
	{
		lstrcpy(name, MODEL_W);
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);

		lstrcpy(name, m_ModelPrototypeName.c_str());
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
	}
	if (m_pTextureCom)
	{
		lstrcpy(name, TEXTURE_W);
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);

		lstrcpy(name, m_TexturePrototypeName.c_str());
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
	}
	
	if (m_pVIBufferCom)
	{
		lstrcpy(name, VIBUFFER_W);
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);

		lstrcpy(name, m_VIBufferPrototypeName.c_str());
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
	}
	if (m_pNavigationCom)
	{
		lstrcpy(name, NAVIGATION_W);
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);

		lstrcpy(name, m_NavigationPrototypeName.c_str());
		WriteFile(handle, name, sizeof(name), &dwByte, nullptr);
	}

	// 무슨 버퍼였는지 텍스쳐였는지 저장

	// 무슨 쉐이더를 쓰고 있었는지 저장

	

	Safe_Delete(objectData);
	return nullptr;
}

ParsingData* CDummyObject::Load_Data(HANDLE handle, ParsingData* data)
{
	ObjectParsingData* objectData = new ObjectParsingData;
	DWORD dwByte;
	_tchar name[MAX_PATH];

	while (true)
	{
		ReadFile(handle, name, sizeof(name), &dwByte, nullptr);
		if (dwByte == 0) break;

		CComponent** FieldComp = nullptr;
		void* pArg = nullptr;
		wstring prototypeName;
		wstring componentName;

		_bool bAddComponent = true;

		if (lstrcmp(name, TRANSFORM_W) == 0)
		{
			m_pTransformCom->Load_Data(handle, objectData);

			bAddComponent = false;
		}
		else if (lstrcmp(name, SHADER_W) == 0)
		{
			ReadFile(handle, name, sizeof(name), &dwByte, nullptr);

			prototypeName = name;
			componentName = SHADER_W;
			FieldComp = (CComponent**)&m_pShaderCom;
			m_ShaderPrototypeName = prototypeName;
		}
		else if (lstrcmp(name, MODEL_W) == 0)
		{
			ReadFile(handle, name, sizeof(name), &dwByte, nullptr);

			prototypeName = name;
			componentName = MODEL_W;
			FieldComp = (CComponent**)&m_pModelCom;
			m_ModelPrototypeName = prototypeName;
		}
		else if (lstrcmp(name, TEXTURE_W) == 0)
		{
			ReadFile(handle, name, sizeof(name), &dwByte, nullptr);

			prototypeName = name;
			componentName = TEXTURE_W;
			FieldComp = (CComponent**)&m_pTextureCom;
			m_TexturePrototypeName = prototypeName;
		}
		else if (lstrcmp(name, VIBUFFER_W) == 0)
		{
			ReadFile(handle, name, sizeof(name), &dwByte, nullptr);

			prototypeName = name;
			componentName = VIBUFFER_W;
			FieldComp = (CComponent**)&m_pVIBufferCom;
			m_VIBufferPrototypeName = prototypeName;
		}
		else if (lstrcmp(name, NAVIGATION_W) == 0)
		{
			ReadFile(handle, name, sizeof(name), &dwByte, nullptr);

			prototypeName = name;
			componentName = NAVIGATION_W;
			FieldComp = (CComponent**)&m_pNavigationCom;
			m_NavigationPrototypeName = prototypeName;
		}

		if (bAddComponent)
			Add_Component(LEVEL_TOOL, prototypeName.c_str(), componentName, FieldComp, pArg);
	}

	Safe_Delete(objectData);
	return nullptr;
}
