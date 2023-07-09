#include "Calculator.h"
#include "PipeLine.h"
#include "../Public/VIBuffer_Terrain.h"
#include "../Public/Transform.h"
#include "GameObject.h"

_int CCalculator::Raycast(_fvector Origin, _fvector Direction, const vector<Triangle>* TriangleList, _float4* resultPos, _float& Dist)
{
	Dist = 50000.f;
	_int ResultIsIntersect = -1;

	
	_int index = -1;
	for (const auto& Triangle : *TriangleList)
	{
		index++;

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
			if (Dist > IntersectDist)
			{
				Dist = IntersectDist;
				ResultIsIntersect = index;
			}
		}
	}

	if (ResultIsIntersect >= 0)
	{
		_vector result = Origin + Direction * Dist;
		XMStoreFloat4(resultPos, result);
		resultPos->w = 1;
	}
	
	return ResultIsIntersect;
}

_int CCalculator::Raycast(_fvector Origin, _fvector Direction, const vector<BoundingSphere>* SphereList, _float4* resultPos, _float& Dist)
{
	Dist = 50000.f;
	_bool ResultIsIntersect = false;

	_int resultIndex = -1;
	_int index = 0;
	for (const auto& sphere : *SphereList)
	{
		_float IntersectDist;
	
		_bool IsIntersects = sphere.Intersects(Origin, Direction, IntersectDist);
		if (IsIntersects == true)
		{
			ResultIsIntersect = true;
			if (Dist > IntersectDist)
			{
				Dist = IntersectDist;
				resultIndex = index;
			}
			
		}
		index++;
	}

	if (ResultIsIntersect)
	{
		_vector result = Origin + Direction * Dist;
		XMStoreFloat4(resultPos, result);
		resultPos->w = 1;
	}

	return resultIndex;
}

_int CCalculator::Picking_OnTerrain(HWND hWnd, _uint winSizeX, _uint winSizeY, CVIBuffer_Terrain* pTerrainBuffer, CTransform* pTerrainTransform, _float4* resultPos)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	if (!PtInRect(&clientRect, ptMouse))
	{
		return -1;
	}

	// 뷰포트 -> 투영
	_vector	vMouse;

	vMouse.m128_f32[0] = ptMouse.x / (winSizeX * 0.5f) - 1.f;
	vMouse.m128_f32[1] = ptMouse.y / -(winSizeY * 0.5f) + 1.f;
	vMouse.m128_f32[2] = 0.f;
	vMouse.m128_f32[3] = 1.f;

	//  투영 -> 뷰 스페이스
	_matrix		matProjInverse, matViewInverse;
	matProjInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	matViewInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	vMouse = XMVector3TransformCoord(vMouse, matProjInverse);

	// 뷰 -> 월드 스페이스
	_vector	vRayPos, vRayDir;

	vRayPos = { 0.f,0.f,0.f,1.f };
	vRayDir = vMouse - vRayPos;

	vRayDir = XMVector3Normalize(vRayDir);

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	// 월드 -> 로컬 스페이스
	_matrix		matWorldInverse = pTerrainTransform->Get_WorldMatrix_Inverse();

	vRayPos = XMVector3TransformCoord(vRayPos, matWorldInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matWorldInverse);

	//_float4 resultPos = { 0.f,0.f,0.f,1.f };
	_float dist;

	// pTerrainBuffer, pTerrainTransform 이용하여 3번째 매개변수 대입.
	vector<Triangle>* TriangleVector = pTerrainBuffer->Get_VerticesPos();
	_int rayHit = Raycast(vRayPos, vRayDir, TriangleVector, resultPos, dist);

	_vector resPos = XMVector3TransformCoord(XMLoadFloat4(resultPos), XMLoadFloat4x4(&pTerrainTransform->Get_WorldFloat4x4()));
	
	XMStoreFloat4(resultPos, resPos);
	return rayHit;
}

