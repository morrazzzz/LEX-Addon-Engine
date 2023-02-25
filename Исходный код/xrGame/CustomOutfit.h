#pragma once

#include "inventory_item_object.h"

struct SBoneProtections;

class CCustomOutfit: public CInventoryItemObject {
private:
    typedef	CInventoryItemObject inherited;
public:
							CCustomOutfit		();
	virtual					~CCustomOutfit		();

	virtual void			Load				(LPCSTR section);
	
	//уменьшенная версия хита, для вызова, когда костюм надет на персонажа
	virtual void			Hit					(float P, ALife::EHitType hit_type);

	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//если на персонаже надет костюм
	float					GetHitTypeProtection		(ALife::EHitType hit_type, s16 element);
	float					GetDefHitTypeProtection		(ALife::EHitType hit_type);
	float					GetBoneArmor				(s16 element);
	// Lex Addon (correct by Suhar_) 9.07.2016		(begin)
	// Функция получения защиты кости
	float					GetBoneProtection			(s16 element);
	// Lex Addon (correct by Suhar_) 9.07.2016		(end)

	float					HitThroughArmor		(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type);

	virtual void			OnMoveToSlot		(const SInvItemPlace& prev);
	virtual void			OnMoveToRuck		(const SInvItemPlace& previous_place);
	virtual void			OnH_A_Chield		();

protected:
	HitImmunity::HitTypeSVec		m_HitTypeProtection;

	shared_str				m_ActorVisual;
	// Lex Addon (correct by Suhar_) 1.03.2017		(begin)
	// Добавлено чтение ссылки на худ из апгрейдов
	shared_str				m_PlayerHudSection;
	// Lex Addon (correct by Suhar_) 1.03.2017		(end)
	shared_str				m_full_icon_name;
	SBoneProtections*		m_boneProtection;	
protected:
	u32						m_ef_equipment_type;
	u32						m_artefact_count;

public:
	float					m_fPowerLoss;
	float					m_additional_weight;
	float					m_additional_weight2;

	float					m_fHealthRestoreSpeed;
	float 					m_fRadiationRestoreSpeed;
	float 					m_fSatietyRestoreSpeed;
	float					m_fPowerRestoreSpeed;
	float					m_fBleedingRestoreSpeed;
	// Lex Addon (correct by Suhar_) 				(begin)
	// Добавляем чтение параметра дополнительного здоровья у костюма 4.07.2016
	float					m_fMaxHealthAdd;
	// Добавляем чтение параметра дополнительной скорости прыжка у костюма 6.07.2016
	float					m_fJumpSpeedAdd;
	// Добавляем чтение параметра дополнительной скорости передвижения у костюма
	float					m_fWalkAccelAdd;
	// Lex Addon (correct by Suhar_) 				(end)

	shared_str				m_BonesProtectionSect;
	shared_str				m_NightVisionSect;
	// Lex Addon (correct by Suhar_) 31.08.2015		(begin)
	// Добавляем чтение параметра инфракрасного сканнера у костюма
	float					m_fShowNearestEnemiesDistance;
	// Lex Addon (correct by Suhar_) 31.08.2015		(end)

	bool					bIsHelmetAvaliable;
	
	// Lex Addon (correct by Suhar_) 10.08.2018		(begin)
	// Добавляем чтение параметра замкнутой системы дыхания у костюма
	bool					bCyclicAirBreath;
	// Lex Addon (correct by Suhar_) 10.08.2018		(end)
	
	// Lex Addon (correct by Suhar_) 1.08.2018		(begin)
	// Добавляем чтение параметра наличия дозиметра и эмулятора мозговых волн у костюма
	bool					bIsDozimiterAvaliable;
	bool					bIsPsyControllerAvaliable;
	// Lex Addon (correct by Suhar_) 1.08.2018		(end)

	virtual u32				ef_equipment_type		() const;
	virtual	BOOL			BonePassBullet			(int boneID);
	const shared_str&		GetFullIconName			() const	{ return m_full_icon_name; }
	u32						get_artefact_count		() const	{ return m_artefact_count; }

	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual void			net_Export				(NET_Packet& P);
	virtual void			net_Import				(NET_Packet& P);
			void			ApplySkinModel			(CActor* pActor, bool bDress, bool bHUDOnly);
			void			ReloadBonesProtection	();
			void			AddBonesProtection		(LPCSTR bones_section);

protected:
	virtual bool			install_upgrade_impl( LPCSTR section, bool test );
};
