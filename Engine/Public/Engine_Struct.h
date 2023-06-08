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
}
