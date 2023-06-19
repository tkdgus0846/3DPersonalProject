#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)


class ENGINE_DLL CCalculator
{
public:
    static _bool Raycast(_fvector Origin, _fvector Direction, const vector<Triangle>* TriangleList, _float4* resultPos, _float& Dist);

    static _bool Picking_OnTerrain(HWND hWnd, _uint winSizeX, _uint winSizeY, class CVIBuffer_Terrain* pTerrainBuffer, class CTransform* pTerrainTransform, _float4* resultPos);
};

END