_int CCalculator::Picking_Triangle(HWND hWnd, _uint winSizeX, _uint winSizeY, const vector<Triangle>* TriangleList, _float4* resultPos)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	if (!PtInRect(&clientRect, ptMouse))
	{
		return -1;
	}

	// 뷰포트 -> 투영
	_vector	vMouse;

	vMouse.m128_f32[0] = ptMouse.x / (winSizeX * 0.5f) - 1.f;
	vMouse.m128_f32[1] = ptMouse.y / -(winSizeY * 0.5f) + 1.f;
	vMouse.m128_f32[2] = 0.f;
	vMouse.m128_f32[3] = 1.f;

	//  투영 -> 뷰 스페이스
	_matrix		matProjInverse, matViewInverse;
	matProjInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	matViewInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	vMouse = XMVector3TransformCoord(vMouse, matProjInverse);

	// 뷰 -> 월드 스페이스
	_vector	vRayPos, vRayDir;

	vRayPos = { 0.f,0.f,0.f,1.f };
	vRayDir = vMouse - vRayPos;

	vRayDir = XMVector3Normalize(vRayDir);

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	_float dist;

	_int resultIndex = Raycast(vRayPos, vRayDir, TriangleList, resultPos, dist);
	return resultIndex;
}

_int CCalculator::Picking_Sphere(HWND hWnd, _uint winSizeX, _uint winSizeY, const vector<BoundingSphere>* SphereList, _float4* resultPos)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	if (!PtInRect(&clientRect, ptMouse))
	{
		return -1;
	}

	// 뷰포트 -> 투영
	_vector	vMouse;

	vMouse.m128_f32[0] = ptMouse.x / (winSizeX * 0.5f) - 1.f;
	vMouse.m128_f32[1] = ptMouse.y / -(winSizeY * 0.5f) + 1.f;
	vMouse.m128_f32[2] = 0.f;
	vMouse.m128_f32[3] = 1.f;

	//  투영 -> 뷰 스페이스
	_matrix		matProjInverse, matViewInverse;
	matProjInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	matViewInverse = CPipeLine::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	vMouse = XMVector3TransformCoord(vMouse, matProjInverse);

	// 뷰 -> 월드 스페이스
	_vector	vRayPos, vRayDir;

	vRayPos = { 0.f,0.f,0.f,1.f };
	vRayDir = vMouse - vRayPos;

	vRayDir = XMVector3Normalize(vRayDir);

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	_float dist;

	_int resultIndex = Raycast(vRayPos, vRayDir, SphereList, resultPos, dist);
	return resultIndex;
}

_float CCalculator::Distance_Vector_X(_fvector AVector, _fvector BVector)
{
	_vector AClipVec = _vector({ AVector.m128_f32[0], 0.f, 0.f });
	_vector BClipVec = _vector({ BVector.m128_f32[0], 0.f, 0.f});

	_vector subVec = AClipVec - BClipVec;
	return XMVector3Length(subVec).m128_f32[0];
}

_float CCalculator::Distance_Vector_Y(_fvector AVector, _fvector BVector)
{
	_vector AClipVec = _vector({ 0.f, AVector.m128_f32[1], 0.f });
	_vector BClipVec = _vector({ 0.f, BVector.m128_f32[1], 0.f });

	_vector subVec = AClipVec - BClipVec;
	return XMVector3Length(subVec).m128_f32[0];
}

_float CCalculator::Distance_Vector_Z(_fvector AVector, _fvector BVector)
{
	_vector AClipVec = _vector({ 0.f, 0.f, AVector.m128_f32[2] });
	_vector BClipVec = _vector({ 0.f, 0.f, AVector.m128_f32[2] });

	_vector subVec = AClipVec - BClipVec;
	return XMVector3Length(subVec).m128_f32[0];
}

_float CCalculator::Distance_Vector_XZ(_fvector AVector, _fvector BVector)
{
	_vector AClipVec = _vector({ AVector.m128_f32[0],0.f,AVector.m128_f32[2] });
	_vector BClipVec = _vector({ BVector.m128_f32[0],0.f,BVector.m128_f32[2] });

	_vector subVec = AClipVec - BClipVec;
	return XMVector3Length(subVec).m128_f32[0];
}

_float CCalculator::Distance_Vector_XYZ(_fvector AVector, _fvector BVector)
{
	_vector subVec = AVector - BVector;
	return XMVector3Length(subVec).m128_f32[0];
}
