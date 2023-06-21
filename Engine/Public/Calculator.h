#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)


class ENGINE_DLL CCalculator
{
public:
    static _bool Raycast(_fvector Origin, _fvector Direction, const vector<Triangle>* TriangleList, _float4* resultPos, _float& Dist);

    static _int Raycast(_fvector Origin, _fvector Direction, const vector<BoundingSphere>* SphereList, _float4* resultPos, _float& Dist);


    // 아래 함수들은 툴에서 피킹하기위해서 만든함수이다.
    static _bool Picking_OnTerrain(HWND hWnd, _uint winSizeX, _uint winSizeY, class CVIBuffer_Terrain* pTerrainBuffer, class CTransform* pTerrainTransform, _float4* resultPos);

    static _bool Picking_Triangle(HWND hWnd, _uint winSizeX, _uint winSizeY, const vector<Triangle>* TriangleList, _float4* resultPos);

    // 구의 인덱스를 뱉어주는 형태의 함수
    static _int Picking_Sphere(HWND hWnd, _uint winSizeX, _uint winSizeY, const vector<BoundingSphere>* SphereList, _float4* resultPos);
};

END