#pragma once

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WM_FULL, WM_WIN, WM_END };
		HWND			hWnd;
		unsigned int	iViewportSizeX, iViewportSizeY;
		WINMODE			eWinMode;		
	}GRAPHICDESC;

	typedef struct tagMeshMaterial
	{
		class CTexture* pMtrlTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIAL;

	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		double			Time;
	}KEYFRAME;

	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexCoord;
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC			Elements[2];
	}VTXPOSTEX_DECL;

	typedef struct tagVertex_Position_TexCoord3D
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexCoord;
	}VTXPOSTEXCUBE;

	typedef struct ENGINE_DLL tagVertex_Position_TexCoord3D_Declaration
	{
		static const unsigned int iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];
	}VTXPOSTEXCUBE_DECL;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexCoord;
	}VTXPOSNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_TexCoord_Declaration
	{
		static const unsigned int				iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSNORTEX_DECL;

	typedef struct tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexCoord;
		XMFLOAT3		vTangent;
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Mesh_Declaration
	{
		static const unsigned int				iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC			Elements[4];
	}VTXMESH_DECL;

	struct Triangle
	{
		Triangle(const _float3& v1, const _float3& v2, const _float3& v3) : vVertex1(v1), vVertex2(v2), vVertex3(v3) {}
		Triangle() {}

		_float3 vVertex1;
		_float3 vVertex2;
		_float3 vVertex3;
	};

	typedef struct tagVertex_AnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexCoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* �� ������ � ����(�ִ�4��)�� ������� �޾Ƽ� ó���Ǿ��ϴ°�?! : ��(��X, �޽�O)�� �ε��� */
		XMFLOAT4		vBlendWeights; /* �װ����� �̿��ϵ� �� ���� ��������� ���ۼ�Ʈ(1�� �������� ǥ���� ����)�� �̿��ϴ°�?! */
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertex_AnimMesh_Declaration
	{
		static const unsigned int				iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC			Elements[6];
	}VTXANIMMESH_DECL;

	struct ParsingData
	{
	};

	class ENGINE_DLL ISerializable abstract
	{
	public:
		virtual HRESULT Save_Data(ParsingData* data) PURE;
		virtual HRESULT Load_Data(ParsingData* data) PURE;
	};

	// Load ��ɸ� �����ϰ� ������ ���
	class ENGINE_DLL IReadable : public ISerializable
	{
	public:
		virtual HRESULT Save_Data(ParsingData* data) final { return S_OK; };
	};

	// Save ��ɸ� �����ϰ� ������ ���
	class ENGINE_DLL IWriteable : public ISerializable
	{
	public:
		virtual HRESULT Load_Data(ParsingData* data) final { return S_OK; };
	};

	struct MeshParsingData : public ParsingData
	{
		_char				szName[FILE_NAME_SIZE];
		_uint				iMaterialIndex;
		_uint				iNumVertices;
		_uint				iNumIndices;
		vector<_ulong>		Indices;
		vector<VTXANIMMESH> Vertices;
		_uint				iNumBones;
		vector<_uint>		BoneIndices;
	};

	struct BoneParsingData : public ParsingData
	{
		_char				szName[FILE_NAME_SIZE];
		_float4x4			TransformationMatrix;
		_float4x4			CombinedTransformationMatrix;
		_float4x4			OffsetMatrix;
		_int				iParentIndex;
		_uint				iIndex;

	};

	struct ChannelParsingData : public ParsingData
	{
		_char				szName[FILE_NAME_SIZE];
		_uint				iNumKeyFrames;
		vector<KEYFRAME>	KeyFrames;
		_uint				iBoneIndex;
	};
	struct AnimationParsingData : public ParsingData
	{
		vector<ChannelParsingData>	ChannelDatas;
		_char						szName[FILE_NAME_SIZE];
		_uint						iNumChannels;
		_double						Duration;
		_double						TickPerSecond;
	};

	struct ModelParsingData : public ParsingData
	{
		/////////////////// �� ������ ���� ///////////////
		_uint				iNumMeshes;
		_uint				iNumMaterials;
		vector<char*>		MaterialPaths;
		_uint				iNumAnimations;

		/////////////////// �޽� ���� //////////////////////
		vector<MeshParsingData> MeshDatas;

		////////////////// �� ���� ////////////////////////
		vector<BoneParsingData> BoneDatas;

		///////////////// �ִϸ��̼� ���� ///////////////////
		vector<AnimationParsingData> AnimationDatas;
	};

	struct ObjectParsingData : public ParsingData
	{

	};
}
