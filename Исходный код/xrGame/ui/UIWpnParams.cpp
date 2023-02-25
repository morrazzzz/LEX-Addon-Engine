#include "pch_script.h"
#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "../level.h"
#include "game_base_space.h"
#include "../ai_space.h"
#include "../../xrServerEntities/script_engine.h"
#include "inventory_item_object.h"
#include "UIInventoryUtilities.h"
#include "Weapon.h"

struct SLuaWpnParams
{
	luabind::functor<float>		m_functorRPM;
	luabind::functor<float>		m_functorAccuracy;
	luabind::functor<float>		m_functorDamage;
	luabind::functor<float>		m_functorDamageMP;
	luabind::functor<float>		m_functorHandling;
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Инициализируем скриптовые функции
	luabind::functor<float>		m_functorBulletSpeed;
	luabind::functor<float>		m_functorRecoil;
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)

	SLuaWpnParams();
	~SLuaWpnParams();
};

SLuaWpnParams::SLuaWpnParams()
{
	bool	functor_exists;
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetRPM",			m_functorRPM);			VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamage",		m_functorDamage);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamageMP", 	m_functorDamageMP);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetHandling", 	m_functorHandling);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetAccuracy", 	m_functorAccuracy);		VERIFY(functor_exists);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Устанавливаем соответствие между функциями в движке и их аналогами в скриптах
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetBulletSpeed",	m_functorBulletSpeed);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetRecoil",		m_functorRecoil);		VERIFY(functor_exists);
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)
}

SLuaWpnParams::~SLuaWpnParams()
{
}

SLuaWpnParams* g_lua_wpn_params = NULL;

void destroy_lua_wpn_params()
{
	if(g_lua_wpn_params)
		xr_delete(g_lua_wpn_params);
}

// =====================================================================

CUIWpnParams::CUIWpnParams()
{
	AttachChild(&m_Prop_line);

	AttachChild(&m_icon_acc);
	AttachChild(&m_icon_dam);
	AttachChild(&m_icon_han);
	AttachChild(&m_icon_rpm);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Инициализируем необходимые элементы (иконки, заголовки, ползунки)
	AttachChild(&m_icon_spe);
	AttachChild(&m_icon_rec);

	AttachChild(&m_textAccuracy);
	AttachChild(&m_textDamage);
	AttachChild(&m_textHandling);
	AttachChild(&m_textRPM);
	AttachChild(&m_textBulletSpeed);
	AttachChild(&m_textRecoil);

	AttachChild(&m_progressAccuracy);
	AttachChild(&m_progressDamage);
	AttachChild(&m_progressHandling);
	AttachChild(&m_progressRPM);
	AttachChild(&m_progressBulletSpeed);
	AttachChild(&m_progressRecoil);
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)

	AttachChild(&m_stAmmo);
	AttachChild(&m_textAmmoCount);
	AttachChild(&m_textAmmoCount2);
	AttachChild(&m_textAmmoTypes);
	AttachChild(&m_textAmmoUsedType);
	AttachChild(&m_stAmmoType1);
	AttachChild(&m_stAmmoType2);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Инициализируем переменные для отображения ещё 4 типов патронов в свойствах оружия
	AttachChild(&m_stAmmoType3);
	AttachChild(&m_stAmmoType4);
	AttachChild(&m_stAmmoType5);
	AttachChild(&m_stAmmoType6);
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)
}

