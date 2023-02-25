#include "StdAfx.h"
#include "UIOutfitInfo.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UIDoubleProgressBar.h"

#include "..\CustomOutfit.h"
#include "..\ActorHelmet.h"
#include "..\string_table.h"
#include "..\actor.h"
#include "..\ActorCondition.h"
#include "..\player_hud.h"


LPCSTR immunity_names[]=
{
	"burn_immunity",
	"shock_immunity",
	"chemical_burn_immunity",
	"radiation_immunity",
	"telepatic_immunity",
	"wound_immunity",
	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Отображение параметра "броня" теперь работает иначе
	//"fire_wound_immunity",
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)
	//"strike_immunity",
	//"explosion_immunity",
};

LPCSTR immunity_st_names[]=
{
	"ui_inv_outfit_burn_protection",
	"ui_inv_outfit_shock_protection",
	"ui_inv_outfit_chemical_burn_protection",
	"ui_inv_outfit_radiation_protection",
	"ui_inv_outfit_telepatic_protection",
	"ui_inv_outfit_wound_protection",
	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Отображение параметра "броня" теперь работает иначе
	//"ui_inv_outfit_fire_wound_protection",
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)
	//"ui_inv_outfit_strike_protection",
	//"ui_inv_outfit_explosion_protection",
};

CUIOutfitImmunity::CUIOutfitImmunity()
{
	AttachChild( &m_name );
	AttachChild( &m_progress );
	AttachChild( &m_value );
	m_magnitude = 1.0f;
}

CUIOutfitImmunity::~CUIOutfitImmunity()
{
}

void CUIOutfitImmunity::InitFromXml( CUIXml& xml_doc, LPCSTR base_str, u32 hit_type )
{
	CUIXmlInit::InitWindow( xml_doc, base_str, 0, this );

	string256 buf;
	
	strconcat( sizeof(buf), buf, base_str, ":", immunity_names[hit_type] );
	CUIXmlInit::InitWindow( xml_doc, buf, 0, this );
	CUIXmlInit::InitStatic( xml_doc, buf, 0, &m_name );
	m_name.TextItemControl()->SetTextST( immunity_st_names[hit_type] );

	strconcat( sizeof(buf), buf, base_str, ":", immunity_names[hit_type], ":progress_immunity" );
	m_progress.InitFromXml( xml_doc, buf );
	
	strconcat( sizeof(buf), buf, base_str, ":", immunity_names[hit_type], ":static_value" );
	m_value.SetVisible( false );

	m_magnitude = xml_doc.ReadAttribFlt( buf, 0, "magnitude", 1.0f );
}

void CUIOutfitImmunity::SetProgressValue( float cur, float comp )
{
	cur  *= m_magnitude;
	comp *= m_magnitude;
	m_progress.SetTwoPos( cur, comp );
	string32 buf;
//	xr_sprintf( buf, sizeof(buf), "%d %%", (int)cur );
	xr_sprintf( buf, sizeof(buf), "%.0f", cur );
	m_value.SetText( buf );
}

// ===========================================================================================

CUIOutfitInfo::CUIOutfitInfo()
{
	for (u32 i = 0; i < max_count; ++i)
	{
		if (i==ALife::eHitTypeFireWound)
		{
			continue;
		}
		m_items[i] = NULL;
	}
	
	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Расширение алгоритма отображения свойств у костюмов и шлемов
	// Инициализация визуальных элементов:
	// Иконка
	AttachChild(&m_staticHeadArmor);
	// Название свойства
	AttachChild(&m_textHeadArmorName);
	// Значение параметра
	AttachChild(&m_textHeadArmorValue);
	AttachChild(&m_staticBodyArmor);
	AttachChild(&m_textBodyArmorName);
	AttachChild(&m_textBodyArmorValue);
	AttachChild(&m_staticPowerLoss);
	AttachChild(&m_textPowerLossName);
	AttachChild(&m_textPowerLossValue);
	AttachChild(&m_staticSprintAllowed);
	AttachChild(&m_textSprintAllowedValue);
	AttachChild(&m_staticAdditionalWeight);
	AttachChild(&m_textAdditionalWeightName);
	AttachChild(&m_textAdditionalWeightValue);
	AttachChild(&m_staticAdditionalWalkSpeed);
	AttachChild(&m_textAdditionalWalkSpeedName);
	AttachChild(&m_textAdditionalWalkSpeedValue);
	AttachChild(&m_staticAdditionalJumpSpeed);
	AttachChild(&m_textAdditionalJumpSpeedName);
	AttachChild(&m_textAdditionalJumpSpeedValue);
	AttachChild(&m_staticPowerRestoreSpeed);
	AttachChild(&m_textPowerRestoreSpeedName);
	AttachChild(&m_textPowerRestoreSpeedValue);
	AttachChild(&m_staticRadiationRestoreSpeed);
	AttachChild(&m_textRadiationRestoreSpeedName);
	AttachChild(&m_textRadiationRestoreSpeedValue);
	AttachChild(&m_staticBleedingRestoreSpeed);
	AttachChild(&m_textBleedingRestoreSpeedName);
	AttachChild(&m_textBleedingRestoreSpeedValue);
	AttachChild(&m_staticHealthRestoreSpeed);
	AttachChild(&m_textHealthRestoreSpeedName);
	AttachChild(&m_textHealthRestoreSpeedValue);
	AttachChild(&m_staticMaxHealthAdd);
	AttachChild(&m_textMaxHealthAddName);
	AttachChild(&m_textMaxHealthAddValue);
	AttachChild(&m_staticArtefactCount);
	AttachChild(&m_textArtefactCountName);
	AttachChild(&m_textArtefactCountValue);
	AttachChild(&m_staticNightvision);
	AttachChild(&m_textNightvisionName);
	AttachChild(&m_textNightvisionValue);
	AttachChild(&m_staticEnemiesScanner);
	AttachChild(&m_textEnemiesScannerValue);
	AttachChild(&m_staticDozimeter);
	AttachChild(&m_textDozimeterValue);
	AttachChild(&m_staticPsyController);
	AttachChild(&m_textPsyControllerValue);
	// Добавочная высота окна
	AdditionalHeight = 0;
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)
}

CUIOutfitInfo::~CUIOutfitInfo()
{
	for (u32 i = 0; i < max_count; ++i)
	{
		if (i==ALife::eHitTypeFireWound)
		{
			continue;
		}
		xr_delete( m_items[i] );
	}
}


// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
// Расширение алгоритма отображения свойств у костюмов и шлемов
// Количество отображаемых свойств = 17
// Таблица положения по высоте отображаемых позиций. Необходима для построения правильного списка.
//		Содержит координату высоты для каждой ячейки свойства
int			PropHeight[17];
// Таблица множителей для показателя каждого улучшения. Необходима для интерпретации исходных значений в понятные игроку.  
float		PropMagnitude[17];
// Переменная количества знаков после запятой для дополнительного переносимого веса
int			AdditionalWeightAccuracy = 2;
// Таблица постфиксов. Содержит необходимые игроку пояснения для значений свойств. Напимер, текст единиц измерения.
LPCSTR		PropPostfix[17];
// Lex Addon (correct by Suhar_) 5.08.2018		(end)

