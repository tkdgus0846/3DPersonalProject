#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pOriginalAnimation : rhs.m_Animations)
		m_Animations.push_back(pOriginalAnimation->Clone());

	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_AddRef(pTexture);
	}
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix)
{
	_uint		iFlag = 0;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(eType, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;





	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta)
{
	/* 어떤 애니메이션을 재생하려고하는지?! */
	/* 이 애니메이션은 어떤 뼈를 사용하는지?! */
	/* 뼈들은 각각 어떤 상태(TransformationMatrix)를 취하고 있어야하는가?! */

	/* 현재 애니메이션에서 사용하는 뼈들을 찾아서 해당 뼈들의 TransformationMatrix를 갱신한다. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_Bones, TimeDelta);

	/* 모델에 표현되어있는 모든 뼈들의 CombinedTransformationMatrix */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

HRESULT CModel::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType MaterialType)
{
	if (iMeshIndex >= m_iNumMeshes ||
		MaterialType >= AI_TEXTURE_TYPE_MAX)
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	/* iMeshIndex번째 메시가 사용하느 ㄴ뼈들의 행려을 가져와서 BoneMatrices에 넣어준다. */
	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);

	return S_OK;
}


HRESULT CModel::Ready_Meshes(TYPE eType, _fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, m_pAIScene->mMeshes[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	/* 현재 모델에게 부여할 수 있는 재질(Diffuse, Normal, Specular etc) 텍스쳐의 갯수. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESHMATERIAL			MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	strPath;

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char		szDrive[MAX_PATH] = "";
			char		szDirectory[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char		szFullPath[MAX_PATH] = "";
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		wszFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath),
				wszFullPath, MAX_PATH);

			MeshMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, wszFullPath, 1);
			if (nullptr == MeshMaterial.pMtrlTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pNode, CBone* pParent)
{
	CBone* pBone = CBone::Create(pNode, pParent, m_Bones.size());
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (size_t i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_Bones(pNode->mChildren[i], pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_Release(pTexture);
	}

	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();
}
