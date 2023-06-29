#pragma once


#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagColliderDesc
	{
		COLGROUP	eColGroup;
		_float3		vPosition;
	}BOUNDINGDESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBounding(const CBounding& rhs);
	virtual ~CBounding() = default;

public:
	virtual void Tick(_fmatrix TransformMatrix) = 0;
	virtual HRESULT Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch) = 0;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding) = 0;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_bool					m_isColl = { false };

public:
	virtual CBounding* Clone(void* pBoundingDesc) = 0;
	virtual void Free() override;
};

END