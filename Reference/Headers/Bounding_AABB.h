#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingBoxDesc : public CBounding::tagColliderDesc
	{
		_float3		vExtents;

	} BOUNDINGBOX;

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBounding_AABB(const CBounding_AABB& rhs);
	virtual ~CBounding_AABB() = default;

public:
	const BoundingBox* Get_Bounding() const {
		return m_pAABB;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pBoundingDesc);

public:
	virtual void Tick(_fmatrix TransformMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)override;
	// virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding)override;
	virtual _bool Intersect(CCollider::TYPE eType, const CBounding* pBounding)override;
private:
	BoundingBox*				m_pAABB_Original = { nullptr };
	BoundingBox*				m_pAABB = { nullptr };

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	
public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END