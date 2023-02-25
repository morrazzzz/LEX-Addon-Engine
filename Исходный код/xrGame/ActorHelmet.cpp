#include "stdafx.h"
#include "ActorHelmet.h"
#include "Actor.h"
#include "Inventory.h"
#include "Torch.h"
#include "BoneProtections.h"
#include "../Include/xrRender/Kinematics.h"
//#include "CustomOutfit.h"

CHelmet::CHelmet()
{
	m_flags.set(FUsingCondition, TRUE);
	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;

	m_boneProtection = xr_new<SBoneProtections>();
}

CHelmet::~CHelmet()
{
	xr_delete(m_boneProtection);
}

void CHelmet::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn]		= pSettings->r_float(section,"burn_protection");
	m_HitTypeProtection[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_protection");
	m_HitTypeProtection[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_protection");
	m_HitTypeProtection[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_protection");
	m_HitTypeProtection[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_protection");
	m_HitTypeProtection[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_protection");
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn]= pSettings->r_float(section,"chemical_burn_protection");
	m_HitTypeProtection[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_protection");
	// Lex Addon (correct by Suhar_) 15.07.2016		(begin)
	// Включаем чтение параметра fire_wound_protection из свойств шлема (если есть)
	m_HitTypeProtection[ALife::eHitTypeFireWound]	= READ_IF_EXISTS(pSettings, r_float, section, "fire_wound_protection",    0.0f );
	// Lex Addon (correct by Suhar_) 15.07.2016		(end)
//	m_HitTypeProtection[ALife::eHitTypePhysicStrike]= pSettings->r_float(section,"physic_strike_protection");
	m_HitTypeProtection[ALife::eHitTypeLightBurn]	= m_HitTypeProtection[ALife::eHitTypeBurn];
	m_boneProtection->m_fHitFracActor				= pSettings->r_float(section, "hit_fraction_actor");

	if (pSettings->line_exist(section, "nightvision_sect"))
		m_NightVisionSect = pSettings->r_string(section, "nightvision_sect");
	else
		m_NightVisionSect = "";

	m_fHealthRestoreSpeed			= READ_IF_EXISTS(pSettings, r_float, section, "health_restore_speed",    0.0f );
	m_fRadiationRestoreSpeed		= READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed", 0.0f );
	m_fSatietyRestoreSpeed			= READ_IF_EXISTS(pSettings, r_float, section, "satiety_restore_speed",   0.0f );
	m_fPowerRestoreSpeed			= READ_IF_EXISTS(pSettings, r_float, section, "power_restore_speed",     0.0f );
	m_fBleedingRestoreSpeed			= READ_IF_EXISTS(pSettings, r_float, section, "bleeding_restore_speed",  0.0f );
	m_fPowerLoss					= READ_IF_EXISTS(pSettings, r_float, section, "power_loss",    1.0f );
	// Lex Addon (correct by Suhar_) 15.07.2016		(begin)
	// Устанавливаем предел коэффициента потери сил на 10
	clamp							( m_fPowerLoss, 0.0f, 10.0f );
	// Lex Addon (correct by Suhar_) 15.07.2016		(end)

	m_BonesProtectionSect			= READ_IF_EXISTS(pSettings, r_string, section, "bones_koeff_protection",  "" );
	m_fShowNearestEnemiesDistance	= READ_IF_EXISTS(pSettings, r_float, section, "nearest_enemies_show_dist",  0.0f );

}

void CHelmet::ReloadBonesProtection()
{
	CObject* parent = H_Parent();
	if(IsGameTypeSingle())
		parent = smart_cast<CObject*>(Level().CurrentViewEntity());

	if(parent && parent->Visual() && m_BonesProtectionSect.size())
		m_boneProtection->reload( m_BonesProtectionSect, smart_cast<IKinematics*>(parent->Visual()));
}

BOOL CHelmet::net_Spawn(CSE_Abstract* DC)
{
	if(IsGameTypeSingle())
		ReloadBonesProtection();

	BOOL res = inherited::net_Spawn(DC);
	return					(res);
}

void CHelmet::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
	P.w_float_q8(GetCondition(),0.0f,1.0f);
}

void CHelmet::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
	float _cond;
	P.r_float_q8(_cond,0.0f,1.0f);
	SetCondition(_cond);
}

void CHelmet::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
//	ReloadBonesProtection();
}

void CHelmet::OnMoveToSlot(const SInvItemPlace& previous_place)
{
	inherited::OnMoveToSlot		(previous_place);
	if (m_pInventory && (previous_place.type==eItemPlaceSlot))
	{
		CActor* pActor = smart_cast<CActor*> (H_Parent());
		if (pActor)
		{
			CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
			if(pTorch && pTorch->GetNightVisionStatus())
				pTorch->SwitchNightVision(true, false);
		}
	}
}

void CHelmet::OnMoveToRuck(const SInvItemPlace& previous_place)
{
	inherited::OnMoveToRuck		(previous_place);
	if (m_pInventory && (previous_place.type==eItemPlaceSlot))
	{
		CActor* pActor = smart_cast<CActor*> (H_Parent());
		if (pActor)
		{
			CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
			if(pTorch)
				pTorch->SwitchNightVision(false);
		}
	}
}

void CHelmet::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= GetHitImmunity(hit_type);
	ChangeCondition(-hit_power);
}

