#include "stdafx.h"

#include "customoutfit.h"
#include "../xrphysics/PhysicsShell.h"
#include "inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"
#include "BoneProtections.h"
#include "../Include/xrRender/Kinematics.h"
#include "player_hud.h"
#include "ActorHelmet.h"


CCustomOutfit::CCustomOutfit()
{
	m_flags.set(FUsingCondition, TRUE);

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;

	m_boneProtection = xr_new<SBoneProtections>();
	m_artefact_count = 0;
	m_BonesProtectionSect = NULL;
}

CCustomOutfit::~CCustomOutfit() 
{
	xr_delete(m_boneProtection);
}

BOOL CCustomOutfit::net_Spawn(CSE_Abstract* DC)
{
	if(IsGameTypeSingle())
		ReloadBonesProtection();

	BOOL res = inherited::net_Spawn(DC);
	return					(res);
}

void CCustomOutfit::net_Export(NET_Packet& P)
{
	inherited::net_Export	(P);
	P.w_float_q8			(GetCondition(),0.0f,1.0f);
}

void CCustomOutfit::net_Import(NET_Packet& P)
{
	inherited::net_Import	(P);
	float _cond;
	P.r_float_q8			(_cond,0.0f,1.0f);
	SetCondition			(_cond);
}

void CCustomOutfit::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
	if (!IsGameTypeSingle())
		ReloadBonesProtection();
}


void CCustomOutfit::Load(LPCSTR section) 
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
	// �������� ������ ��������� fire_wound_protection �� ������� ������� (���� ����)
	m_HitTypeProtection[ALife::eHitTypeFireWound]	= READ_IF_EXISTS(pSettings, r_float, section, "fire_wound_protection",    0.0f );
	// Lex Addon (correct by Suhar_) 15.07.2016		(end)