void CUIOutfitInfo::InitFromXml( CUIXml& xml_doc )
{
	LPCSTR base_str	= "outfit_info";

	CUIXmlInit::InitWindow( xml_doc, base_str, 0, this );
	
	string128 buf;
	//m_caption = xr_new<CUIStatic>();
	//AttachChild( m_caption );
	//m_caption->SetAutoDelete( true );	
	//string128 buf;
	//strconcat( sizeof(buf), buf, base_str, ":caption" );
	//CUIXmlInit::InitStatic( xml_doc, buf, 0, m_caption );

	m_Prop_line = xr_new<CUIStatic>();
	AttachChild( m_Prop_line );
	m_Prop_line->SetAutoDelete( true );	
	strconcat( sizeof(buf), buf, base_str, ":", "prop_line" );
	CUIXmlInit::InitStatic( xml_doc, buf, 0, m_Prop_line );

	Fvector2 pos;
	pos.set( 0.0f, m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y );

	for (u32 i = 0; i < max_count; ++i)
	{
		if (i==ALife::eHitTypeFireWound)
		{
			continue;
		}
		m_items[i] = xr_new<CUIOutfitImmunity>();
		m_items[i]->InitFromXml( xml_doc, base_str, i );
		AttachChild( m_items[i] );
		m_items[i]->SetWndPos( pos );
		pos.y += m_items[i]->GetWndSize().y;
	}
	pos.x = GetWndSize().x;
	SetWndSize( pos );
	
	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Расширение алгоритма отображения свойств у костюмов и шлемов
	// Считываем параметры визуальных элементов из конфига xml
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_head_armor_static",				0, &m_staticHeadArmor);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_head_armor_name",				0, &m_textHeadArmorName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_head_armor_value",				0, &m_textHeadArmorValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_body_armor_static",				0, &m_staticBodyArmor);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_body_armor_name",				0, &m_textBodyArmorName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_body_armor_value",				0, &m_textBodyArmorValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_power_loss_static",				0, &m_staticPowerLoss);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_power_loss_name",				0, &m_textPowerLossName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_power_loss_value",				0, &m_textPowerLossValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_sprint_allowed_static",			0, &m_staticSprintAllowed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_sprint_allowed_value",			0, &m_textSprintAllowedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_additional_weight_static",		0, &m_staticAdditionalWeight);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_additional_weight_name",			0, &m_textAdditionalWeightName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_additional_weight_value",		0, &m_textAdditionalWeightValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_additional_walk_speed_static",	0, &m_staticAdditionalWalkSpeed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_additional_walk_speed_name",		0, &m_textAdditionalWalkSpeedName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_additional_walk_speed_value",	0, &m_textAdditionalWalkSpeedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_additional_jump_speed_static",	0, &m_staticAdditionalJumpSpeed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_additional_jump_speed_name",		0, &m_textAdditionalJumpSpeedName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_additional_jump_speed_value",	0, &m_textAdditionalJumpSpeedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_power_restore_speed_static",		0, &m_staticPowerRestoreSpeed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_power_restore_speed_name",		0, &m_textPowerRestoreSpeedName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_power_restore_speed_value",		0, &m_textPowerRestoreSpeedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_radiation_restore_speed_static",	0, &m_staticRadiationRestoreSpeed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_radiation_restore_speed_name",	0, &m_textRadiationRestoreSpeedName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_radiation_restore_speed_value",	0, &m_textRadiationRestoreSpeedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_bleeding_restore_speed_static",	0, &m_staticBleedingRestoreSpeed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_bleeding_restore_speed_name",	0, &m_textBleedingRestoreSpeedName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_bleeding_restore_speed_value",	0, &m_textBleedingRestoreSpeedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_health_restore_speed_static",	0, &m_staticHealthRestoreSpeed);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_health_restore_speed_name",		0, &m_textHealthRestoreSpeedName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_health_restore_speed_value",		0, &m_textHealthRestoreSpeedValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_max_health_add_static",			0, &m_staticMaxHealthAdd);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_max_health_add_name",			0, &m_textMaxHealthAddName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_max_health_add_value",			0, &m_textMaxHealthAddValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_artefact_count_static",			0, &m_staticArtefactCount);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_artefact_count_name",			0, &m_textArtefactCountName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_artefact_count_value",			0, &m_textArtefactCountValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_nightvision_static",				0, &m_staticNightvision);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_nightvision_name",				0, &m_textNightvisionName);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_nightvision_value",				0, &m_textNightvisionValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_enemies_scanner_static",			0, &m_staticEnemiesScanner);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_enemies_scanner_value",			0, &m_textEnemiesScannerValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_dozimeter_static",				0, &m_staticDozimeter);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_dozimeter_value",				0, &m_textDozimeterValue);
	CUIXmlInit::InitStatic		(xml_doc, "outfit_info:cap_psy_controller_static",			0, &m_staticPsyController);
	CUIXmlInit::InitTextWnd		(xml_doc, "outfit_info:cap_psy_controller_value",			0, &m_textPsyControllerValue);
	
	// Запоминаем положение каждого свойства. Сами свойства могут перемещаться в описании, однако позиции должны оставаться фиксированными.
	PropHeight[1]				= m_staticHeadArmor.GetWndPos().y;
	PropHeight[2]				= m_staticBodyArmor.GetWndPos().y;
	PropHeight[3]				= m_staticPowerLoss.GetWndPos().y;
	PropHeight[4]				= m_staticSprintAllowed.GetWndPos().y;
	PropHeight[5]				= m_staticAdditionalWeight.GetWndPos().y;
	PropHeight[6]				= m_staticAdditionalWalkSpeed.GetWndPos().y;
	PropHeight[7]				= m_staticAdditionalJumpSpeed.GetWndPos().y;
	PropHeight[8]				= m_staticPowerRestoreSpeed.GetWndPos().y;
	PropHeight[9]				= m_staticRadiationRestoreSpeed.GetWndPos().y;
	PropHeight[10]				= m_staticBleedingRestoreSpeed.GetWndPos().y;
	PropHeight[11]				= m_staticHealthRestoreSpeed.GetWndPos().y;
	PropHeight[12]				= m_staticMaxHealthAdd.GetWndPos().y;
	PropHeight[13]				= m_staticArtefactCount.GetWndPos().y;
	PropHeight[14]				= m_staticNightvision.GetWndPos().y;
	PropHeight[15]				= m_staticEnemiesScanner.GetWndPos().y;
	PropHeight[16]				= m_staticDozimeter.GetWndPos().y;
	PropHeight[17]				= m_staticPsyController.GetWndPos().y;
	
	// Читаем количество знаков после запятой для отображения дополнительного переносимого веса
	AdditionalWeightAccuracy	= xml_doc.ReadAttribInt("outfit_info:cap_additional_weight_value", 0, "accuracy", 2);
	
	// Читаем калибровочные множители
	PropMagnitude[1]			= xml_doc.ReadAttribFlt("outfit_info:cap_head_armor_value", 0, "magnitude", 100.0f);
	PropMagnitude[2]			= xml_doc.ReadAttribFlt("outfit_info:cap_body_armor_value", 0, "magnitude", 100.0f);
	PropMagnitude[3]			= xml_doc.ReadAttribFlt("outfit_info:cap_power_loss_value", 0, "magnitude", 100.0f);
	// Некоторые параметры не имеют значения, поэтому калибровать нечего (то же самое и для постфиксов)
	PropMagnitude[4]			= 0;	//xml_doc.ReadAttribFlt("outfit_info:cap_sprint_allowed_value", 0, "magnitude", 0.0f);
	PropMagnitude[5]			= xml_doc.ReadAttribFlt("outfit_info:cap_additional_weight_value", 0, "magnitude", 1.0f);
	PropMagnitude[6]			= xml_doc.ReadAttribFlt("outfit_info:cap_additional_walk_speed_value", 0, "magnitude", 10.0f);
	PropMagnitude[7]			= xml_doc.ReadAttribFlt("outfit_info:cap_additional_jump_speed_value", 0, "magnitude", 25.0f);
	PropMagnitude[8]			= xml_doc.ReadAttribFlt("outfit_info:cap_power_restore_speed_value", 0, "magnitude", 1000.0f);
	PropMagnitude[9]			= xml_doc.ReadAttribFlt("outfit_info:cap_radiation_restore_speed_value", 0, "magnitude", 10000.0f);
	PropMagnitude[10]			= xml_doc.ReadAttribFlt("outfit_info:cap_bleeding_restore_speed_value", 0, "magnitude", 10000.0f);
	PropMagnitude[11]			= xml_doc.ReadAttribFlt("outfit_info:cap_health_restore_speed_value", 0, "magnitude", 10000.0f);
	PropMagnitude[12]			= xml_doc.ReadAttribFlt("outfit_info:cap_max_health_add_value", 0, "magnitude", 100.0f);
	PropMagnitude[13]			= xml_doc.ReadAttribFlt("outfit_info:cap_artefact_count_value", 0, "magnitude", 1.0f);
	PropMagnitude[14]			= 0;	//xml_doc.ReadAttribFlt("outfit_info:cap_nightvision_value", 0, "magnitude", 0.0f);
	PropMagnitude[15]			= 0;	//xml_doc.ReadAttribFlt("outfit_info:cap_enemies_scanner_value", 0, "magnitude", 0.0f);
	PropMagnitude[16]			= 0;	//xml_doc.ReadAttribFlt("outfit_info:cap_dozimeter_value", 0, "magnitude", 0.0f);
	PropMagnitude[17]			= 0;	//xml_doc.ReadAttribFlt("outfit_info:cap_psy_controller_value", 0, "magnitude", 0.0f);
	
	// Читаем постфиксы и сразу же переводим их в человеческий вид
	PropPostfix[1]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_head_armor_value", 0, "unit_str", "")).c_str();
	PropPostfix[2]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_body_armor_value", 0, "unit_str", "")).c_str();
	PropPostfix[3]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_power_loss_value", 0, "unit_str", "")).c_str();
	PropPostfix[4]				= "";	//CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_sprint_allowed_value", 0, "unit_str", "")).c_str();
	PropPostfix[5]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_additional_weight_value", 0, "unit_str", "")).c_str();
	PropPostfix[6]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_additional_walk_speed_value", 0, "unit_str", "")).c_str();
	PropPostfix[7]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_additional_jump_speed_value", 0, "unit_str", "")).c_str();
	PropPostfix[8]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_power_restore_speed_value", 0, "unit_str", "")).c_str();
	PropPostfix[9]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_radiation_restore_speed_value", 0, "unit_str", "")).c_str();
	PropPostfix[10]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_bleeding_restore_speed_value", 0, "unit_str", "")).c_str();
	PropPostfix[11]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_health_restore_speed_value", 0, "unit_str", "")).c_str();
	PropPostfix[12]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_max_health_add_value", 0, "unit_str", "")).c_str();
	PropPostfix[13]				= CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_artefact_count_value", 0, "unit_str", "")).c_str();
	PropPostfix[14]				= "";	//CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_nightvision_value", 0, "unit_str", "")).c_str();
	PropPostfix[15]				= "";	//CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_enemies_scanner_value", 0, "unit_str", "")).c_str();
	PropPostfix[16]				= "";	//CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_dozimeter_value", 0, "unit_str", "")).c_str();
	PropPostfix[17]				= "";	//CStringTable().translate(xml_doc.ReadAttrib("outfit_info:cap_psy_controller_value", 0, "unit_str", "")).c_str();
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)
}

