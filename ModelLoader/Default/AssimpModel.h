#pragma once

class CAssimpModel : public IWriteable
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
public:
	CAssimpModel();
	~CAssimpModel();

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	void Set_AnimIndex(_uint iAnimIndex) {
		if (iAnimIndex >= m_iNumAnimations)
			return;
		m_iCurrentAnimIndex = iAnimIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const string& pModelFilePath, _fmatrix PivotMatrix);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer; /* 파일을 읽어준다 .*/

private: /* For.Meshes  */
	_uint					m_iNumMeshes = { 0 };
	vector<class CAssimpMesh*>	m_Meshes;

private: /* For.Materials */
	_uint					m_iNumMaterials = { 0 };
	vector<_char*>	m_MaterialPaths;

private: /* For.Bones*/
	vector<class CAssimpBone*>			m_Bones;
	_uint					m_iNumBones;

public:
	typedef vector<class CAssimpBone*>	BONES;

private:
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAssimpAnimation*>		m_Animations;

private:
	_float4x4						m_PivotMatrix;

private:
	HRESULT Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Bones(aiNode * pNode, class CAssimpBone* pParent);
	HRESULT Ready_Animations();


public:
	static CAssimpModel* Create(const string& pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());


	// ISerializable을(를) 통해 상속됨
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) override;

};