float CHelmet::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	// Lex Addon (correct by Suhar_) 7.12.2018		(begin)
	// Изменение формулы учёта состояния у шлемов и костюмов
	// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
	return m_HitTypeProtection[hit_type] * (GetCondition() * 0.9 + 0.1);
	// Lex Addon (correct by Suhar_) 7.12.2018		(end)
}

float CHelmet::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	// Lex Addon (correct by Suhar_) 7.12.2018		(begin)
	// Изменение формулы учёта состояния у шлемов и костюмов
	// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
	float fBase = m_HitTypeProtection[hit_type] * (GetCondition() * 0.9 + 0.1);
	// Lex Addon (correct by Suhar_) 7.12.2018		(end)
	float bone = m_boneProtection->getBoneProtection(element);
	return fBase * bone;
}

float CHelmet::GetBoneArmor(s16 element)
{
	return m_boneProtection->getBoneArmor(element);
}
// Lex Addon (correct by Suhar_) 9.07.2016		(begin)
// Функция получения защиты кости
float CHelmet::GetBoneProtection(s16 element)
{
	return m_boneProtection->getBoneProtection(element);
}
// Lex Addon (correct by Suhar_) 9.07.2016		(end)

bool CHelmet::install_upgrade_impl( LPCSTR section, bool test )
{
	bool result = inherited::install_upgrade_impl( section, test );

	result |= process_if_exists( section, "burn_protection",          &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeBurn]        , test );
	result |= process_if_exists( section, "shock_protection",         &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeShock]       , test );
	result |= process_if_exists( section, "strike_protection",        &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeStrike]      , test );
	result |= process_if_exists( section, "wound_protection",         &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeWound]       , test );
	result |= process_if_exists( section, "radiation_protection",     &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeRadiation]   , test );
	result |= process_if_exists( section, "telepatic_protection",     &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeTelepatic]   , test );
	result |= process_if_exists( section, "chemical_burn_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeChemicalBurn], test );
	result |= process_if_exists( section, "explosion_protection",     &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeExplosion]   , test );
	result |= process_if_exists( section, "fire_wound_protection",    &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeFireWound]   , test );

	LPCSTR str;
	bool result2 = process_if_exists_set( section, "nightvision_sect", &CInifile::r_string, str, test );
	if ( result2 && !test )
	{
		m_NightVisionSect._set( str );
	}
	result |= result2;

	// Lex Addon (correct by Suhar_) 27.05.2016		(begin)
	// Добавляем чтение параметра hit_fraction_actor у шлемов
	result |= process_if_exists( section, "hit_fraction_actor", &CInifile::r_float, m_boneProtection->m_fHitFracActor, test );
	// Lex Addon (correct by Suhar_) 27.05.2016		(end)
	
	result |= process_if_exists( section, "health_restore_speed",    &CInifile::r_float, m_fHealthRestoreSpeed,    test );
	result |= process_if_exists( section, "radiation_restore_speed", &CInifile::r_float, m_fRadiationRestoreSpeed, test );
	result |= process_if_exists( section, "satiety_restore_speed",   &CInifile::r_float, m_fSatietyRestoreSpeed,   test );
	result |= process_if_exists( section, "power_restore_speed",     &CInifile::r_float, m_fPowerRestoreSpeed,     test );
	result |= process_if_exists( section, "bleeding_restore_speed",  &CInifile::r_float, m_fBleedingRestoreSpeed,  test );

	result |= process_if_exists( section, "power_loss", &CInifile::r_float, m_fPowerLoss, test );
	// Lex Addon (correct by Suhar_) 23.09.2016		(begin)
	// Устанавливаем предел коэффициента потери сил на 10
	clamp( m_fPowerLoss, 0.0f, 10.0f );
	// Lex Addon (correct by Suhar_) 23.09.2016		(end)

	result |= process_if_exists( section, "nearest_enemies_show_dist",  &CInifile::r_float, m_fShowNearestEnemiesDistance,  test );

	result2 = process_if_exists_set( section, "bones_koeff_protection", &CInifile::r_string, str, test );
	if ( result2 && !test )
	{
		m_BonesProtectionSect	= str;
		ReloadBonesProtection	();
	}
	result2 = process_if_exists_set( section, "bones_koeff_protection_add", &CInifile::r_string, str, test );
	if ( result2 && !test )
		AddBonesProtection	(str);

	return result;
}

