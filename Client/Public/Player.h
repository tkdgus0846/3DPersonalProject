#pragma once

#include "Client_Defines.h"
#include "Creature.h"


/*
				플레이어 무기(공격,스킬) 콜라이더는 플레이어가 들고있게하고
				그냥 스킬 콜라이더는 스킬 자체가 가지고 있게 한다.
*/

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CAnimInstance;
END

BEGIN(Client)

class CPlayer final : public CCreature
{
	friend class CWeapon;
	friend class CAxe;
	friend class CSlasher;
	friend class CMace;
	friend class CHand;
	friend class CSword;
public:
	enum COLLIDER { COLLIDER_PLAYER, COLLIDER_ATTACK, COLLIDER_SPHERE, COLLIDER_MACE_E, COLLIDER_SWORD_E, COLLIDER_SWORD_Q,  COLLIDER_END };
	enum WEAPONTYPE { WEAPON_HAND, WEAPON_AXE, WEAPON_SLASHER, WEAPON_MACE, WEAPON_SWORD, WEAPON_END};

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	OnCollisionEnter(const Collision* collision) override;
	virtual void	OnCollisionStay(const Collision* collision) override;
	virtual void	OnCollisionExit(const Collision* collision) override;

	void			HitShake_Collision(const Collision* collision);
	virtual void	Get_Damaged(_int Damage) override;
	void			All_Collider_Off();

private:
	void		Collider_On(COLLIDER eCollider);
	void		Collider_Off(COLLIDER eCollider);
	CCollider*	Get_Collider(COLLIDER eCollider);

	void	Move(_float TimeDelta);
	void	PlayerRotate(_float TimeDelta);
	void	AttackMove(_float TimeDelta);
	void	CameraRotate(_float TimeDelta);
	void	CameraZoom(_float TimeDelta);
	void	Dash(_float TimeDelta);

	void	Attack_Combo();
	

	// 스킬들
	void	Skills(const _float& TimeDelta);
	void	Skill_Q(const _float& TimeDelta);
	void	Skill_C(const _float& TimeDelta);
	void	Skill_R(const _float& TimeDelta);
	void	Skill_E(const _float& TimeDelta);
	void	Skill_T(const _float& TimeDelta);

	// 무기 교체 관련 함수들
	void	Select_Weapon();
	void	Change_Weapon();

	/// 애니메이션 키 선택해주는 함수들
	void	Select_ChangingWeaponKey();
	void	Select_MoveKey();
	void	Select_DashKey();
	void	Select_AttackKey();
	void	Select_IdleKey();

	virtual void	Select_AnimationKey() override;

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };	
	CRenderer*					m_pRendererCom = { nullptr };
	CCollider*					m_pColliderCom[COLLIDER_END] = { nullptr };
	class CCamera_Player_Main*	m_pCameraCom = { nullptr };
	CAnimInstance*				m_pAnimInstance = { nullptr };

	// 플레이어 기본 능력치에 관한 변수들.
	const _float				m_fNormalSpeed = { 6.0f };
	const _float				m_fMoveAttackSpeed = { 1.5f };

	
	// 애님 상태 제어 변수들
	_bool						m_bMove = { false };
	_bool						m_bRotate = { false };
	_bool						m_bAttack = { false };
	_bool						m_bDash = { false };

	_bool						m_bSkillQ = { false };
	_bool						m_bSkillC = { false };
	_bool						m_bSkillR = { false };
	_bool						m_bSkillE = { false };
	_bool						m_bSkillT = { false };
	_bool						m_bSkill = { false };

	// 카메라 제어 관련 변수들
	POINT						m_lastCursorPoint;
	_float						m_fCameraRotationX = { 0.f };
	_float						m_fCameraRotationY = { 0.f };
	_float						m_fCameraZoom = { 0.f };
	_float						m_fCurCameraZoom = { 0.f };
	_float3						m_OriginOffset = { 0.f, 12.f, -12.f };
	_float						m_ObjectAttack_CameraShake_TimeAcc = { 0.0 };

	// 이동 관련 변수들
	_float						m_CurHeight = { 0.f };
	_float3						m_MoveDir;
	
	// 무기에 관한 정보들
	WEAPONTYPE					m_eWeaponType = { WEAPON_HAND };
	WEAPONTYPE					m_ePrevWeaponType = { WEAPON_HAND };
	vector<class CWeapon*>		m_Weapons[WEAPON_END];

	

	//무기에 대한 애니메이션 정보
	_int						m_Combo = { 0 };
	_int						m_PrevCombo = { 0 };
	const _int					m_ComboMax = { 3 };
	_bool						m_bAttackPressing = { false };

	// 움직이면서 공격하는 것에 대한 변수들
	_int						m_MoveAttackCheckCombo = { 0 };
	_float						m_MoveAttackTimeAcc = { 0.0f };
	const _float				m_MoveAttackAccel = { -1.7f };
	const _float				m_MoveAttackInitSpeed = { 0.5f };
	_bool						m_bAttackMove = { false };

	// Dash 관련 변수들
	_float3						m_DashDir;
	_float						m_DashTimeAcc = { 0.0f };
	_bool						m_bDashStart = { false };
	const _float				m_DashTime = { 0.5f };
	const _float				m_DashAccel = { 2.3f };
	const _float				m_DashInitSpeed = { 0.1f };

public:
	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual HRESULT	Add_Animations() override;

public:
	/* 원형을 생성한다. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END