//	m_HitTypeProtection[ALife::eHitTypePhysicStrike]= pSettings->r_float(section,"physic_strike_protection");
	m_HitTypeProtection[ALife::eHitTypeLightBurn]	= m_HitTypeProtection[ALife::eHitTypeBurn];
	m_boneProtection->m_fHitFracActor = pSettings->r_float(section, "hit_fraction_actor");

	if (pSettings->line_exist(section, "nightvision_sect"))
		m_NightVisionSect = pSettings->r_string(section, "nightvision_sect");
	else
		m_NightVisionSect = "";

	if (pSettings->line_exist(section, "actor_visual"))
		m_ActorVisual = pSettings->r_string(section, "actor_visual");
	else
		m_ActorVisual = NULL;
	
	// Lex Addon (correct by Suhar_) 1.03.2017		(begin)
	// ��������� ������ ������ �� ��� �� ���������
	m_PlayerHudSection		= pSettings->r_string(section, "player_hud_section");
	// Lex Addon (correct by Suhar_) 1.07.2017		(end)

	m_ef_equipment_type		= pSettings->r_u32(section,"ef_equipment_type");
	m_fPowerLoss			= READ_IF_EXISTS(pSettings, r_float, section, "power_loss",    1.0f );
	// Lex Addon (correct by Suhar_) 15.07.2016		(begin)
	// ������������� ������ ������������ ������ ��� �� 10
	clamp					( m_fPowerLoss, 0.0f, 10.0f );
	// Lex Addon (correct by Suhar_) 15.07.2016		(end)

	m_additional_weight		= pSettings->r_float(section,"additional_inventory_weight");
	m_additional_weight2	= pSettings->r_float(section,"additional_inventory_weight2");

	m_fHealthRestoreSpeed		= READ_IF_EXISTS(pSettings, r_float, section, "health_restore_speed", 		0.0f );
	m_fRadiationRestoreSpeed	= READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed",	0.0f );
	m_fSatietyRestoreSpeed		= READ_IF_EXISTS(pSettings, r_float, section, "satiety_restore_speed",		0.0f );
	m_fPowerRestoreSpeed		= READ_IF_EXISTS(pSettings, r_float, section, "power_restore_speed",		0.0f );
	m_fBleedingRestoreSpeed		= READ_IF_EXISTS(pSettings, r_float, section, "bleeding_restore_speed",		0.0f );
	
	// Lex Addon (correct by Suhar_) 				(begin)
	// ��������� ������ ��������������� �������� �� ������� ������� 4.07.2016
	m_fMaxHealthAdd			 	= READ_IF_EXISTS(pSettings, r_float, section, "max_health_add",				0.0f );
	// ��������� ������ �������������� �������� ������ �� ������� ������� 6.07.2016
	m_fJumpSpeedAdd			 	= READ_IF_EXISTS(pSettings, r_float, section, "jump_speed_add",				0.0f );
	// ��������� ������ �������������� �������� ������������ �� ������� �������
	m_fWalkAccelAdd			 	= READ_IF_EXISTS(pSettings, r_float, section, "walk_accel_add",				0.0f );
	// Lex Addon (correct by Suhar_) 				(end)

	m_full_icon_name			= pSettings->r_string( section, "full_icon_name" );
	m_artefact_count 			= READ_IF_EXISTS( pSettings, r_u32, section, "artefact_count", 0 );
	// Lex Addon (correct by Suhar_) 2.06.2016		(begin)
	// ����������� ������������ ����������� ����� ��� ���������� � 5 �� 10
	clamp( m_artefact_count, (u32)0, (u32)10 );
	// Lex Addon (correct by Suhar_) 2.06.2016		(end)

	m_BonesProtectionSect		= READ_IF_EXISTS(pSettings, r_string, section, "bones_koeff_protection",  "" );
	bIsHelmetAvaliable			= !!READ_IF_EXISTS(pSettings, r_bool, section, "helmet_avaliable", true);
	
	// Lex Addon (correct by Suhar_) 10.08.2018		(begin)
	// ��������� ������ ��������� ��������� ������� ������� � �������
	bCyclicAirBreath			= READ_IF_EXISTS(pSettings, r_bool, section, "cyclic_air_breath",  false);
	// Lex Addon (correct by Suhar_) 10.08.2018		(end)
	
	// Lex Addon (correct by Suhar_) 31.08.2015		(begin)
	// ��������� ������ ��������� ������������� �������� � �������
	m_fShowNearestEnemiesDistance	= READ_IF_EXISTS(pSettings, r_float, section, "nearest_enemies_show_dist",  0.0f );
	// Lex Addon (correct by Suhar_) 31.08.2015		(end)
	
	// Lex Addon (correct by Suhar_) 1.08.2018		(begin)
	// ��������� ������ ��������� ������� ��������� � ��������� �������� ���� � �������
	bIsDozimiterAvaliable		= READ_IF_EXISTS(pSettings, r_bool, section, "dozimeter", false);
	bIsPsyControllerAvaliable	= READ_IF_EXISTS(pSettings, r_bool, section, "psy_controller", false);
	// Lex Addon (correct by Suhar_) 1.08.2018		(end)
}

void CCustomOutfit::ReloadBonesProtection()
{
	CObject* parent = H_Parent();
	if(IsGameTypeSingle())
		parent = smart_cast<CObject*>(Level().CurrentViewEntity());

	if(parent && parent->Visual() && m_BonesProtectionSect.size())
		m_boneProtection->reload( m_BonesProtectionSect, smart_cast<IKinematics*>(parent->Visual()));
}

void CCustomOutfit::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= GetHitImmunity(hit_type);
	ChangeCondition(-hit_power);
}

float CCustomOutfit::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	// Lex Addon (correct by Suhar_) 7.12.2018		(begin)
	// ��������� ������� ����� ��������� � ������ � ��������
	// ����� � ������� ���� ��������� ������ ���� ��� ����� ������ ��������� (10% ������ �� ���������)
	return m_HitTypeProtection[hit_type] * (GetCondition() * 0.9 + 0.1);
	// Lex Addon (correct by Suhar_) 7.12.2018		(end)
}

