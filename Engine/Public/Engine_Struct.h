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
		XMUINT4			vBlendIndices; /* 이 정점은 어떤 뼈들(최대4개)의 상태행렬 받아서 처리되야하는가?! : 뼈(모델X, 메시O)의 인덱스 */
		XMFLOAT4		vBlendWeights; /* 네개뼈를 이용하되 각 뼈의 상태행렬을 몇퍼센트(1을 기준으로 표현한 비율)나 이용하는가?! */
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

	// Load 기능만 구현하고 싶으면 상속
	class IReadable : public ISerializable
	{
	public:
		virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) final { return nullptr; };
	};

	// Save 기능만 구현하고 싶으면 상속
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
		/////////////////// 모델 변수들 저장 ///////////////
		_uint				iNumMeshes;
		_uint				iNumMaterials;
		_uint				iNumAnimations;
		_uint				iNumBones;

		///////////////// 텍스쳐 경로들 저장 ////////////
		vector<char*>		MaterialPaths;

		/////////////////// 메쉬 저장 //////////////////////
		vector<MeshParsingData> MeshDatas;

		////////////////// 뼈 저장 ////////////////////////
		vector<BoneParsingData> BoneDatas;

		///////////////// 애니메이션 저장 ///////////////////
		vector<AnimationParsingData> AnimationDatas;
	};

	struct ObjectParsingData : public ParsingData
	{
		TransformParsingData	TransformData;
	};

	
}
