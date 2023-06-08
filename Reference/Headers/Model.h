#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}



public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType MaterialType);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer; /* 파일을 읽어준다 .*/

private: /* For.Meshes  */
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private: /* For.Materials */
	_uint					m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>	m_Materials;

private:
	HRESULT Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);


public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END