float CCustomOutfit::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	// Lex Addon (correct by Suhar_) 7.12.2018		(begin)
	// ��������� ������� ����� ��������� � ������ � ��������
	// ����� � ������� ���� ��������� ������ ���� ��� ����� ������ ��������� (10% ������ �� ���������)
	float fBase = m_HitTypeProtection[hit_type] * (GetCondition() * 0.9 + 0.1);
	// Lex Addon (correct by Suhar_) 7.12.2018		(end)
	float bone = m_boneProtection->getBoneProtection(element);
	return fBase*bone;
}

float CCustomOutfit::GetBoneArmor(s16 element)
{
	return m_boneProtection->getBoneArmor(element);
}
// Lex Addon (correct by Suhar_) 9.07.2016		(begin)
// ������� ��������� ������ �����
float CCustomOutfit::GetBoneProtection(s16 element)
{
	return m_boneProtection->getBoneProtection(element);
}
// Lex Addon (correct by Suhar_) 9.07.2016		(end)

// Lex Addon (correct by Suhar_) 18.01.2017		(begin)
// ������� ���������� �����, ����������� �������������� �������
float CCustomOutfit::HitThroughArmor(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type)
{
	// ���������� ������ �����
	float NewHitPower = hit_power;
	// ������������� ������ ����� �� ������� ������
	float ba = GetBoneArmor(element);
	float bp = GetBoneProtection(element);
	// ���������� ��������� �������
	// 7.12.2018	(begin)
	// ��������� ������� ����� ��������� � ������ � ��������
	// ����� � ������� ���� ��������� ������ ���� ��� ����� ������ ��������� (10% ������ �� ���������)
	float condition = GetCondition() * 0.9 + 0.1;
	// 7.12.2018	(end)
	// hit_fraction_actor � ������ ��������� �������
	float HitFracActor = - (1 - m_boneProtection->m_fHitFracActor) * condition + 1;
	// ���������� ������ ������� �� ������� ���� �����
	float protect = GetDefHitTypeProtection(hit_type);
	// ���� ��� ����������� - ���������
	if (hit_type == ALife::eHitTypeFireWound)
	{
		// ��������� �������� ������. ���� ��� �������, �� ������ ������� �� ���������������� �� ������ �����. ���������� ����� �� �����.
		if(bp==0)
			return NewHitPower;
		// ��������� ��������� �����
		float BoneArmor = ba * condition;
		if(ap>BoneArmor)
		// ���� ������� �����
		{
			// ��������� ������� �������� �����
			float d_hit_power = (ap - BoneArmor) / ap;
			// ����������� ���� �� ������ ���� ������ HitFracActor
			if(d_hit_power<HitFracActor)
				d_hit_power = HitFracActor;
			NewHitPower *= d_hit_power;
			// ��������� BoneProtection, ����� ����� ���� ������� �������������� ��� ��� �������� ����� �� �������� ������ ���� (���� ������)
			NewHitPower /= bp;
		}
		else
		// ���� �� ������� �����
		{
			// ��������� ����� ��������������� ��������� hit_fraction
			// NewHitPower *= m_boneProtection->m_fHitFracActor;
			// ��������� ��������� �����
			NewHitPower *= HitFracActor;
			// ��������� ���������� ����� ����	12.01.2017
			NewHitPower += NewHitPower*(1/bp-1)*0.05;
			// ���� ���
			add_wound = false;
		}
		// ��������� �������� ����� (���� ����)
		NewHitPower -= protect;
	}
	else
	{
		// ���� �������� ������������� �� ������ �����������
		// ����� �������� ����� ����� �� ���������� �����������
		if(protect>0.f)
		{
			// NewHitPower -= protect;
			// ���� ����������� �� ��� ����� � ��������� ��� � ������
			// ���� ������ ����� �������������� �� ������������ �����
			float HitPart1 = (NewHitPower-protect)*0.75;
			// ���������� ������������� ��������
			if(HitPart1<0.f)
				HitPart1 = 0.f;
			// ���� ������ �������������� �� ������ hit_fraction_actor
			float HitPart2 = NewHitPower*HitFracActor*0.25;
			// ������� � �������� ���������� ������� � ������� ������� �������� �� ���������� ����������� � ��������
			if ( bCyclicAirBreath && ( hit_type==ALife::eHitTypeChemicalBurn || hit_type == ALife::eHitTypeRadiation ) )
				HitPart2 = 0;
			NewHitPower = HitPart1+HitPart2;
		}
	}
	if(NewHitPower < 0.f)
		NewHitPower = 0.f;
	// ��������� ������������ �������
	Hit(hit_power, hit_type);
	return NewHitPower;
}
// Lex Addon (correct by Suhar_) 18.01.2017		(end)

