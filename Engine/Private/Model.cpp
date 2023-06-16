#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include <DataParsing.h>

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
	, m_iNumBones(rhs.m_iNumBones)
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
	//_uint		iFlag = 0;

	/*if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);*/

	/*if (nullptr == m_pAIScene)
		return E_FAIL;*/

	/*char ch[MAX_PATH];
	strcpy(ch, pModelFilePath);*/
	string path = pModelFilePath;
	wstring finalPath = CConversion::StringToWstring(path);
	
	ModelParsingData* myData = (ModelParsingData*)CDataParsing::Load_File(finalPath.c_str(), this);

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(myData)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(myData, eType, PivotMatrix)))
		return E_FAIL;

	//if (FAILED(Ready_Materials(myData)))
	//	return E_FAIL;

	if (FAILED(Ready_Animations(myData)))
		return E_FAIL;


	Safe_Delete(myData);



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
	/* � �ִϸ��̼��� ����Ϸ����ϴ���?! */
	/* �� �ִϸ��̼��� � ���� ����ϴ���?! */
	/* ������ ���� � ����(TransformationMatrix)�� ���ϰ� �־���ϴ°�?! */

	/* ���� �ִϸ��̼ǿ��� ����ϴ� ������ ã�Ƽ� �ش� ������ TransformationMatrix�� �����Ѵ�. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_Bones, TimeDelta);

	/* �𵨿� ǥ���Ǿ��ִ� ��� ������ CombinedTransformationMatrix */
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

	/* iMeshIndex��° �޽ð� ����ϴ� �������� ����� �����ͼ� BoneMatrices�� �־��ش�. */
	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);

	return S_OK;
}


HRESULT CModel::Ready_Meshes(ModelParsingData* parsingData,  TYPE eType, _fmatrix PivotMatrix)
{

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, &parsingData->MeshDatas[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bones(ModelParsingData* parsingData)
{
	for (int i = 0; i < m_iNumBones; i++)
	{
		CBone* pBone = CBone::Create(&parsingData->BoneDatas[i]);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ModelParsingData* parsingData)
{
	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(&parsingData->AnimationDatas[i]);
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

ParsingData* CModel::Load_Data(HANDLE handle, ParsingData* data)
{
	///////////////////////////////////////
	////////////////////////////////////////
	///////////////////////////////// �Ʒ����� ���Ϻҷ�����

	ModelParsingData* myData = new ModelParsingData;

	DWORD dwByte = 0;
	char str[MAX_PATH];

	ReadFile(handle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	ReadFile(handle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	ReadFile(handle, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	ReadFile(handle, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	myData->iNumMeshes = m_iNumMeshes; // ����
	myData->iNumMaterials = m_iNumMaterials; // ����
	myData->iNumAnimations = m_iNumAnimations;
	myData->iNumBones = m_iNumBones;

	///////// �ؽ��� ��� ����/////////////// ��� ���߿�
	/*m_Materials.push_back();
	for (auto path : m_MaterialPaths)
	{
		if (path == nullptr) continue;
		str = path;
		WriteFile(handle, &str, sizeof(str), &dwByte, nullptr);
	}*/

	for (int i = 0; i < m_iNumMaterials; i++)
	{
		MESHMATERIAL			MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			ReadFile(handle, &str, sizeof(str), &dwByte, nullptr);

			if (strcmp("NULL", str) == 0) continue;

			wstring wszFullPath = CConversion::StringToWstring(str);

			MeshMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, wszFullPath.c_str(), 1);
			if (nullptr == MeshMaterial.pMtrlTexture[j])
				return nullptr;
		}
		m_Materials.push_back(MeshMaterial);
	}

	
	// �������� �ؼ� myData�� �ø��⸸ �ϸ�� .

	CMesh mesh;
	CBone bone;
	CAnimation Animation;

	///////// �޽� ���� ����///////////////
	for (int i = 0; i < m_iNumMeshes; i++)
	{
		mesh.Load_Data(handle, myData);
	}

	///////// �� ���� ����///////////////
	for (int i = 0; i < m_iNumBones; i++)
	{
		bone.Load_Data(handle, myData);
	}

	///////// �ִϸ��̼� ���� ����///////////////
	for (int i = 0; i < m_iNumAnimations; i++)
	{
		Animation.Load_Data(handle, myData);
	}

	return myData;
}