CUIWpnParams::~CUIWpnParams()
{
}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc)
{
	if (!xml_doc.NavigateToNode("wpn_params", 0))	return;
	CUIXmlInit::InitWindow				(xml_doc, "wpn_params",						0, this);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:prop_line",			0, &m_Prop_line);

	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:static_accuracy",		0, &m_icon_acc);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:static_damage",		0, &m_icon_dam);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:static_handling",		0, &m_icon_han);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:static_rpm",			0, &m_icon_rpm);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Читаем необходимые параметры из конфига
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:static_bullet_speed",	0, &m_icon_spe);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:static_recoil",		0, &m_icon_rec);

	CUIXmlInit::InitTextWnd				(xml_doc, "wpn_params:cap_accuracy",		0, &m_textAccuracy);
	CUIXmlInit::InitTextWnd				(xml_doc, "wpn_params:cap_damage",			0, &m_textDamage);
	CUIXmlInit::InitTextWnd				(xml_doc, "wpn_params:cap_handling",		0, &m_textHandling);
	CUIXmlInit::InitTextWnd				(xml_doc, "wpn_params:cap_rpm",				0, &m_textRPM);
	CUIXmlInit::InitTextWnd				(xml_doc, "wpn_params:cap_bullet_speed",	0, &m_textBulletSpeed);
	CUIXmlInit::InitTextWnd				(xml_doc, "wpn_params:cap_recoil",			0, &m_textRecoil);

	m_progressAccuracy.InitFromXml		( xml_doc, "wpn_params:progress_accuracy" );
	m_progressDamage.InitFromXml		( xml_doc, "wpn_params:progress_damage" );
	m_progressHandling.InitFromXml		( xml_doc, "wpn_params:progress_handling" );
	m_progressRPM.InitFromXml			( xml_doc, "wpn_params:progress_rpm" );
	m_progressBulletSpeed.InitFromXml	( xml_doc, "wpn_params:progress_bullet_speed" );
	m_progressRecoil.InitFromXml		( xml_doc, "wpn_params:progress_recoil" );
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)

	if(IsGameTypeSingle())
	{
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo",			0, &m_stAmmo);
		CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_ammo_count",		0, &m_textAmmoCount);
		CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_ammo_count2",		0, &m_textAmmoCount2);
		CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_ammo_types",		0, &m_textAmmoTypes);
		CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_ammo_used_type",	0, &m_textAmmoUsedType);
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo_type1",	0, &m_stAmmoType1);
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo_type2",	0, &m_stAmmoType2);
		// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
		// Читаем параметры отображения отображения ещё 4 типов патронов в свойствах оружия
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo_type3",	0, &m_stAmmoType3);
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo_type4",	0, &m_stAmmoType4);
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo_type5",	0, &m_stAmmoType5);
		CUIXmlInit::InitStatic			(xml_doc, "wpn_params:static_ammo_type6",	0, &m_stAmmoType6);
		// Lex Addon (correct by Suhar_) 7.08.2018		(end)
	}

}