BOOL	CCustomOutfit::BonePassBullet					(int boneID)
{
	return m_boneProtection->getBonePassBullet(s16(boneID));
}

#include "torch.h"
void	CCustomOutfit::OnMoveToSlot		(const SInvItemPlace& prev)
{
	if ( m_pInventory )
	{
		CActor* pActor = smart_cast<CActor*>( H_Parent() );
		if ( pActor )
		{
			ApplySkinModel(pActor, true, false);
			if (prev.type==eItemPlaceSlot && !bIsHelmetAvaliable)
			{
				CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
				if(pTorch && pTorch->GetNightVisionStatus())
					pTorch->SwitchNightVision(true, false);
			}
			PIItem pHelmet = pActor->inventory().ItemFromSlot(HELMET_SLOT);
			if(pHelmet && !bIsHelmetAvaliable)
				pActor->inventory().Ruck(pHelmet, false);
		}
	}
}

void CCustomOutfit::ApplySkinModel(CActor* pActor, bool bDress, bool bHUDOnly)
{
	if(bDress)
	{
		if(!bHUDOnly && m_ActorVisual.size())
		{
			shared_str NewVisual = NULL;
			char* TeamSection = Game().getTeamSection(pActor->g_Team());
			if (TeamSection)
			{
				if (pSettings->line_exist(TeamSection, *cNameSect()))
				{
					NewVisual = pSettings->r_string(TeamSection, *cNameSect());
					string256 SkinName;

					xr_strcpy(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
					xr_strcat(SkinName, *NewVisual);
					xr_strcat(SkinName, ".ogf");
					NewVisual._set(SkinName);
				}
			}
			if (!NewVisual.size())
				NewVisual = m_ActorVisual;

			pActor->ChangeVisual(NewVisual);
		}

		// Lex Addon (correct by Suhar_) 1.03.2017		(begin)
		// ��������� ������ ������ �� ��� �� ���������
		if (pActor == Level().CurrentViewEntity())
			g_player_hud->load(m_PlayerHudSection);
			// g_player_hud->load(pSettings->r_string(cNameSect(),"player_hud_section"));
		// Lex Addon (correct by Suhar_) 1.03.2017		(end)
	}else
	{
		if (!bHUDOnly && m_ActorVisual.size())
		{
			shared_str DefVisual	= pActor->GetDefaultVisualOutfit();
			if (DefVisual.size())
			{
				pActor->ChangeVisual(DefVisual);
			};
		}

		if (pActor == Level().CurrentViewEntity())	
			g_player_hud->load_default();
	}

}

void	CCustomOutfit::OnMoveToRuck		(const SInvItemPlace& prev)
{
	if(m_pInventory && prev.type==eItemPlaceSlot)
	{
		CActor* pActor = smart_cast<CActor*> (H_Parent());
		if (pActor)
		{
			ApplySkinModel(pActor, false, false);
			CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
			if(pTorch && !bIsHelmetAvaliable)
				pTorch->SwitchNightVision(false);
		}
	}
};

u32	CCustomOutfit::ef_equipment_type	() const
{
	return		(m_ef_equipment_type);
}

bool CCustomOutfit::install_upgrade_impl( LPCSTR section, bool test )
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
//	result |= process_if_exists( section, "physic_strike_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypePhysicStrike], test );
	LPCSTR str;
	bool result2 = process_if_exists_set( section, "nightvision_sect", &CInifile::r_string, str, test );
	if ( result2 && !test )
	{
		m_NightVisionSect._set( str );
	}
	result |= result2;
	
	// Lex Addon (correct by Suhar_) 				(begin)
	CActor* pActor = smart_cast<CActor*>( H_Parent() );
	PIItem pOutfit = NULL;
	PIItem iitem = NULL;
	if ( pActor )
	{
		// ������ � �����
		pOutfit = pActor->inventory().ItemFromSlot(OUTFIT_SLOT);
		// ������, ������� ���������
		iitem = smart_cast<CInventoryItem*>(this);
	}
	// ���� ������������� ��������� ����� ���� 1.03.2017
	bool need_change_skin = false;
	// ��������� ����������� ��������� ����� ������ ������ �� ��������� 1.03.2017
	result2 = process_if_exists_set( section, "actor_visual",			&CInifile::r_string, str, test );
	if ( result2 && !test )
	{
		m_ActorVisual._set( str );
		// ����� ������ ����
		need_change_skin = true;
	}
	result |= result2;
	
	// ��������� ������ ������ �� ��� �� ��������� 1.03.2017
	result2 = process_if_exists_set( section, "player_hud_section",		&CInifile::r_string, str, test );
	if ( result2 && !test )
	{
		m_PlayerHudSection._set( str );
		// ����� ������ ����
		need_change_skin = true;
	}
	result |= result2;
	
	// �������� �� ������������� ������ ���� 1.03.2017
	if ( need_change_skin )
	{
		if ( pActor )
			// ���� ��������� ������ � �����
			if ( pOutfit==iitem )
				ApplySkinModel(pActor, true, false);
	}
	
	// ��������� ����������� ���������/����������� ���� ����� ��� �������� 1.03.2017
	BOOL is_helmet_avaliable;
	result2 = process_if_exists_set( section, "helmet_avaliable",		&CInifile::r_bool, is_helmet_avaliable, test );
	if ( result2 && !test )
	{
		// ������������� ���������� ������� ����� ��������� ��������
		bIsHelmetAvaliable = is_helmet_avaliable;
		// ���������, ����� �� ��������� ���� �����
		if ( !bIsHelmetAvaliable && pActor )
		{
			// ���� ��������� ������ � �����
			if ( pOutfit==iitem )
			{
				// ���� ������� ��� - ���������
				CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
				if ( pTorch && pTorch->GetNightVisionStatus() )
					pTorch->SwitchNightVision(true, false);
				// ���� ����� ���� - �������
				PIItem pHelmet = pActor->inventory().ItemFromSlot(HELMET_SLOT);
				if ( pHelmet )
					// ��������! ��������� ��������� �������� ���������� ����� ����� �� �������;
					// ��� ���������� ����� ���� ����������� � ������, ������ ��������� ��� ����� ����� ������ ����� ����������� ���������;
					// �������������� ������� � ������ ������ �� �������; �������, �� ����, ����������, �� ���� �����;
					// ��� ��������� ������������� ������� ������������� ����� ��������� ������� ���� ��������� ��������� ��������� ��������;
					pActor->inventory().Ruck(pHelmet, false);
			}
		}
	}
	result |= result2;
	
	// Lex Addon (correct by Suhar_) 1.08.2018		(begin)
	// ��������� ������ ��������� ������� ��������� � ��������� �������� ���� � �������
	BOOL is_dozimiter_avaliable;
	result2 = process_if_exists_set( section, "dozimeter",		&CInifile::r_bool, is_dozimiter_avaliable, test );
	if ( result2 && !test )
		bIsDozimiterAvaliable = is_dozimiter_avaliable;
	result |= result2;
	BOOL is_psy_controller_avaliable;
	result2 = process_if_exists_set( section, "psy_controller",	&CInifile::r_bool, is_psy_controller_avaliable, test );
	if ( result2 && !test )
		bIsPsyControllerAvaliable = is_psy_controller_avaliable;
	result |= result2;
	// Lex Addon (correct by Suhar_) 1.08.2018		(end)

	result2 = process_if_exists_set( section, "bones_koeff_protection", &CInifile::r_string, str, test );
	if ( result2 && !test )
	{
		m_BonesProtectionSect	= str;
		ReloadBonesProtection	();
	}
	result2 = process_if_exists_set( section, "bones_koeff_protection_add", &CInifile::r_string, str, test );
	if ( result2 && !test )
		AddBonesProtection	(str);

	result |= result2;
	result |= process_if_exists( section, "hit_fraction_actor", &CInifile::r_float, m_boneProtection->m_fHitFracActor, test );
	
	result |= process_if_exists( section, "additional_inventory_weight",  &CInifile::r_float,  m_additional_weight,  test );
	result |= process_if_exists( section, "additional_inventory_weight2", &CInifile::r_float,  m_additional_weight2, test );

	result |= process_if_exists( section, "health_restore_speed",    &CInifile::r_float, m_fHealthRestoreSpeed,    test );
	// ��������� ������ ��������������� �������� �� ������� ������� 4.07.2016
	result |= process_if_exists( section, "max_health_add",    		 &CInifile::r_float, m_fMaxHealthAdd,    	   test );
	// ��������� ������ �������������� �������� ������ �� ������� ������� 6.07.2016
	result |= process_if_exists( section, "jump_speed_add",    		 &CInifile::r_float, m_fJumpSpeedAdd,		   test );
	// ��������� ������ �������������� �������� ������������ �� ������� �������
	result |= process_if_exists( section, "walk_accel_add",    		 &CInifile::r_float, m_fWalkAccelAdd,		   test );
	// Lex Addon (correct by Suhar_) 				(end)
	result |= process_if_exists( section, "radiation_restore_speed", &CInifile::r_float, m_fRadiationRestoreSpeed, test );
	result |= process_if_exists( section, "satiety_restore_speed",   &CInifile::r_float, m_fSatietyRestoreSpeed,   test );
	result |= process_if_exists( section, "power_restore_speed",     &CInifile::r_float, m_fPowerRestoreSpeed,     test );
	result |= process_if_exists( section, "bleeding_restore_speed",  &CInifile::r_float, m_fBleedingRestoreSpeed,  test );

	result |= process_if_exists( section, "power_loss",				&CInifile::r_float, m_fPowerLoss, test );
	// ������������� ������ ������������ ������ ��� �� 10
	clamp( m_fPowerLoss, 0.0f, 10.0f );
	
	// Lex Addon (correct by Suhar_) 10.08.2018		(begin)
	// ��������� ������ ��������� ��������� ������� ������� � �������
	BOOL value;
	result2 = process_if_exists_set( section, "cyclic_air_breath",	&CInifile::r_bool, value, test );
	if ( result2 && !test )
		bCyclicAirBreath = value;
	result |= result2;
	// Lex Addon (correct by Suhar_) 10.08.2018		(end)
	
	// Lex Addon (correct by Suhar_) 31.08.2015		(begin)
	// ��������� ������ ��������� ������������� �������� � �������
	result |= process_if_exists( section, "nearest_enemies_show_dist",  &CInifile::r_float, m_fShowNearestEnemiesDistance,  test );
	// Lex Addon (correct by Suhar_) 31.08.2015		(end)
	
	result |= process_if_exists( section, "artefact_count", &CInifile::r_u32, m_artefact_count, test );
	// Lex Addon (correct by Suhar_) 2.06.2016		(begin)
	// ����������� ������������ ����������� ����� ��� ���������� � 5 �� 10
	clamp( m_artefact_count, (u32)0, (u32)10 );
	// Lex Addon (correct by Suhar_) 2.06.2016		(end)

	return result;
}

void CCustomOutfit::AddBonesProtection(LPCSTR bones_section)
{
	CObject* parent = H_Parent();
	if(IsGameTypeSingle())
		parent = smart_cast<CObject*>(Level().CurrentViewEntity());

	if ( parent && parent->Visual() && m_BonesProtectionSect.size() )
		m_boneProtection->add(bones_section, smart_cast<IKinematics*>( parent->Visual() ) );
}
