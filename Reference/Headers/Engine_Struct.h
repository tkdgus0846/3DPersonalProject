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

	class ISerializable abstract
	{
	public:
		virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) PURE;
		virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) PURE;
	};

	// Load ��ɸ� �����ϰ� ������ ���
	class IReadable : public ISerializable
	{
	public:
		virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) final { return nullptr; };
	};

	// Save ��ɸ� �����ϰ� ������ ���
	class IWriteable : public ISerializable
	{
	public:
		virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final { return nullptr; };
	};

	struct LevelParsingData : public ParsingData
	{
		LevelParsingData(const wstring& levelName)
		{
			lstrcpy(LevelName, levelName.c_str());
		}
		_tchar				LevelName[MAX_PATH];
		
	};

	struct TransformParsingData : public ParsingData
	{
		_float4x4 WorldMatrix;
	};

	struct MeshParsingData : public ParsingData
	{
		_char				szName[MAX_PATH];
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
		_char				szName[MAX_PATH];
		_float4x4			TransformationMatrix;
		_float4x4			CombinedTransformationMatrix;
		_float4x4			OffsetMatrix;
		_int				iParentIndex;
		_uint				iIndex;

	};

	struct ChannelParsingData : public ParsingData
	{
		_char				szName[MAX_PATH];
		_uint				iNumKeyFrames;
		vector<KEYFRAME>	KeyFrames;
		_uint				iBoneIndex;
	};
	struct AnimationParsingData : public ParsingData
	{
		_uint						iNumChannels;
		vector<ChannelParsingData>	ChannelDatas;
		_char						szName[MAX_PATH];
		_double						Duration;
		_double						TickPerSecond;
	};

	struct ModelParsingData : public ParsingData
	{
		/////////////////// �� ������ ���� ///////////////
		_uint				iNumMeshes;
		_uint				iNumMaterials;
		_uint				iNumAnimations;
		_uint				iNumBones;

		///////////////// �ؽ��� ��ε� ���� ////////////
		vector<char*>		MaterialPaths;

		/////////////////// �޽� ���� //////////////////////
		vector<MeshParsingData> MeshDatas;

		////////////////// �� ���� ////////////////////////
		vector<BoneParsingData> BoneDatas;

		///////////////// �ִϸ��̼� ���� ///////////////////
		vector<AnimationParsingData> AnimationDatas;
	};

	struct ObjectParsingData : public ParsingData
	{
		TransformParsingData	TransformData;
	};

	
}
