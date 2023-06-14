#include "stdafx.h"
#include "AssimpModel.h"
#include "AssimpMesh.h"
#include "AssimpBone.h"
#include "AssimpAnimation.h"

CAssimpModel::CAssimpModel()
{
}

CAssimpModel::~CAssimpModel()
{
	for (auto& Material : m_MaterialPaths)
	{
		for (auto& pPath : m_MaterialPaths)
			Safe_Delete(pPath);
	}

	m_MaterialPaths.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Delete(pMesh);

	m_Meshes.clear();

	for (auto& pBone : m_Bones)
		Safe_Delete(pBone);

	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Delete(pAnimation);

	m_Animations.clear();
}

HRESULT CAssimpModel::Initialize_Prototype(const string& pModelFilePath, _fmatrix PivotMatrix)
{
	_uint		iFlag = 0;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_MaterialPaths.resize(AI_TEXTURE_TYPE_MAX);

	/*if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	_uint AnimationNums = m_pAIScene->mNumAnimations;

	TYPE eType = TYPE_ANIM;

	if (AnimationNums == 0) eType = TYPE_NONANIM;
	else eType = TYPE_ANIM;

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath.c_str())))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}



HRESULT CAssimpModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CAssimpMesh* pMesh = CAssimpMesh::Create(m_Bones, m_pAIScene->mMeshes[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CAssimpModel::Ready_Materials(const char* pModelFilePath)
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

			char* szFullPath = new char[MAX_PATH]; 
			strcpy(szFullPath, szDrive);
			strcat(szFullPath, szDirectory);
			strcat(szFullPath, szFileName);
			strcat(szFullPath, szExt);

			strcpy(m_MaterialPaths[j], szFullPath);
		}
	}

	return S_OK;
}

HRESULT CAssimpModel::Ready_Bones(aiNode* pNode, CAssimpBone* pParent)
{
	CAssimpBone* pBone = CAssimpBone::Create(pNode, pParent, m_Bones.size());
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (size_t i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_Bones(pNode->mChildren[i], pBone);
	}

	return S_OK;
}

HRESULT CAssimpModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAssimpAnimation* pAnimation = CAssimpAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CAssimpModel* CAssimpModel::Create(const string& pModelFilePath, _fmatrix PivotMatrix)
{
	CAssimpModel* pInstance = new CAssimpModel();

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CAssimpModel");
		Safe_Delete(pInstance);
	}
	return pInstance;
}

HRESULT CAssimpModel::Save_Data(ParsingData* data)
{
	ModelParsingData* modelParsingData = new ModelParsingData;

	modelParsingData->iNumMeshes = m_iNumMeshes; // 저장
	modelParsingData->iNumMaterials = m_iNumMaterials; // 저장
	

	modelParsingData->MaterialPaths.resize(AI_TEXTURE_TYPE_MAX);
	for (int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		strcpy(modelParsingData->MaterialPaths[i], m_MaterialPaths[i]); // 벡터 그대로 다 저장
	}
	
	modelParsingData->iNumAnimations = m_iNumAnimations; // 저장

	for (auto mesh : m_Meshes)
	{
		mesh->Save_Data(data);
	}

	for (auto bone : m_Bones)
	{
		bone->Save_Data(data);
	}

	
	for (auto animation : m_Animations)
	{
		animation->Save_Data(data);
	}
	// 메쉬 Save_Data(modelParsingData);
	// 애니메이션 Save_Data(modelParsingData);
	// ??? Save_Data(modelParsingData);

	// 라고 했을때 세이브와 로드 순서가 지켜지는가?
	// modelParsingData 를 함수 들어갈때 채워준 다음에 가지고 나와서 여기서 저장해도 되고, Save_Data 함수를 호출 할 때에 저장하는 방식을 채택해도 된다.

	Safe_Delete(modelParsingData);

	return S_OK;
}
