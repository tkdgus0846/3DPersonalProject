#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)


class ENGINE_DLL CCalculator
{
public:
    static _int Raycast(_fvector Origin, _fvector Direction, const vector<Triangle>* TriangleList, _float4* resultPos, _float& Dist);

    static _int Raycast(_fvector Origin, _fvector Direction, const vector<BoundingSphere>* SphereList, _float4* resultPos, _float& Dist);


    // �Ʒ� �Լ����� ������ ��ŷ�ϱ����ؼ� �����Լ��̴�.

    // -1 �̸� ��ŷ�� �ƿ� �ȵȰŰ� 0���� ũ�� ��ŷ�� �Ȱ���.
    static _int Picking_OnTerrain(HWND hWnd, _uint winSizeX, _uint winSizeY, class CVIBuffer_Terrain* pTerrainBuffer, class CTransform* pTerrainTransform, _float4* resultPos);

    static _int Picking_Triangle(HWND hWnd, _uint winSizeX, _uint winSizeY, const vector<Triangle>* TriangleList, _float4* resultPos);

    // ���� �ε����� ����ִ� ������ �Լ� -1 �̸� ��ŷ�� ����������
    static _int Picking_Sphere(HWND hWnd, _uint winSizeX, _uint winSizeY, const vector<BoundingSphere>* SphereList, _float4* resultPos);

    static _float Distance_Vector_X(_fvector AVector, _fvector BVector);
    static _float Distance_Vector_Y(_fvector AVector, _fvector BVector);
    static _float Distance_Vector_Z(_fvector AVector, _fvector BVector);
    static _float Distance_Vector_XZ(_fvector AVector, _fvector BVector);
    static _float Distance_Vector_XYZ(_fvector AVector, _fvector BVector);
};

END