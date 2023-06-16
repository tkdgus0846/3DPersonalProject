#include "Calculator.h"
#include "PipeLine.h"
#include "../Public/VIBuffer_Terrain.h"
#include "../Public/Transform.h"

_bool CCalculator::Raycast(_fvector Origin, _fvector Direction, const vector<Triangle>* TriangleList, _float4* resultPos, _float& Dist)
{
	Dist = 50000.f;
	_bool ResultIsIntersect = false;

	
	for (const auto& Triangle : *TriangleList)
	{
		_float4 vVertex1 = _float4(Triangle.vVertex1.x, Triangle.vVertex1.y, Triangle.vVertex1.z, 1.f);
		_float4 vVertex2 = _float4(Triangle.vVertex2.x, Triangle.vVertex2.y, Triangle.vVertex2.z, 1.f);
		_float4 vVertex3 = _float4(Triangle.vVertex3.x, Triangle.vVertex3.y, Triangle.vVertex3.z, 1.f);

		_vector Vertex1 = XMLoadFloat4(&vVertex1);
		_vector Vertex2 = XMLoadFloat4(&vVertex2);
		_vector Vertex3 = XMLoadFloat4(&vVertex3);

		_float IntersectDist;

		_bool IsIntersects = TriangleTests::Intersects(Origin, Direction, Vertex1, Vertex2, Vertex3, IntersectDist);
		if (IsIntersects == true)
		{
			ResultIsIntersect = true;
			Dist = (Dist > IntersectDist) ? IntersectDist : Dist;
		}
	}

	if (ResultIsIntersect)
	{
		_vector result = Origin + Direction * Dist;
		XMStoreFloat4(resultPos, result);
		resultPos->w = 1;
	}
	
	return ResultIsIntersect;
}

_float4 CCalculator::Picking_OnTerrain(HWND hWnd, _uint winSizeX, _uint winSizeY, CVIBuffer_Terrain* pTerrainBuffer, CTransform* pTerrainTransform)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// ����Ʈ -> ����

	_vector	vMouse;

	vMouse.m128_f32[0] = ptMouse.x / (winSizeX * 0.5f) - 1.f;
	vMouse.m128_f32[1] = ptMouse.y / -(winSizeY * 0.5f) + 1.f;
	vMouse.m128_f32[2] = 0.f;
	vMouse.m128_f32[3] = 1.f;

	//  ���� -> �� �����̽�
	_matrix		matProjInverse, matViewInverse;
	matProjInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	matViewInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	vMouse = XMVector3TransformCoord(vMouse, matProjInverse);

	// �� -> ���� �����̽�
	_vector	vRayPos, vRayDir;

	vRayPos = { 0.f,0.f,0.f,1.f };
	vRayDir = vMouse - vRayPos;

	vRayDir = XMVector3Normalize(vRayDir);

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	// ���� -> ���� �����̽�
	_matrix		matWorldInverse = pTerrainTransform->Get_WorldMatrix_Inverse();

	vRayPos = XMVector3TransformCoord(vRayPos, matWorldInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matWorldInverse);

	_float4 resultPos = { 0.f,0.f,0.f,1.f };
	_float dist;

	// pTerrainBuffer, pTerrainTransform �̿��Ͽ� 3��° �Ű����� ����.
	vector<Triangle>* TriangleVector = pTerrainBuffer->Get_VerticesPos();
	_bool rayHit = Raycast(vRayPos, vRayDir, TriangleVector, &resultPos, dist);

	_vector resPos = XMVector3TransformCoord(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&pTerrainTransform->Get_WorldFloat4x4()));
	
	XMStoreFloat4(&resultPos, resPos);
	return resultPos;
}