void CUIWpnParams::SetInfo( CInventoryItem* slot_wpn, CInventoryItem& cur_wpn )
{
	if ( !g_lua_wpn_params )
	{
		g_lua_wpn_params = xr_new<SLuaWpnParams>();
	}
	
	LPCSTR cur_section  = cur_wpn.object().cNameSect().c_str();
	string2048 str_upgrades;
	str_upgrades[0] = 0;
	cur_wpn.get_upgrades_str( str_upgrades );

	float cur_rpm		= iFloor(g_lua_wpn_params->m_functorRPM( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_accur		= iFloor(g_lua_wpn_params->m_functorAccuracy( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_hand		= iFloor(g_lua_wpn_params->m_functorHandling( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_damage	= ( GameID() == eGameIDSingle ) ?
		iFloor(g_lua_wpn_params->m_functorDamage( cur_section, str_upgrades )*53.0f)/53.0f
		: iFloor(g_lua_wpn_params->m_functorDamageMP( cur_section, str_upgrades )*53.0f)/53.0f;
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Обрабатываем значения из скриптовых функций и передаём их ползункам
	float cur_speed		= iFloor(g_lua_wpn_params->m_functorBulletSpeed( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_recoil	= iFloor(g_lua_wpn_params->m_functorRecoil( cur_section, str_upgrades )*53.0f)/53.0f;

	float slot_rpm		= cur_rpm;
	float slot_accur	= cur_accur;
	float slot_hand		= cur_hand;
	float slot_damage	= cur_damage;
	float slot_speed	= cur_speed;
	float slot_recoil	= cur_recoil;

	if ( slot_wpn && (slot_wpn != &cur_wpn) )
	{
		LPCSTR slot_section  = slot_wpn->object().cNameSect().c_str();
		str_upgrades[0] = 0;
		slot_wpn->get_upgrades_str( str_upgrades );

		slot_rpm    = iFloor(g_lua_wpn_params->m_functorRPM( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_accur  = iFloor(g_lua_wpn_params->m_functorAccuracy( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_hand   = iFloor(g_lua_wpn_params->m_functorHandling( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_damage = ( GameID() == eGameIDSingle ) ?
			iFloor(g_lua_wpn_params->m_functorDamage( slot_section, str_upgrades )*53.0f)/53.0f
			: iFloor(g_lua_wpn_params->m_functorDamageMP( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_speed	= iFloor(g_lua_wpn_params->m_functorBulletSpeed( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_recoil	= iFloor(g_lua_wpn_params->m_functorRecoil( slot_section, str_upgrades )*53.0f)/53.0f;
	}
	
	m_progressAccuracy.SetTwoPos	( cur_accur,	slot_accur );
	m_progressDamage.SetTwoPos		( cur_damage,	slot_damage );
	m_progressHandling.SetTwoPos	( cur_hand,		slot_hand );
	m_progressRPM.SetTwoPos			( cur_rpm,		slot_rpm );
	m_progressBulletSpeed.SetTwoPos	( cur_speed,	slot_speed );
	m_progressRecoil.SetTwoPos		( cur_recoil,	slot_recoil );
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)

	if(IsGameTypeSingle())
	{
		// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
		// Инициализируем переменную используемых оружием патронов
		xr_vector<shared_str> ammo_types;

		CWeapon* weapon = cur_wpn.cast_weapon();
		if(!weapon)
			return;

		int ammo_count = weapon->GetAmmoMagSize();
		int ammo_count2 = ammo_count;

		if(slot_wpn)
		{
			CWeapon* slot_weapon = slot_wpn->cast_weapon();
			if(slot_weapon)
				ammo_count2 = slot_weapon->GetAmmoMagSize(); 
		}

		if(ammo_count==ammo_count2)
			m_textAmmoCount2.SetTextColor(color_rgba(170,170,170,255));
		else if(ammo_count<ammo_count2)
			m_textAmmoCount2.SetTextColor(color_rgba(255,0,0,255));
		else
			m_textAmmoCount2.SetTextColor(color_rgba(0,255,0,255));

		string128 str;
		xr_sprintf(str, sizeof(str), "%d", ammo_count);
		m_textAmmoCount2.SetText(str);
		
		// Получаем используемые оружием типы патронов
		ammo_types = weapon->m_ammoTypes;
		if(ammo_types.empty())
			return;
		// Получаем количчество видов используемых оружием патронов
		ammo_types_size = ammo_types.size();

		xr_sprintf(str, sizeof(str), "%s", pSettings->r_string(ammo_types[0].c_str(), "inv_name_short"));
		m_textAmmoUsedType.SetTextST(str);

		m_stAmmoType1.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		Frect				tex_rect;
		tex_rect.x1			= float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
		tex_rect.y1			= float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
		tex_rect.x2			= float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_width") * INV_GRID_WIDTH );
		tex_rect.y2			= float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
		tex_rect.rb.add		(tex_rect.lt);
		m_stAmmoType1.SetTextureRect(tex_rect);
		m_stAmmoType1.TextureOn();
		m_stAmmoType1.SetStretchTexture(true);
		m_stAmmoType1.SetWndSize(Fvector2().set((tex_rect.x2-tex_rect.x1)*UI().get_current_kx(), tex_rect.y2-tex_rect.y1));
		
		// Если оружие использует только один тип патронов, то необходимо оставить переменные 2-го, 3-го, 4-го, 5-го и 6-го типов пустыми
		// Аналогично, если оружие использует другое число патронов - оставшиеся невостребованные переменные делаем пустыми
		m_stAmmoType2.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		if(ammo_types_size<=1)
		{
			tex_rect.set(0,0,1,1);
		}
		else
		{
			tex_rect.x1			= float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
			tex_rect.y1			= float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
			tex_rect.x2			= float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_width") * INV_GRID_WIDTH );
			tex_rect.y2			= float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
			tex_rect.rb.add		(tex_rect.lt);
		}
		m_stAmmoType2.SetTextureRect(tex_rect);
		m_stAmmoType2.TextureOn();
		m_stAmmoType2.SetStretchTexture(true);
		m_stAmmoType2.SetWndSize(Fvector2().set((tex_rect.x2-tex_rect.x1)*UI().get_current_kx(), tex_rect.y2-tex_rect.y1));
		
		m_stAmmoType3.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		if(ammo_types_size<=2)
		{
			tex_rect.set(0,0,1,1);
		}
		else
		{
			tex_rect.x1			= float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
			tex_rect.y1			= float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
			tex_rect.x2			= float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_width") * INV_GRID_WIDTH );
			tex_rect.y2			= float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
			tex_rect.rb.add		(tex_rect.lt);
		}
		m_stAmmoType3.SetTextureRect(tex_rect);
		m_stAmmoType3.TextureOn();
		m_stAmmoType3.SetStretchTexture(true);
		m_stAmmoType3.SetWndSize(Fvector2().set((tex_rect.x2-tex_rect.x1)*UI().get_current_kx(), tex_rect.y2-tex_rect.y1));
		
		m_stAmmoType4.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		if(ammo_types_size<=3)
		{
			tex_rect.set(0,0,1,1);
		}
		else
		{
			tex_rect.x1			= float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
			tex_rect.y1			= float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
			tex_rect.x2			= float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_width") * INV_GRID_WIDTH );
			tex_rect.y2			= float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
			tex_rect.rb.add		(tex_rect.lt);
		}
		m_stAmmoType4.SetTextureRect(tex_rect);
		m_stAmmoType4.TextureOn();
		m_stAmmoType4.SetStretchTexture(true);
		m_stAmmoType4.SetWndSize(Fvector2().set((tex_rect.x2-tex_rect.x1)*UI().get_current_kx(), tex_rect.y2-tex_rect.y1));
		
		m_stAmmoType5.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		if(ammo_types_size<=4)
		{
			tex_rect.set(0,0,1,1);
		}
		else
		{
			tex_rect.x1			= float(pSettings->r_u32(ammo_types[4].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
			tex_rect.y1			= float(pSettings->r_u32(ammo_types[4].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
			tex_rect.x2			= float(pSettings->r_u32(ammo_types[4].c_str(), "inv_grid_width") * INV_GRID_WIDTH );
			tex_rect.y2			= float(pSettings->r_u32(ammo_types[4].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
			tex_rect.rb.add		(tex_rect.lt);
		}
		m_stAmmoType5.SetTextureRect(tex_rect);
		m_stAmmoType5.TextureOn();
		m_stAmmoType5.SetStretchTexture(true);
		m_stAmmoType5.SetWndSize(Fvector2().set((tex_rect.x2-tex_rect.x1)*UI().get_current_kx(), tex_rect.y2-tex_rect.y1));
		
		m_stAmmoType6.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		if(ammo_types_size<=5)
		{
			tex_rect.set(0,0,1,1);
		}
		else
		{
			tex_rect.x1			= float(pSettings->r_u32(ammo_types[5].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
			tex_rect.y1			= float(pSettings->r_u32(ammo_types[5].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
			tex_rect.x2			= float(pSettings->r_u32(ammo_types[5].c_str(), "inv_grid_width") * INV_GRID_WIDTH );
			tex_rect.y2			= float(pSettings->r_u32(ammo_types[5].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
			tex_rect.rb.add		(tex_rect.lt);
		}
		m_stAmmoType6.SetTextureRect(tex_rect);
		m_stAmmoType6.TextureOn();
		m_stAmmoType6.SetStretchTexture(true);
		m_stAmmoType6.SetWndSize(Fvector2().set((tex_rect.x2-tex_rect.x1)*UI().get_current_kx(), tex_rect.y2-tex_rect.y1));
		// Lex Addon (correct by Suhar_) 7.08.2018		(end)
	}
}

// Lex Addon (correct by Suhar_) 28.09.2016		(begin)
// Проверяем, можно ли отображать свойства оружия
bool CUIWpnParams::Check(const shared_str& wpn_section)
{
	// Свойства будут выводиться только для оружия, имеющего в секции параметр fire_dispersion_base
	if (pSettings->line_exist(wpn_section, "fire_dispersion_base"))
	{
        // Проверяем, есть ли в секции параметр класса
		if (pSettings->line_exist(wpn_section, "class"))
		{
			// Читаем параметр класса
			shared_str class_name = pSettings->r_string(wpn_section,"class");
			// Запрещаем выводить свойства для ножей
			if (class_name=="WP_KNIFE" || class_name=="W_KNIFE")
				return false;
			// Запрещаем выводить свойства для биноклей
			if (class_name=="WP_BINOC" || class_name=="W_BINOC")
				return false;
			// Запрещаем выводить свойства для глушителей
			if (class_name=="WP_SILEN" || class_name=="W_SILENC")
				return false;		
		}
        return true;		
	}
	return false;
}
// Lex Addon (correct by Suhar_) 28.09.2016		(end)

// -------------------------------------------------------------------------------------------------

CUIConditionParams::CUIConditionParams()
{
	AttachChild( &m_progress );
	AttachChild( &m_text );
}

CUIConditionParams::~CUIConditionParams()
{
}

void CUIConditionParams::InitFromXml(CUIXml& xml_doc)
{
	if (!xml_doc.NavigateToNode("condition_params", 0))	return;
	CUIXmlInit::InitWindow	(xml_doc, "condition_params", 0, this);
	CUIXmlInit::InitStatic	( xml_doc, "condition_params:caption", 0, &m_text );
	m_progress.InitFromXml	( xml_doc, "condition_params:progress_state" );
}

void CUIConditionParams::SetInfo( CInventoryItem const* slot_item, CInventoryItem const& cur_item )
{
	float cur_value  = cur_item.GetConditionToShow() * 100.0f + 1.0f - EPS;
	float slot_value = cur_value;

	if ( slot_item && (slot_item != &cur_item) /*&& (cur_item.object().cNameSect()._get() == slot_item->object().cNameSect()._get())*/ )
	{
		slot_value = slot_item->GetConditionToShow() * 100.0f + 1.0f - EPS;
	}
	m_progress.SetTwoPos( cur_value, slot_value );
}
