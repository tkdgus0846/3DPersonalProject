
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>

#include "Engine_Defines.h"


using namespace std::filesystem;

#define MAX_PATH 256

using namespace DirectX;

using namespace std;

//typedef struct tagKeyFrame
//{
//	XMFLOAT3		vScale;
//	XMFLOAT4		vRotation;
//	XMFLOAT3		vTranslation;
//	double			Time;
//}KEYFRAME;
//
//typedef struct tagVertex_AnimMesh
//{
//	XMFLOAT3		vPosition;
//	XMFLOAT3		vNormal;
//	XMFLOAT2		vTexCoord;
//	XMFLOAT3		vTangent;
//	XMUINT4			vBlendIndices; /* �� ������ � ����(�ִ�4��)�� ������� �޾Ƽ� ó���Ǿ��ϴ°�?! : ��(��X, �޽�O)�� �ε��� */
//	XMFLOAT4		vBlendWeights; /* �װ����� �̿��ϵ� �� ���� ��������� ���ۼ�Ʈ(1�� �������� ǥ���� ����)�� �̿��ϴ°�?! */
//}VTXANIMMESH;
//
//typedef struct tagVertex_Mesh
//{
//	XMFLOAT3		vPosition;
//	XMFLOAT3		vNormal;
//	XMFLOAT2		vTexCoord;
//	XMFLOAT3		vTangent;
//}VTXMESH;

#define MAX_PATH 256

#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}
#define			USING(NAMESPACE)	using namespace NAMESPACE;


#define NULL_CHECK( _ptr)	\
	{if( _ptr == 0){__debugbreak();return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__debugbreak();return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);__debugbreak();}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);__debugbreak();return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); __debugbreak(); return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); __debugbreak(); return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); __debugbreak();return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); __debugbreak();return _return;}

typedef		signed char				_byte;
typedef		char					_char;
typedef		unsigned char			_ubyte;


typedef		wchar_t					_tchar;

typedef		signed short			_short;
typedef		unsigned short			_ushort;

typedef		signed int				_int;
typedef		unsigned int			_uint;

typedef		signed long				_long;
typedef		unsigned long			_ulong;

typedef		float					_float;
typedef		double					_double;

typedef		bool					_bool;

typedef		XMFLOAT2				_float2;
typedef		XMFLOAT3				_float3;
typedef		XMFLOAT4				_float4;
typedef		XMVECTOR				_vector;
typedef		FXMVECTOR				_fvector;

typedef		XMFLOAT4X4				_float4x4;
typedef		XMMATRIX				_matrix;
typedef		FXMMATRIX				_fmatrix;