void CHelmet::AddBonesProtection(LPCSTR bones_section)
{
	CObject* parent = H_Parent();
	if(IsGameTypeSingle())
		parent = smart_cast<CObject*>(Level().CurrentViewEntity());

	if ( parent && parent->Visual() && m_BonesProtectionSect.size() )
		m_boneProtection->add(bones_section, smart_cast<IKinematics*>( parent->Visual() ) );
}
// Lex Addon (correct by Suhar_) 18.01.2017		(begin)
// Функция вычисления урона, учитывающая характеристики шлема
// Использован тот же принцип, что и в CustomOutfit.cpp
float CHelmet::HitThroughArmor(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type)
{
	float NewHitPower = hit_power;
	float ba = GetBoneArmor(element);
	float bp = GetBoneProtection(element);
	// 7.12.2018	(begin)
	// Изменение формулы учёта состояния у шлемов и костюмов
	// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
	float condition = GetCondition() * 0.9 + 0.1;
	// 7.12.2018	(end)
	float HitFracActor = - (1 - m_boneProtection->m_fHitFracActor) * condition + 1;
	float protect = GetDefHitTypeProtection(hit_type);
	if(hit_type == ALife::eHitTypeFireWound)
	{
		if(bp==0)
			return NewHitPower;
		float BoneArmor = ba * condition;
		if (ap > BoneArmor)
		{
			float d_hit_power = (ap - BoneArmor) / ap;
			if(d_hit_power < HitFracActor)
				d_hit_power = HitFracActor;
			NewHitPower *= d_hit_power;
			NewHitPower /= bp;
		}
		else
		{
			NewHitPower *= HitFracActor;
			NewHitPower += NewHitPower*(1/bp-1)*0.05;
			add_wound = false;
		}
		NewHitPower -= protect;
	}
	else
	{
		if(protect>0.f)
		{
			float HitPart1 = (NewHitPower-protect)*0.75;
			if(HitPart1<0.f)
				HitPart1 = 0.f;
			float HitPart2 = NewHitPower*HitFracActor*0.25;
			NewHitPower = HitPart1+HitPart2;
		}
	}
	if(NewHitPower < 0.f)
		NewHitPower = 0.f;
	Hit(hit_power, hit_type);
	return NewHitPower;
}
// Lex Addon (correct by Suhar_) 18.01.2017		(end)