void CUIOutfitInfo::UpdateInfo( CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit )
{
	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor || !cur_outfit )
	{
		return;
	}

	for (u32 i = 0; i < max_count; ++i)
	{
		if (i == ALife::eHitTypeFireWound)
		{
			continue;
		}
		
		ALife::EHitType hit_type = (ALife::EHitType)i;
		float max_power = actor->conditions().GetZoneMaxPower( hit_type );

		float cur = cur_outfit->GetDefHitTypeProtection( hit_type );
		cur /= max_power; // = 0..1
		float slot = cur;
		
		if ( slot_outfit )
		{
			slot = slot_outfit->GetDefHitTypeProtection( hit_type );
			slot /= max_power; //  = 0..1
		}
		m_items[i]->SetProgressValue( cur, slot );
	}

	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Расширение алгоритма отображения свойств у костюмов
	// Старый алгорит отображения брони больше не актуален
	/*
	if ( m_items[ALife::eHitTypeFireWound] )
	{
		IKinematics* ikv = smart_cast<IKinematics*>( actor->Visual() );
		VERIFY( ikv );
		u16 spine_bone = ikv->LL_BoneID( "bip01_spine" );

		float cur = cur_outfit->GetBoneArmor( spine_bone )*cur_outfit->GetCondition();
		//if(!cur_outfit->bIsHelmetAvaliable)
		//{
		//	spine_bone = ikv->LL_BoneID("bip01_head");
		//	cur += cur_outfit->GetBoneArmor(spine_bone);
		//}
		float slot = cur;
		if(slot_outfit)
		{
			spine_bone = ikv->LL_BoneID( "bip01_spine" );
			slot = slot_outfit->GetBoneArmor( spine_bone )*slot_outfit->GetCondition(); 
			//if(!slot_outfit->bIsHelmetAvaliable)
			//{
			//	spine_bone = ikv->LL_BoneID("bip01_head");
			//	slot += slot_outfit->GetBoneArmor(spine_bone);
			//}
		}
		float max_power = actor->conditions().GetMaxFireWoundProtection();
		cur /= max_power;
		slot /= max_power;
		m_items[ALife::eHitTypeFireWound]->SetProgressValue( cur, slot );
	}
	*/
	
	// Инициализируем счётчик отображаемых свойств
	int PropCount = 0;
	// Инициализируем добавочную высоту окна
	int NewAdditionalHeight = 0;
	// Подготовка переменных для получения данных по костям
	IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
	VERIFY(ikv);
	u16 control_bone;
	// Подготовка вспомогательных данных
	string128 str;
	char* str_prefix;
	Fvector2 prop_pos;
	// Будем сравнивать параметры, связанные с ношением шлемов, только если у обоих костюмов параметр доступности шлема одинаков
	bool can_compare_helmets = false;
	if (slot_outfit)
		can_compare_helmets = (cur_outfit->bIsHelmetAvaliable == slot_outfit->bIsHelmetAvaliable);
	
	//--------------------------------------------------
	//		Защита головы (опциональный параметр)
	//--------------------------------------------------
	// Если у данного костюма есть встроенный шлема, то будем показывать защиту головы
	if(!cur_outfit->bIsHelmetAvaliable)
	{
		// Получаем айди кости головы
		control_bone = ikv->LL_BoneID("bip01_head");
		// Получаем защиту по айди кости головы с учётом состояния костюма
		// 7.12.2018	(begin)
		// Изменение формулы учёта состояния у шлемов и костюмов
		// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
		float HeadArmor = cur_outfit->GetBoneArmor(control_bone) * (cur_outfit->GetCondition() * 0.9 + 0.1);
		// 7.12.2018	(end)
		// Округляем до сотых
		HeadArmor = floor(HeadArmor*100+0.5)/100;
		// Если есть возможность сравнить с шлемом костюма в слоте, то будем сравнивать
		float SlotHeadArmor = HeadArmor;
		if (can_compare_helmets)
		{
			// 7.12.2018	(begin)
			// Изменение формулы учёта состояния у шлемов и костюмов
			// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
			SlotHeadArmor = slot_outfit->GetBoneArmor(control_bone) * (slot_outfit->GetCondition() * 0.9 + 0.1);
			// 7.12.2018	(end)
			SlotHeadArmor = floor(SlotHeadArmor*100+0.5)/100;
		}
		// Сравниваем значение брони шлема выбранного костюма и того, что в слоте
		if (HeadArmor==SlotHeadArmor)
			m_textHeadArmorValue.SetTextColor(color_rgba(170,170,170,255));
		else if (HeadArmor<SlotHeadArmor)
			m_textHeadArmorValue.SetTextColor(color_rgba(255,0,0,255));
		else
			m_textHeadArmorValue.SetTextColor(color_rgba(0,255,0,255));
		str_prefix = "";
		// Будем выводить число с без дробной части
		xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, HeadArmor*PropMagnitude[1], PropPostfix[1]);
		// Присваиваем текстовой переменной полученное значение
		m_textHeadArmorValue.SetText(str);
		// Увеличиваем счётчик отображаемых свойств
		PropCount++;
		// Устанавливаем позицию высоты на ближайшую доступную
		prop_pos = m_staticHeadArmor.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticHeadArmor.SetWndPos(prop_pos);
		prop_pos = m_textHeadArmorName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textHeadArmorName.SetWndPos(prop_pos);
		prop_pos = m_textHeadArmorValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textHeadArmorValue.SetWndPos(prop_pos);
		// Включаем показ параметров
		m_staticHeadArmor.Show(true);
		m_textHeadArmorName.Show(true);
		m_textHeadArmorValue.Show(true);
		// Учитываем высоту статика (отталкиваемся именно от него), чтобы растянуть главное окно
		NewAdditionalHeight += m_staticHeadArmor.GetWndSize().y;
	}
	// Иначе нужно скрыть лишние параметры
	else
	{
		m_staticHeadArmor.Show(false);
		m_textHeadArmorName.Show(false);
		m_textHeadArmorValue.Show(false);
	}
	
	//--------------------------------------------------
	//		Защита туловища (обязательный параметр)
	//--------------------------------------------------
	control_bone = ikv->LL_BoneID("bip01_spine");
	// 7.12.2018	(begin)
	// Изменение формулы учёта состояния у шлемов и костюмов
	// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
	float BodyArmor = cur_outfit->GetBoneArmor(control_bone) * (cur_outfit->GetCondition() * 0.9 + 0.1);
	// 7.12.2018	(end)
	BodyArmor = floor(BodyArmor*100+0.5)/100;
	float SlotBodyArmor = BodyArmor;
	if (slot_outfit)
	{
		// 7.12.2018	(begin)
		// Изменение формулы учёта состояния у шлемов и костюмов
		// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
		SlotBodyArmor = slot_outfit->GetBoneArmor(control_bone) * (slot_outfit->GetCondition() * 0.9 + 0.1);
		// 7.12.2018	(end)
		SlotBodyArmor = floor(SlotBodyArmor*100+0.5)/100;
	}
	if (BodyArmor==SlotBodyArmor)
		m_textBodyArmorValue.SetTextColor(color_rgba(170,170,170,255));
	else if (BodyArmor<SlotBodyArmor)
		m_textBodyArmorValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textBodyArmorValue.SetTextColor(color_rgba(0,255,0,255));
	str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, BodyArmor*PropMagnitude[2], PropPostfix[2]);
	m_textBodyArmorValue.SetText(str);
	PropCount++;
	prop_pos = m_staticBodyArmor.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_staticBodyArmor.SetWndPos(prop_pos);
	prop_pos = m_textBodyArmorName.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textBodyArmorName.SetWndPos(prop_pos);
	prop_pos = m_textBodyArmorValue.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textBodyArmorValue.SetWndPos(prop_pos);
	m_staticBodyArmor.Show(true);
	m_textBodyArmorName.Show(true);
	m_textBodyArmorValue.Show(true);
	NewAdditionalHeight += m_staticBodyArmor.GetWndSize().y;
	
	//--------------------------------------------------
	//		Потеря сил (обязательный параметр)
	//--------------------------------------------------
	float PowerLoss = cur_outfit->m_fPowerLoss - 1;
	PowerLoss = floor(PowerLoss*100+0.5)/100;
	float SlotPowerLoss = PowerLoss;
	if (can_compare_helmets)
	{
		SlotPowerLoss = slot_outfit->m_fPowerLoss - 1;
		SlotPowerLoss = floor(SlotPowerLoss*100+0.5)/100;
	}
	if (PowerLoss==SlotPowerLoss)
		m_textPowerLossValue.SetTextColor(color_rgba(170,170,170,255));
	else if (PowerLoss<SlotPowerLoss)
		m_textPowerLossValue.SetTextColor(color_rgba(0,255,0,255));
	else
		m_textPowerLossValue.SetTextColor(color_rgba(255,0,0,255));
	if (PowerLoss>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, PowerLoss*PropMagnitude[3], PropPostfix[3]);
	m_textPowerLossValue.SetText(str);
	PropCount++;
	prop_pos = m_staticPowerLoss.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_staticPowerLoss.SetWndPos(prop_pos);
	prop_pos = m_textPowerLossName.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textPowerLossName.SetWndPos(prop_pos);
	prop_pos = m_textPowerLossValue.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textPowerLossValue.SetWndPos(prop_pos);
	NewAdditionalHeight += m_staticPowerLoss.GetWndSize().y;
	
	//----------------------------------------------------------------------
	//		Невозможность передвигаться бегом (опциональный параметр)
	//----------------------------------------------------------------------
	bool SprintAllowed = cur_outfit->IsSprintAllowed();
	bool SlotSprintAllowed = SprintAllowed;
	if (slot_outfit)
		SlotSprintAllowed = slot_outfit->IsSprintAllowed();
	if (SprintAllowed==SlotSprintAllowed)
		m_textSprintAllowedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (SprintAllowed && !SlotSprintAllowed)
		m_textSprintAllowedValue.SetTextColor(color_rgba(0,255,0,255));
	else
		m_textSprintAllowedValue.SetTextColor(color_rgba(255,0,0,255));
	if (SprintAllowed)
		str_prefix = "ui_sprint_allowed";
	else
		str_prefix = "ui_sprint_impossible";
	xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	m_textSprintAllowedValue.SetText(str);
	if (!SprintAllowed || (SprintAllowed && !SlotSprintAllowed))
	{
		PropCount++;
		prop_pos = m_staticSprintAllowed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticSprintAllowed.SetWndPos(prop_pos);
		prop_pos = m_textSprintAllowedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textSprintAllowedValue.SetWndPos(prop_pos);
		m_staticSprintAllowed.Show(true);
		m_textSprintAllowedValue.Show(true);
		NewAdditionalHeight += m_staticSprintAllowed.GetWndSize().y;
	}
	else
	{
		m_staticSprintAllowed.Show(false);
		m_textSprintAllowedValue.Show(false);
	}
	
	//------------------------------------------------------------------
	//		Дополнительный переносимый вес (опциональный параметр)
	//------------------------------------------------------------------
	float AdditionalWeight = cur_outfit->m_additional_weight;
	float SlotAdditionalWeight = AdditionalWeight;
	if (slot_outfit)
		SlotAdditionalWeight = slot_outfit->m_additional_weight;
	if (AdditionalWeight==SlotAdditionalWeight)
		m_textAdditionalWeightValue.SetTextColor(color_rgba(170,170,170,255));
	else if (AdditionalWeight<SlotAdditionalWeight)
		m_textAdditionalWeightValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textAdditionalWeightValue.SetTextColor(color_rgba(0,255,0,255));
	if (AdditionalWeight>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	if (AdditionalWeightAccuracy==0)
		xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, AdditionalWeight*PropMagnitude[5], PropPostfix[5]);
	else if (AdditionalWeightAccuracy==1)
		xr_sprintf(str, sizeof(str), "%s%.1f %s", str_prefix, AdditionalWeight*PropMagnitude[5], PropPostfix[5]);
	else if (AdditionalWeightAccuracy==2)
		xr_sprintf(str, sizeof(str), "%s%.2f %s", str_prefix, AdditionalWeight*PropMagnitude[5], PropPostfix[5]);
	else
		xr_sprintf(str, sizeof(str), "%s%.3f %s", str_prefix, AdditionalWeight*PropMagnitude[5], PropPostfix[5]);
	m_textAdditionalWeightValue.SetText(str);
	if (AdditionalWeight!=0 || AdditionalWeight!=SlotAdditionalWeight)
	{
		PropCount++;
		prop_pos = m_staticAdditionalWeight.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticAdditionalWeight.SetWndPos(prop_pos);
		prop_pos = m_textAdditionalWeightName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textAdditionalWeightName.SetWndPos(prop_pos);
		prop_pos = m_textAdditionalWeightValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textAdditionalWeightValue.SetWndPos(prop_pos);
		m_staticAdditionalWeight.Show(true);
		m_textAdditionalWeightName.Show(true);
		m_textAdditionalWeightValue.Show(true);
		NewAdditionalHeight += m_staticAdditionalWeight.GetWndSize().y;
	}
	else
	{
		m_staticAdditionalWeight.Show(false);
		m_textAdditionalWeightName.Show(false);
		m_textAdditionalWeightValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Дополнительная скорость передвижения (опциональный параметр)
	//----------------------------------------------------------------------
	float AdditionalWalkSpeed = cur_outfit->m_fWalkAccelAdd;
	float SlotAdditionalWalkSpeed = AdditionalWalkSpeed;
	if (slot_outfit)
		SlotAdditionalWalkSpeed = slot_outfit->m_fWalkAccelAdd;
	if (AdditionalWalkSpeed==SlotAdditionalWalkSpeed)
		m_textAdditionalWalkSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (AdditionalWalkSpeed<SlotAdditionalWalkSpeed)
		m_textAdditionalWalkSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textAdditionalWalkSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (AdditionalWalkSpeed>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, AdditionalWalkSpeed*PropMagnitude[6], PropPostfix[6]);
	m_textAdditionalWalkSpeedValue.SetText(str);
	if (AdditionalWalkSpeed!=0 || AdditionalWalkSpeed!=SlotAdditionalWalkSpeed)
	{
		PropCount++;
		prop_pos = m_staticAdditionalWalkSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticAdditionalWalkSpeed.SetWndPos(prop_pos);
		prop_pos = m_textAdditionalWalkSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textAdditionalWalkSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textAdditionalWalkSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textAdditionalWalkSpeedValue.SetWndPos(prop_pos);
		m_staticAdditionalWalkSpeed.Show(true);
		m_textAdditionalWalkSpeedName.Show(true);
		m_textAdditionalWalkSpeedValue.Show(true);
		NewAdditionalHeight += m_staticAdditionalWalkSpeed.GetWndSize().y;
	}
	else
	{
		m_staticAdditionalWalkSpeed.Show(false);
		m_textAdditionalWalkSpeedName.Show(false);
		m_textAdditionalWalkSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Дополнительная скорость прыжка (опциональный параметр)
	//----------------------------------------------------------------------
	float AdditionalJumpSpeed = cur_outfit->m_fJumpSpeedAdd;
	float SlotAdditionalJumpSpeed = AdditionalJumpSpeed;
	if (slot_outfit)
		SlotAdditionalJumpSpeed = slot_outfit->m_fJumpSpeedAdd;
	if (AdditionalJumpSpeed==SlotAdditionalJumpSpeed)
		m_textAdditionalJumpSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (AdditionalJumpSpeed<SlotAdditionalJumpSpeed)
		m_textAdditionalJumpSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textAdditionalJumpSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (AdditionalJumpSpeed>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, AdditionalJumpSpeed*PropMagnitude[7], PropPostfix[7]);
	m_textAdditionalJumpSpeedValue.SetText(str);
	if (AdditionalJumpSpeed!=0 || AdditionalJumpSpeed!=SlotAdditionalJumpSpeed)
	{
		PropCount++;
		prop_pos = m_staticAdditionalJumpSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticAdditionalJumpSpeed.SetWndPos(prop_pos);
		prop_pos = m_textAdditionalJumpSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textAdditionalJumpSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textAdditionalJumpSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textAdditionalJumpSpeedValue.SetWndPos(prop_pos);
		m_staticAdditionalJumpSpeed.Show(true);
		m_textAdditionalJumpSpeedName.Show(true);
		m_textAdditionalJumpSpeedValue.Show(true);
		NewAdditionalHeight += m_staticAdditionalJumpSpeed.GetWndSize().y;
	}
	else
	{
		m_staticAdditionalJumpSpeed.Show(false);
		m_textAdditionalJumpSpeedName.Show(false);
		m_textAdditionalJumpSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Восстановление сил (опциональный параметр)
	//----------------------------------------------------------------------
	float PowerRestoreSpeed = cur_outfit->m_fPowerRestoreSpeed;
	float SlotPowerRestoreSpeed = PowerRestoreSpeed;
	if (slot_outfit)
		SlotPowerRestoreSpeed = slot_outfit->m_fPowerRestoreSpeed;
	if (PowerRestoreSpeed==SlotPowerRestoreSpeed)
		m_textPowerRestoreSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (PowerRestoreSpeed<SlotPowerRestoreSpeed)
		m_textPowerRestoreSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textPowerRestoreSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (PowerRestoreSpeed>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, PowerRestoreSpeed*PropMagnitude[8], PropPostfix[8]);
	m_textPowerRestoreSpeedValue.SetText(str);
	if (PowerRestoreSpeed!=0 || PowerRestoreSpeed!=SlotPowerRestoreSpeed)
	{
		PropCount++;
		prop_pos = m_staticPowerRestoreSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticPowerRestoreSpeed.SetWndPos(prop_pos);
		prop_pos = m_textPowerRestoreSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textPowerRestoreSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textPowerRestoreSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textPowerRestoreSpeedValue.SetWndPos(prop_pos);
		m_staticPowerRestoreSpeed.Show(true);
		m_textPowerRestoreSpeedName.Show(true);
		m_textPowerRestoreSpeedValue.Show(true);
		NewAdditionalHeight += m_staticPowerRestoreSpeed.GetWndSize().y;
	}
	else
	{
		m_staticPowerRestoreSpeed.Show(false);
		m_textPowerRestoreSpeedName.Show(false);
		m_textPowerRestoreSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Вывод радиации (опциональный параметр)
	//----------------------------------------------------------------------
	float RadiationRestoreSpeed = cur_outfit->m_fRadiationRestoreSpeed;
	float SlotRadiationRestoreSpeed = RadiationRestoreSpeed;
	if (slot_outfit)
		SlotRadiationRestoreSpeed = slot_outfit->m_fRadiationRestoreSpeed;
	if (RadiationRestoreSpeed==SlotRadiationRestoreSpeed)
		m_textRadiationRestoreSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (RadiationRestoreSpeed>SlotRadiationRestoreSpeed)
		m_textRadiationRestoreSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textRadiationRestoreSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (RadiationRestoreSpeed>0)
		str_prefix = "+";
	else if (RadiationRestoreSpeed==0)
		str_prefix = "-";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, RadiationRestoreSpeed*PropMagnitude[9], PropPostfix[9]);
	m_textRadiationRestoreSpeedValue.SetText(str);
	if (RadiationRestoreSpeed!=0 || RadiationRestoreSpeed!=SlotRadiationRestoreSpeed)
	{
		PropCount++;
		prop_pos = m_staticRadiationRestoreSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticRadiationRestoreSpeed.SetWndPos(prop_pos);
		prop_pos = m_textRadiationRestoreSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textRadiationRestoreSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textRadiationRestoreSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textRadiationRestoreSpeedValue.SetWndPos(prop_pos);
		m_staticRadiationRestoreSpeed.Show(true);
		m_textRadiationRestoreSpeedName.Show(true);
		m_textRadiationRestoreSpeedValue.Show(true);
		NewAdditionalHeight += m_staticRadiationRestoreSpeed.GetWndSize().y;
	}
	else
	{
		m_staticRadiationRestoreSpeed.Show(false);
		m_textRadiationRestoreSpeedName.Show(false);
		m_textRadiationRestoreSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Заживление ран (опциональный параметр)
	//----------------------------------------------------------------------
	float BleedingRestoreSpeed = cur_outfit->m_fBleedingRestoreSpeed;
	float SlotBleedingRestoreSpeed = BleedingRestoreSpeed;
	if (slot_outfit)
		SlotBleedingRestoreSpeed = slot_outfit->m_fBleedingRestoreSpeed;
	if (BleedingRestoreSpeed==SlotBleedingRestoreSpeed)
		m_textBleedingRestoreSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (BleedingRestoreSpeed<SlotBleedingRestoreSpeed)
		m_textBleedingRestoreSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textBleedingRestoreSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (BleedingRestoreSpeed>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, BleedingRestoreSpeed*PropMagnitude[10], PropPostfix[10]);
	m_textBleedingRestoreSpeedValue.SetText(str);
	if (BleedingRestoreSpeed!=0 || BleedingRestoreSpeed!=SlotBleedingRestoreSpeed)
	{
		PropCount++;
		prop_pos = m_staticBleedingRestoreSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticBleedingRestoreSpeed.SetWndPos(prop_pos);
		prop_pos = m_textBleedingRestoreSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textBleedingRestoreSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textBleedingRestoreSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textBleedingRestoreSpeedValue.SetWndPos(prop_pos);
		m_staticBleedingRestoreSpeed.Show(true);
		m_textBleedingRestoreSpeedName.Show(true);
		m_textBleedingRestoreSpeedValue.Show(true);
		NewAdditionalHeight += m_staticBleedingRestoreSpeed.GetWndSize().y;
	}
	else
	{
		m_staticBleedingRestoreSpeed.Show(false);
		m_textBleedingRestoreSpeedName.Show(false);
		m_textBleedingRestoreSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Восстановление здоровья (опциональный параметр)
	//----------------------------------------------------------------------
	float HealthRestoreSpeed = cur_outfit->m_fHealthRestoreSpeed;
	float SlotHealthRestoreSpeed = HealthRestoreSpeed;
	if (slot_outfit)
		SlotHealthRestoreSpeed = slot_outfit->m_fHealthRestoreSpeed;
	if (HealthRestoreSpeed==SlotHealthRestoreSpeed)
		m_textHealthRestoreSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (HealthRestoreSpeed<SlotHealthRestoreSpeed)
		m_textHealthRestoreSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textHealthRestoreSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (HealthRestoreSpeed>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, HealthRestoreSpeed*PropMagnitude[11], PropPostfix[11]);
	m_textHealthRestoreSpeedValue.SetText(str);
	if (HealthRestoreSpeed!=0 || HealthRestoreSpeed!=SlotHealthRestoreSpeed)
	{
		PropCount++;
		prop_pos = m_staticHealthRestoreSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticHealthRestoreSpeed.SetWndPos(prop_pos);
		prop_pos = m_textHealthRestoreSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textHealthRestoreSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textHealthRestoreSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textHealthRestoreSpeedValue.SetWndPos(prop_pos);
		m_staticHealthRestoreSpeed.Show(true);
		m_textHealthRestoreSpeedName.Show(true);
		m_textHealthRestoreSpeedValue.Show(true);
		NewAdditionalHeight += m_staticHealthRestoreSpeed.GetWndSize().y;
	}
	else
	{
		m_staticHealthRestoreSpeed.Show(false);
		m_textHealthRestoreSpeedName.Show(false);
		m_textHealthRestoreSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Дополнительное здоровье (опциональный параметр)
	//----------------------------------------------------------------------
	float MaxHealthAdd = cur_outfit->m_fMaxHealthAdd;
	float SlotMaxHealthAdd = MaxHealthAdd;
	if (slot_outfit)
		SlotMaxHealthAdd = slot_outfit->m_fMaxHealthAdd;
	if (MaxHealthAdd==SlotMaxHealthAdd)
		m_textMaxHealthAddValue.SetTextColor(color_rgba(170,170,170,255));
	else if (MaxHealthAdd<SlotMaxHealthAdd)
		m_textMaxHealthAddValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textMaxHealthAddValue.SetTextColor(color_rgba(0,255,0,255));
	if (MaxHealthAdd>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, MaxHealthAdd*PropMagnitude[12], PropPostfix[12]);
	m_textMaxHealthAddValue.SetText(str);
	if (MaxHealthAdd!=0 || MaxHealthAdd!=SlotMaxHealthAdd)
	{
		PropCount++;
		prop_pos = m_staticMaxHealthAdd.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticMaxHealthAdd.SetWndPos(prop_pos);
		prop_pos = m_textMaxHealthAddName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textMaxHealthAddName.SetWndPos(prop_pos);
		prop_pos = m_textMaxHealthAddValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textMaxHealthAddValue.SetWndPos(prop_pos);
		m_staticMaxHealthAdd.Show(true);
		m_textMaxHealthAddName.Show(true);
		m_textMaxHealthAddValue.Show(true);
		NewAdditionalHeight += m_staticMaxHealthAdd.GetWndSize().y;
	}
	else
	{
		m_staticMaxHealthAdd.Show(false);
		m_textMaxHealthAddName.Show(false);
		m_textMaxHealthAddValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Контейнеры для артефактов (обязательный параметр)
	//----------------------------------------------------------------------
	int ArtefactCount = cur_outfit->get_artefact_count();
	int SlotArtefactCount = ArtefactCount;
	if (slot_outfit)
		SlotArtefactCount = slot_outfit->get_artefact_count();
	if (ArtefactCount==SlotArtefactCount)
		m_textArtefactCountValue.SetTextColor(color_rgba(170,170,170,255));
	else if (ArtefactCount<SlotArtefactCount)
		m_textArtefactCountValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textArtefactCountValue.SetTextColor(color_rgba(0,255,0,255));
	str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%d %s", str_prefix, ArtefactCount, PropPostfix[13]);
	m_textArtefactCountValue.SetText(str);
	PropCount++;
	prop_pos = m_staticArtefactCount.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_staticArtefactCount.SetWndPos(prop_pos);
	prop_pos = m_textArtefactCountName.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textArtefactCountName.SetWndPos(prop_pos);
	prop_pos = m_textArtefactCountValue.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textArtefactCountValue.SetWndPos(prop_pos);
	m_staticArtefactCount.Show(true);
	m_textArtefactCountName.Show(true);
	m_textArtefactCountValue.Show(true);
	NewAdditionalHeight += m_staticArtefactCount.GetWndSize().y;
	
	//----------------------------------------------------------------------
	//		Прибор ночного видения (опциональный параметр)
	//----------------------------------------------------------------------
	shared_str NightvisionSect = cur_outfit->m_NightVisionSect;
	shared_str SlotNightvisionSect = NightvisionSect;
	if (can_compare_helmets)
		SlotNightvisionSect = slot_outfit->m_NightVisionSect;
	int NightvisionStatus = READ_IF_EXISTS(pSettings, r_u32, NightvisionSect, "nightvision_level", 0);
	int SlotNightvisionStatus = READ_IF_EXISTS(pSettings, r_u32, SlotNightvisionSect, "nightvision_level", 0);
	if (NightvisionStatus==SlotNightvisionStatus)
		m_textNightvisionValue.SetTextColor(color_rgba(170,170,170,255));
	else if (NightvisionStatus<SlotNightvisionStatus)
		m_textNightvisionValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textNightvisionValue.SetTextColor(color_rgba(0,255,0,255));
	if (NightvisionStatus==0)
	{
		str_prefix = "ui_nightvision_apsent";
		xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	}
	else
	{
		str_prefix = "ui_nightvision_level";
		xr_sprintf(str, sizeof(str), "%d %s", NightvisionStatus, CStringTable().translate(str_prefix).c_str());
	}
	m_textNightvisionValue.SetText(str);
	if (NightvisionStatus!=0 || NightvisionStatus!=SlotNightvisionStatus)
	{
		PropCount++;
		prop_pos = m_staticNightvision.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticNightvision.SetWndPos(prop_pos);
		prop_pos = m_textNightvisionName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textNightvisionName.SetWndPos(prop_pos);
		prop_pos = m_textNightvisionValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textNightvisionValue.SetWndPos(prop_pos);
		m_staticNightvision.Show(true);
		m_textNightvisionName.Show(true);
		m_textNightvisionValue.Show(true);
		NewAdditionalHeight += m_staticNightvision.GetWndSize().y;
	}
	else
	{
		m_staticNightvision.Show(false);
		m_textNightvisionName.Show(false);
		m_textNightvisionValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Инфракрасный сканнер (опциональный параметр)
	//----------------------------------------------------------------------
	int EnemiesScannerDist = cur_outfit->m_fShowNearestEnemiesDistance;
	int SlotEnemiesScannerDist = EnemiesScannerDist;
	bool EnemiesScanner = (EnemiesScannerDist>=25);
	if (can_compare_helmets)
		SlotEnemiesScannerDist = slot_outfit->m_fShowNearestEnemiesDistance;
	bool SlotEnemiesScanner = (SlotEnemiesScannerDist>=25);
	if (EnemiesScanner==SlotEnemiesScanner)
		m_textEnemiesScannerValue.SetTextColor(color_rgba(170,170,170,255));
	else if (!EnemiesScanner && SlotEnemiesScanner)
		m_textEnemiesScannerValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textEnemiesScannerValue.SetTextColor(color_rgba(0,255,0,255));
	if (EnemiesScanner)
		str_prefix = "ui_enemies_scanner";
	else
		str_prefix = "ui_enemies_scanner_apsent";
	xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	m_textEnemiesScannerValue.SetText(str);
	if (EnemiesScanner || SlotEnemiesScanner)
	{
		PropCount++;
		prop_pos = m_staticEnemiesScanner.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticEnemiesScanner.SetWndPos(prop_pos);
		prop_pos = m_textEnemiesScannerValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textEnemiesScannerValue.SetWndPos(prop_pos);
		m_staticEnemiesScanner.Show(true);
		m_textEnemiesScannerValue.Show(true);
		NewAdditionalHeight += m_staticEnemiesScanner.GetWndSize().y;
	}
	else
	{
		m_staticEnemiesScanner.Show(false);
		m_textEnemiesScannerValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Дозиметр (опциональный параметр)
	//----------------------------------------------------------------------
	bool Dozimeter = cur_outfit->bIsDozimiterAvaliable;
	bool SlotDozimeter = Dozimeter;
	if (slot_outfit)
		SlotDozimeter = slot_outfit->bIsDozimiterAvaliable;
	if (Dozimeter==SlotDozimeter)
		m_textDozimeterValue.SetTextColor(color_rgba(170,170,170,255));
	else if (!Dozimeter && SlotDozimeter)
		m_textDozimeterValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textDozimeterValue.SetTextColor(color_rgba(0,255,0,255));
	if (Dozimeter)
		str_prefix = "ui_dozimeter";
	else
		str_prefix = "ui_dozimeter_apsent";
	xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	m_textDozimeterValue.SetText(str);
	if (Dozimeter || SlotDozimeter)
	{
		PropCount++;
		prop_pos = m_staticDozimeter.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticDozimeter.SetWndPos(prop_pos);
		prop_pos = m_textDozimeterValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textDozimeterValue.SetWndPos(prop_pos);
		m_staticDozimeter.Show(true);
		m_textDozimeterValue.Show(true);
		NewAdditionalHeight += m_staticDozimeter.GetWndSize().y;
	}
	else
	{
		m_staticDozimeter.Show(false);
		m_textDozimeterValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Эмулятор мозговых волн (опциональный параметр)
	//----------------------------------------------------------------------
	bool PsyController = cur_outfit->bIsPsyControllerAvaliable;
	bool SlotPsyController = PsyController;
	if (slot_outfit)
		SlotPsyController = slot_outfit->bIsPsyControllerAvaliable;
	if (PsyController==SlotPsyController)
		m_textPsyControllerValue.SetTextColor(color_rgba(170,170,170,255));
	else if (!PsyController && SlotPsyController)
		m_textPsyControllerValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textPsyControllerValue.SetTextColor(color_rgba(0,255,0,255));
	if (PsyController)
		str_prefix = "ui_psy_controller";
	else
		str_prefix = "ui_psy_controller_apsent";
	xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	m_textPsyControllerValue.SetText(str);
	if (PsyController || SlotPsyController)
	{
		PropCount++;
		prop_pos = m_staticPsyController.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticPsyController.SetWndPos(prop_pos);
		prop_pos = m_textPsyControllerValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textPsyControllerValue.SetWndPos(prop_pos);
		m_staticPsyController.Show(true);
		m_textPsyControllerValue.Show(true);
		NewAdditionalHeight += m_staticPsyController.GetWndSize().y;
	}
	else
	{
		m_staticPsyController.Show(false);
		m_textPsyControllerValue.Show(false);
	}
	
	// Если высоту окна нужно менять, то
	if (AdditionalHeight!=NewAdditionalHeight)
	{
		// Получаем текущую высоту окна
		Fvector2 MainWndSize = GetWndSize();
		// Откатываем до исходного значения
		MainWndSize.y -= AdditionalHeight;
		// Перерасчитываем высоту с учётом новых параметров
		MainWndSize.y += NewAdditionalHeight;
		// Применяем новые размеры
		SetWndSize(MainWndSize);
		// Запоминаем новую добавочную высоту окна
		AdditionalHeight = NewAdditionalHeight;
	}
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)
}


void CUIOutfitInfo::UpdateInfo( CHelmet* cur_helmet, CHelmet* slot_helmet )
{
	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor || !cur_helmet )
	{
		return;
	}

	for (u32 i = 0; i < max_count; ++i)
	{
		if ( i == ALife::eHitTypeFireWound )
		{
			continue;
		}
		
		ALife::EHitType hit_type = (ALife::EHitType)i;
		float max_power = actor->conditions().GetZoneMaxPower( hit_type );

		float cur = cur_helmet->GetDefHitTypeProtection( hit_type );
		cur /= max_power; // = 0..1
		float slot = cur;
		
		if ( slot_helmet )
		{
			slot = slot_helmet->GetDefHitTypeProtection( hit_type );
			slot /= max_power; //  = 0..1
		}
		m_items[i]->SetProgressValue( cur, slot );
	}

	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Расширение алгоритма отображения свойств у шлемов
	/*
	if ( m_items[ALife::eHitTypeFireWound] )
	{
		IKinematics* ikv = smart_cast<IKinematics*>( actor->Visual() );
		VERIFY( ikv );
		u16 spine_bone = ikv->LL_BoneID( "bip01_head" );

		float cur = cur_helmet->GetBoneArmor( spine_bone )*cur_helmet->GetCondition();
		float slot = (slot_helmet)? slot_helmet->GetBoneArmor( spine_bone )*slot_helmet->GetCondition() : cur;
		// Lex Addon (correct by Suhar_) 14.07.2016		(begin)
		// Добавляем в алгоритм отображения индикатора брони шлема зависимость от максимального значения
		float max_power = actor->conditions().GetMaxFireWoundProtection();
		cur /= max_power;
		slot /= max_power;
		// Lex Addon (correct by Suhar_) 14.07.2016		(end)
		
		m_items[ALife::eHitTypeFireWound]->SetProgressValue( cur, slot );
	}
	*/
	
	int PropCount = 0;
	int NewAdditionalHeight = 0;
	
	IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
	VERIFY(ikv);
	u16 control_bone;
	
	string128 str;
	char* str_prefix;
	Fvector2 prop_pos;
	
	//--------------------------------------------------
	//		Защита головы (обязательный параметр)
	//--------------------------------------------------
	control_bone = ikv->LL_BoneID("bip01_head");
	// 7.12.2018	(begin)
	// Изменение формулы учёта состояния у шлемов и костюмов
	// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
	float HeadArmor = cur_helmet->GetBoneArmor(control_bone) * (cur_helmet->GetCondition() * 0.9 + 0.1);
	// 7.12.2018	(end)
	HeadArmor = floor(HeadArmor*100+0.5)/100;
	float SlotHeadArmor = HeadArmor;
	if (slot_helmet)
	{
		// 7.12.2018	(begin)
		// Изменение формулы учёта состояния у шлемов и костюмов
		// Шлемы и костюмы дают небольшую защиту даже при очень плохом состоянии (10% защиты от максимума)
		SlotHeadArmor = slot_helmet->GetBoneArmor(control_bone) * (slot_helmet->GetCondition() * 0.9 + 0.1);
		// 7.12.2018	(end)
		SlotHeadArmor = floor(SlotHeadArmor*100+0.5)/100;
	}
	if (HeadArmor==SlotHeadArmor)
		m_textHeadArmorValue.SetTextColor(color_rgba(170,170,170,255));
	else if (HeadArmor<SlotHeadArmor)
		m_textHeadArmorValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textHeadArmorValue.SetTextColor(color_rgba(0,255,0,255));
	str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, HeadArmor*PropMagnitude[1], PropPostfix[1]);
	m_textHeadArmorValue.SetText(str);
	PropCount++;
	prop_pos = m_staticHeadArmor.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_staticHeadArmor.SetWndPos(prop_pos);
	prop_pos = m_textHeadArmorName.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textHeadArmorName.SetWndPos(prop_pos);
	prop_pos = m_textHeadArmorValue.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textHeadArmorValue.SetWndPos(prop_pos);
	m_staticHeadArmor.Show(true);
	m_textHeadArmorName.Show(true);
	m_textHeadArmorValue.Show(true);
	NewAdditionalHeight += m_staticHeadArmor.GetWndSize().y;
	
	//--------------------------------------------------
	//		Защита туловища (недоступный параметр)
	//--------------------------------------------------
	m_staticBodyArmor.Show(false);
	m_textBodyArmorName.Show(false);
	m_textBodyArmorValue.Show(false);
	
	//--------------------------------------------------
	//		Потеря сил (обязательный параметр)
	//--------------------------------------------------
	float PowerLoss = cur_helmet->m_fPowerLoss - 1;
	PowerLoss = floor(PowerLoss*100+0.5)/100;
	float SlotPowerLoss = PowerLoss;
	if (slot_helmet)
	{
		SlotPowerLoss = slot_helmet->m_fPowerLoss - 1;
		SlotPowerLoss = floor(SlotPowerLoss*100+0.5)/100;
	}
	if (PowerLoss==SlotPowerLoss)
		m_textPowerLossValue.SetTextColor(color_rgba(170,170,170,255));
	else if (PowerLoss<SlotPowerLoss)
		m_textPowerLossValue.SetTextColor(color_rgba(0,255,0,255));
	else
		m_textPowerLossValue.SetTextColor(color_rgba(255,0,0,255));
	if (PowerLoss>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, PowerLoss*PropMagnitude[3], PropPostfix[3]);
	m_textPowerLossValue.SetText(str);
	PropCount++;
	prop_pos = m_staticPowerLoss.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_staticPowerLoss.SetWndPos(prop_pos);
	prop_pos = m_textPowerLossName.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textPowerLossName.SetWndPos(prop_pos);
	prop_pos = m_textPowerLossValue.GetWndPos();
	prop_pos.y = PropHeight[PropCount];
	m_textPowerLossValue.SetWndPos(prop_pos);
	NewAdditionalHeight += m_staticPowerLoss.GetWndSize().y;
	
	//----------------------------------------------------------------------
	//		Невозможность передвигаться бегом (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticSprintAllowed.Show(false);
	m_textSprintAllowedValue.Show(false);
	
	//------------------------------------------------------------------
	//		Дополнительный переносимый вес (недоступный параметр)
	//------------------------------------------------------------------
	m_staticAdditionalWeight.Show(false);
	m_textAdditionalWeightName.Show(false);
	m_textAdditionalWeightValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Дополнительная скорость передвижения (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticAdditionalWalkSpeed.Show(false);
	m_textAdditionalWalkSpeedName.Show(false);
	m_textAdditionalWalkSpeedValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Дополнительная скорость прыжка (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticAdditionalJumpSpeed.Show(false);
	m_textAdditionalJumpSpeedName.Show(false);
	m_textAdditionalJumpSpeedValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Восстановление сил (опциональный параметр)
	//----------------------------------------------------------------------
	float PowerRestoreSpeed = cur_helmet->m_fPowerRestoreSpeed;
	float SlotPowerRestoreSpeed = PowerRestoreSpeed;
	if (slot_helmet)
		SlotPowerRestoreSpeed = slot_helmet->m_fPowerRestoreSpeed;
	if (PowerRestoreSpeed==SlotPowerRestoreSpeed)
		m_textPowerRestoreSpeedValue.SetTextColor(color_rgba(170,170,170,255));
	else if (PowerRestoreSpeed<SlotPowerRestoreSpeed)
		m_textPowerRestoreSpeedValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textPowerRestoreSpeedValue.SetTextColor(color_rgba(0,255,0,255));
	if (PowerRestoreSpeed>=0)
		str_prefix = "+";
	else
		str_prefix = "";
	xr_sprintf(str, sizeof(str), "%s%.0f %s", str_prefix, PowerRestoreSpeed*PropMagnitude[8], PropPostfix[8]);
	m_textPowerRestoreSpeedValue.SetText(str);
	if (PowerRestoreSpeed!=0 || PowerRestoreSpeed!=SlotPowerRestoreSpeed)
	{
		PropCount++;
		prop_pos = m_staticPowerRestoreSpeed.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticPowerRestoreSpeed.SetWndPos(prop_pos);
		prop_pos = m_textPowerRestoreSpeedName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textPowerRestoreSpeedName.SetWndPos(prop_pos);
		prop_pos = m_textPowerRestoreSpeedValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textPowerRestoreSpeedValue.SetWndPos(prop_pos);
		m_staticPowerRestoreSpeed.Show(true);
		m_textPowerRestoreSpeedName.Show(true);
		m_textPowerRestoreSpeedValue.Show(true);
		NewAdditionalHeight += m_staticPowerRestoreSpeed.GetWndSize().y;
	}
	else
	{
		m_staticPowerRestoreSpeed.Show(false);
		m_textPowerRestoreSpeedName.Show(false);
		m_textPowerRestoreSpeedValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Вывод радиации (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticRadiationRestoreSpeed.Show(false);
	m_textRadiationRestoreSpeedName.Show(false);
	m_textRadiationRestoreSpeedValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Заживление ран (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticBleedingRestoreSpeed.Show(false);
	m_textBleedingRestoreSpeedName.Show(false);
	m_textBleedingRestoreSpeedValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Восстановление здоровья (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticHealthRestoreSpeed.Show(false);
	m_textHealthRestoreSpeedName.Show(false);
	m_textHealthRestoreSpeedValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Дополнительное здоровье (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticMaxHealthAdd.Show(false);
	m_textMaxHealthAddName.Show(false);
	m_textMaxHealthAddValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Контейнеры для артефактов (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticArtefactCount.Show(false);
	m_textArtefactCountName.Show(false);
	m_textArtefactCountValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Прибор ночного видения (опциональный параметр)
	//----------------------------------------------------------------------
	shared_str NightvisionSect = cur_helmet->m_NightVisionSect;
	shared_str SlotNightvisionSect = NightvisionSect;
	if (slot_helmet)
		SlotNightvisionSect = slot_helmet->m_NightVisionSect;
	int NightvisionStatus = READ_IF_EXISTS(pSettings, r_u32, NightvisionSect, "nightvision_level", 0);
	int SlotNightvisionStatus = READ_IF_EXISTS(pSettings, r_u32, SlotNightvisionSect, "nightvision_level", 0);
	if (NightvisionStatus==SlotNightvisionStatus)
		m_textNightvisionValue.SetTextColor(color_rgba(170,170,170,255));
	else if (NightvisionStatus<SlotNightvisionStatus)
		m_textNightvisionValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textNightvisionValue.SetTextColor(color_rgba(0,255,0,255));
	if (NightvisionStatus==0)
	{
		str_prefix = "ui_nightvision_apsent";
		xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	}
	else
	{
		str_prefix = "ui_nightvision_level";
		xr_sprintf(str, sizeof(str), "%d %s", NightvisionStatus, CStringTable().translate(str_prefix).c_str());
	}
	m_textNightvisionValue.SetText(str);
	if (NightvisionStatus!=0 || NightvisionStatus!=SlotNightvisionStatus)
	{
		PropCount++;
		prop_pos = m_staticNightvision.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticNightvision.SetWndPos(prop_pos);
		prop_pos = m_textNightvisionName.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textNightvisionName.SetWndPos(prop_pos);
		prop_pos = m_textNightvisionValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textNightvisionValue.SetWndPos(prop_pos);
		m_staticNightvision.Show(true);
		m_textNightvisionName.Show(true);
		m_textNightvisionValue.Show(true);
		NewAdditionalHeight += m_staticNightvision.GetWndSize().y;
	}
	else
	{
		m_staticNightvision.Show(false);
		m_textNightvisionName.Show(false);
		m_textNightvisionValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Инфракрасный сканнер (опциональный параметр)
	//----------------------------------------------------------------------
	float EnemiesScannerDist = cur_helmet->m_fShowNearestEnemiesDistance;
	float SlotEnemiesScannerDist = EnemiesScannerDist;
	bool EnemiesScanner = (EnemiesScannerDist>=25);
	if (slot_helmet)
		SlotEnemiesScannerDist = slot_helmet->m_fShowNearestEnemiesDistance;
	bool SlotEnemiesScanner = (SlotEnemiesScannerDist>=25);
	if (EnemiesScanner==SlotEnemiesScanner)
		m_textEnemiesScannerValue.SetTextColor(color_rgba(170,170,170,255));
	else if (!EnemiesScanner && SlotEnemiesScanner)
		m_textEnemiesScannerValue.SetTextColor(color_rgba(255,0,0,255));
	else
		m_textEnemiesScannerValue.SetTextColor(color_rgba(0,255,0,255));
	if (EnemiesScanner)
		str_prefix = "ui_enemies_scanner";
	else
		str_prefix = "ui_enemies_scanner_apsent";
	xr_sprintf(str, sizeof(str), "%s", CStringTable().translate(str_prefix).c_str());
	m_textEnemiesScannerValue.SetText(str);
	if (EnemiesScanner || SlotEnemiesScanner)
	{
		PropCount++;
		prop_pos = m_staticEnemiesScanner.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_staticEnemiesScanner.SetWndPos(prop_pos);
		prop_pos = m_textEnemiesScannerValue.GetWndPos();
		prop_pos.y = PropHeight[PropCount];
		m_textEnemiesScannerValue.SetWndPos(prop_pos);
		m_staticEnemiesScanner.Show(true);
		m_textEnemiesScannerValue.Show(true);
		NewAdditionalHeight += m_staticEnemiesScanner.GetWndSize().y;
	}
	else
	{
		m_staticEnemiesScanner.Show(false);
		m_textEnemiesScannerValue.Show(false);
	}
	
	//----------------------------------------------------------------------
	//		Дозиметр (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticDozimeter.Show(false);
	m_textDozimeterValue.Show(false);
	
	//----------------------------------------------------------------------
	//		Эмулятор мозговых волн (недоступный параметр)
	//----------------------------------------------------------------------
	m_staticPsyController.Show(false);
	m_textPsyControllerValue.Show(false);
	
	if (AdditionalHeight!=NewAdditionalHeight)
	{
		Fvector2 MainWndSize = GetWndSize();
		MainWndSize.y -= AdditionalHeight;
		MainWndSize.y += NewAdditionalHeight;
		SetWndSize(MainWndSize);
		AdditionalHeight = NewAdditionalHeight;
	}
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)
}