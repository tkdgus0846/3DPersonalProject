#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingBoxDesc : public CBounding::tagColliderDesc
	{
		_float3		vExtents;
		_float3		vRotation;
	} BOUNDINGBOX;

	typedef struct tagOBB
	{
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	}OBBDESC;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBounding_OBB(const CBounding_OBB& rhs);
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Bounding() const {
		return m_pOBB;
	}
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pBoundingDesc);

public:
	virtual void Tick(_fmatrix TransformMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)override;
	// virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding) override;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding) override;

public:
	CBounding_OBB::OBBDESC Compute_OBBDesc();
		 
private:
	BoundingOrientedBox*		m_pOBB_Original = { nullptr };
	BoundingOrientedBox*		m_pOBB = { nullptr };
	
public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END