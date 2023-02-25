#include "stdafx.h"
#include "UIBoosterInfo.h"
#include "UIStatic.h"
#include "object_broker.h"
#include "../EntityCondition.h"
#include "..\actor.h"
#include "../ActorCondition.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../string_table.h"

CUIBoosterInfo::CUIBoosterInfo()
{
	for(u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		m_booster_items[i] = NULL;
	}
	m_booster_satiety = NULL;
	m_booster_anabiotic = NULL;
	m_booster_time = NULL;
	// Lex Addon (correct by Suhar_) 		(begin)
	// 8.07.2016
	// Физическая имунность
	m_booster_wound_immunity = NULL;
	// 7.07.2016
	// Восстановление здоровья (мгновенное)
	m_booster_health = NULL;
	// Заживление ран (мгновенное)
	m_booster_wounds_heal_perc = NULL;
	// Вывод радиации
	m_booster_radiation = NULL;
	// Восстановление сил (мгновенное)
	m_booster_power = NULL;
	// Восстановление пси-здоровья (мгновенное)
	m_booster_psy_health = NULL;
	// Восстановление пси-здоровья (длительное)
	m_booster_psy_health_restore = NULL;
	// Изменение фактора сна
	m_booster_sleep_factor_add = NULL;
	// Опьянение
	m_booster_alcohol = NULL;
	// Lex Addon (correct by Suhar_) 				(end)
}

CUIBoosterInfo::~CUIBoosterInfo()
{
	delete_data(m_booster_items);
	xr_delete(m_booster_satiety);
	xr_delete(m_booster_anabiotic);
	xr_delete(m_booster_time);
	// Lex Addon (correct by Suhar_) 			(begin)
	// 8.07.2016
	// Физическая имунность
	xr_delete(m_booster_wound_immunity);
	// 7.07.2016
	// Восстановление здоровья (мгновенное)
	xr_delete(m_booster_health);
	// Заживление ран (мгновенное)
	xr_delete(m_booster_wounds_heal_perc);
	// Вывод радиации
	xr_delete(m_booster_radiation);
	// Восстановление сил (мгновенное)
	xr_delete(m_booster_power);
	// Восстановление пси-здоровья (мгновенное)
	xr_delete(m_booster_psy_health);
	// Восстановление пси-здоровья (длительное)
	xr_delete(m_booster_psy_health_restore);
	// Изменение фактора сна
	xr_delete(m_booster_sleep_factor_add);
	// Опьянение
	xr_delete(m_booster_alcohol);
	// Lex Addon (correct by Suhar_)			(end)
	xr_delete(m_Prop_line);
}

LPCSTR boost_influence_caption[] =
{
	"ui_inv_health",
	"ui_inv_power",
	"ui_inv_radiation",
	"ui_inv_bleeding",
	"ui_inv_outfit_additional_weight",
	"ui_inv_outfit_radiation_protection",
	"ui_inv_outfit_telepatic_protection",
	"ui_inv_outfit_chemical_burn_protection",
	"ui_inv_outfit_burn_immunity",
	"ui_inv_outfit_shock_immunity",
	"ui_inv_outfit_radiation_immunity",
	"ui_inv_outfit_telepatic_immunity",
	"ui_inv_outfit_chemical_burn_immunity"
};

void CUIBoosterInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base	= "booster_params";
	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if(!base_node)
		return;

	CUIXmlInit::InitWindow(xml, base, 0, this);
	xml.SetLocalRoot(base_node);
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);	
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	for(u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		m_booster_items[i] = xr_new<UIBoosterInfoItem>();
		m_booster_items[i]->Init(xml, ef_boosters_section_names[i]);
		m_booster_items[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(boost_influence_caption[i]).c_str();
		m_booster_items[i]->SetCaption(name);

		xml.SetLocalRoot(base_node);
	}

	// Lex Addon (correct by Suhar_) 8.07.2016		(begin)
	m_booster_wound_immunity = xr_new<UIBoosterInfoItem>();
	m_booster_wound_immunity->Init(xml, "boost_wound_immunity");
	m_booster_wound_immunity->SetAutoDelete(false);
	LPCSTR name = CStringTable().translate("ui_inv_outfit_wound_immunity").c_str();
	m_booster_wound_immunity->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Lex Addon (correct by Suhar_) 8.07.2016		(end)
	
	m_booster_satiety = xr_new<UIBoosterInfoItem>();
	m_booster_satiety->Init(xml, "boost_satiety");
	m_booster_satiety->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_satiety").c_str();
	m_booster_satiety->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_booster_anabiotic = xr_new<UIBoosterInfoItem>();
	m_booster_anabiotic->Init(xml, "boost_anabiotic");
	m_booster_anabiotic->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_survive_surge").c_str();
	m_booster_anabiotic->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_booster_time = xr_new<UIBoosterInfoItem>();
	m_booster_time->Init(xml, "boost_time");
	m_booster_time->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_effect_time").c_str();
	m_booster_time->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	// Lex Addon (correct by Suhar_) 7.07.2016		(begin)
	// Отображаем у бустеров недостающие свойства
	// Восстановление здоровья (мгновенное)
	m_booster_health = xr_new<UIBoosterInfoItem>();
	m_booster_health->Init(xml, "boost_health");
	m_booster_health->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_fast_health").c_str();
	m_booster_health->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Заживление ран (мгновенное)
	m_booster_wounds_heal_perc = xr_new<UIBoosterInfoItem>();
	m_booster_wounds_heal_perc->Init(xml, "boost_wounds_heal");
	m_booster_wounds_heal_perc->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_fast_wounds_heal").c_str();
	m_booster_wounds_heal_perc->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Вывод радиации (мгновенный)
	m_booster_radiation = xr_new<UIBoosterInfoItem>();
	m_booster_radiation->Init(xml, "boost_radiation");
	m_booster_radiation->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_fast_radiation").c_str();
	m_booster_radiation->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Восстановление сил (мгновенное)
	m_booster_power = xr_new<UIBoosterInfoItem>();
	m_booster_power->Init(xml, "boost_power");
	m_booster_power->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_fast_power").c_str();
	m_booster_power->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Восстановление пси-здоровья (мгновенное)
	m_booster_psy_health = xr_new<UIBoosterInfoItem>();
	m_booster_psy_health->Init(xml, "boost_psy_health");
	m_booster_psy_health->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_fast_psy_health").c_str();
	m_booster_psy_health->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Восстановление пси-здоровья (длительное)
	m_booster_psy_health_restore = xr_new<UIBoosterInfoItem>();
	m_booster_psy_health_restore->Init(xml, "boost_psy_health_restore");
	m_booster_psy_health_restore->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_psy_health_restore").c_str();
	m_booster_psy_health_restore->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Изменение фактора сна
	m_booster_sleep_factor_add = xr_new<UIBoosterInfoItem>();
	m_booster_sleep_factor_add->Init(xml, "boost_sleep_factor_add");
	m_booster_sleep_factor_add->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_sleep_factor_add").c_str();
	m_booster_sleep_factor_add->SetCaption(name);
	xml.SetLocalRoot( base_node );
	// Опьянение
	m_booster_alcohol = xr_new<UIBoosterInfoItem>();
	m_booster_alcohol->Init(xml, "boost_alcohol");
	m_booster_alcohol->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_alcohol").c_str();
	m_booster_alcohol->SetCaption(name);
	xml.SetLocalRoot( stored_root );
	// Lex Addon (correct by Suhar_) 7.07.2016		(end)
}

void CUIBoosterInfo::SetInfo( shared_str const& section )
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}

	CEntityCondition::BOOSTER_MAP boosters = actor->conditions().GetCurBoosterInfluences();

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	for (u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		if(pSettings->line_exist(section.c_str(), ef_boosters_section_names[i]))
		{
			val	= pSettings->r_float(section, ef_boosters_section_names[i]);
			if(fis_zero(val))
				continue;

			EBoostParams type = (EBoostParams)i;
			switch(type)
			{
				case eBoostHpRestore: 
				case eBoostPowerRestore: 
				case eBoostBleedingRestore: 
				case eBoostMaxWeight: 
					max_val = 1.0f;
					break;
				case eBoostRadiationRestore:
					max_val = -1.0f;
					break;
				case eBoostBurnImmunity:
					max_val = actor->conditions().GetZoneMaxPower(ALife::infl_fire);
					break;
				case eBoostShockImmunity:
					max_val = actor->conditions().GetZoneMaxPower(ALife::infl_electra);
					break;
				case eBoostRadiationImmunity:
				case eBoostRadiationProtection: 
					max_val = actor->conditions().GetZoneMaxPower(ALife::infl_rad);
					break;
				case eBoostTelepaticImmunity:
				case eBoostTelepaticProtection: 
					max_val = actor->conditions().GetZoneMaxPower(ALife::infl_psi);
					break;
				case eBoostChemicalBurnImmunity:
				case eBoostChemicalBurnProtection: 
					max_val = actor->conditions().GetZoneMaxPower(ALife::infl_acid);
					break;
			}
			val /= max_val;
			m_booster_items[i]->SetValue(val);

			pos.set(m_booster_items[i]->GetWndPos());
			pos.y = h;
			m_booster_items[i]->SetWndPos(pos);

			h += m_booster_items[i]->GetWndSize().y;
			AttachChild(m_booster_items[i]);
		}
	}
	
	// Lex Addon (correct by Suhar_) 8.07.2016		(begin)
	// Физическая имунность
	if(pSettings->line_exist(section.c_str(), "boost_fire_wound_immunity"))
	{
		val	= pSettings->r_float(section, "boost_fire_wound_immunity");
		if(!fis_zero(val))
		{
			m_booster_wound_immunity->SetValue(val);
			pos.set(m_booster_wound_immunity->GetWndPos());
			pos.y = h;
			m_booster_wound_immunity->SetWndPos(pos);

			h += m_booster_wound_immunity->GetWndSize().y;
			AttachChild(m_booster_wound_immunity);
		}
	}
	// Lex Addon (correct by Suhar_) 8.07.2016		(end)

	if(pSettings->line_exist(section.c_str(), "eat_satiety"))
	{
		val	= pSettings->r_float(section, "eat_satiety");
		if(!fis_zero(val))
		{
			m_booster_satiety->SetValue(val);
			pos.set(m_booster_satiety->GetWndPos());
			pos.y = h;
			m_booster_satiety->SetWndPos(pos);

			h += m_booster_satiety->GetWndSize().y;
			AttachChild(m_booster_satiety);
		}
	}

	if(!xr_strcmp(section.c_str(), "drug_anabiotic"))
	{
		pos.set(m_booster_anabiotic->GetWndPos());
		pos.y = h;
		m_booster_anabiotic->SetWndPos(pos);

		h += m_booster_anabiotic->GetWndSize().y;
		AttachChild(m_booster_anabiotic);
	}

	if(pSettings->line_exist(section.c_str(), "boost_time"))
	{
		val	= pSettings->r_float(section, "boost_time");
		if(!fis_zero(val))
		{
			m_booster_time->SetValue(val);
			pos.set(m_booster_time->GetWndPos());
			pos.y = h;
			m_booster_time->SetWndPos(pos);

			h += m_booster_time->GetWndSize().y;
			AttachChild(m_booster_time);
		}
	}
	
	// Lex Addon (correct by Suhar_) 7.07.2016		(begin)
	// Восстановление здоровья (мгновенное)
	if(pSettings->line_exist(section.c_str(), "eat_health"))
	{
		val	= pSettings->r_float(section, "eat_health");
		if(!fis_zero(val))
		{
			m_booster_health->SetValue(val);
			pos.set(m_booster_health->GetWndPos());
			pos.y = h;
			m_booster_health->SetWndPos(pos);

			h += m_booster_health->GetWndSize().y;
			AttachChild(m_booster_health);
		}
	}
	// Заживление ран (мгновенное)
	if(pSettings->line_exist(section.c_str(), "wounds_heal_perc"))
	{
		val	= pSettings->r_float(section, "wounds_heal_perc");
		if(!fis_zero(val))
		{
			m_booster_wounds_heal_perc->SetValue(val);
			pos.set(m_booster_wounds_heal_perc->GetWndPos());
			pos.y = h;
			m_booster_wounds_heal_perc->SetWndPos(pos);

			h += m_booster_wounds_heal_perc->GetWndSize().y;
			AttachChild(m_booster_wounds_heal_perc);
		}
	}
	// Вывод радиации (мгновенный)
	if(pSettings->line_exist(section.c_str(), "eat_radiation"))
	{
		val	= pSettings->r_float(section, "eat_radiation");
		if(!fis_zero(val))
		{
			m_booster_radiation->SetValue(val);
			pos.set(m_booster_radiation->GetWndPos());
			pos.y = h;
			m_booster_radiation->SetWndPos(pos);

			h += m_booster_radiation->GetWndSize().y;
			AttachChild(m_booster_radiation);
		}
	}
	// Восстановление сил (мгновенное)
	if(pSettings->line_exist(section.c_str(), "eat_power"))
	{
		val	= pSettings->r_float(section, "eat_power");
		if(!fis_zero(val))
		{
			m_booster_power->SetValue(val);
			pos.set(m_booster_power->GetWndPos());
			pos.y = h;
			m_booster_power->SetWndPos(pos);

			h += m_booster_power->GetWndSize().y;
			AttachChild(m_booster_power);
		}
	}
	// Восстановление пси-здоровья (мгновенное)
	if(pSettings->line_exist(section.c_str(), "eat_psy_health"))
	{
		val	= pSettings->r_float(section, "eat_psy_health");
		if(!fis_zero(val))
		{
			m_booster_psy_health->SetValue(val);
			pos.set(m_booster_psy_health->GetWndPos());
			pos.y = h;
			m_booster_psy_health->SetWndPos(pos);

			h += m_booster_psy_health->GetWndSize().y;
			AttachChild(m_booster_psy_health);
		}
	}
	// Восстановление пси-здоровья (длительное)
	if(pSettings->line_exist(section.c_str(), "boost_psy_health_restore"))
	{
		val	= pSettings->r_float(section, "boost_psy_health_restore");
		if(!fis_zero(val))
		{
			m_booster_psy_health_restore->SetValue(val);
			pos.set(m_booster_psy_health_restore->GetWndPos());
			pos.y = h;
			m_booster_psy_health_restore->SetWndPos(pos);

			h += m_booster_psy_health_restore->GetWndSize().y;
			AttachChild(m_booster_psy_health_restore);
		}
	}
	// Изменение фактора сна
	if(pSettings->line_exist(section.c_str(), "actor_sleep_factor_add"))
	{
		val	= pSettings->r_float(section, "actor_sleep_factor_add");
		if(!fis_zero(val))
		{
			m_booster_sleep_factor_add->SetValue(val);
			pos.set(m_booster_sleep_factor_add->GetWndPos());
			pos.y = h;
			m_booster_sleep_factor_add->SetWndPos(pos);

			h += m_booster_sleep_factor_add->GetWndSize().y;
			AttachChild(m_booster_sleep_factor_add);
		}
	}
	// Опьянение
	if(pSettings->line_exist(section.c_str(), "eat_alcohol"))
	{
		val	= pSettings->r_float(section, "eat_alcohol");
		if(!fis_zero(val))
		{
			m_booster_alcohol->SetValue(val);
			pos.set(m_booster_alcohol->GetWndPos());
			pos.y = h;
			m_booster_alcohol->SetWndPos(pos);

			h += m_booster_alcohol->GetWndSize().y;
			AttachChild(m_booster_alcohol);
		}
	}
	// Lex Addon (correct by Suhar_) 7.07.2016		(end)
	
	SetHeight(h);
}

/// ----------------------------------------------------------------

UIBoosterInfoItem::UIBoosterInfoItem()
{
	m_caption				= NULL;
	m_value					= NULL;
	m_magnitude				= 1.0f;
	m_show_sign				= false;
	// Lex Addon (correct by Suhar_) 11.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах бустеров
	m_accuracy = 0;
	// Lex Addon (correct by Suhar_) 11.03.2017		(end)
	
	m_unit_str._set			("");
	m_texture_minus._set	("");
	m_texture_plus._set		("");
}

UIBoosterInfoItem::~UIBoosterInfoItem()
{
}

void UIBoosterInfoItem::Init(CUIXml& xml, LPCSTR section)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_caption   = UIHelper::CreateStatic(xml, "caption", this);
	m_value     = UIHelper::CreateTextWnd(xml, "value",   this);
	m_magnitude = xml.ReadAttribFlt("value", 0, "magnitude", 1.0f);
	m_show_sign = (xml.ReadAttribInt("value", 0, "show_sign", 1) == 1);
	// Lex Addon (correct by Suhar_) 11.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах бустеров
	m_accuracy = xml.ReadAttribInt( "value", 0, "accuracy", 0 );
	// Lex Addon (correct by Suhar_) 11.03.2017		(end)
	
	LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
	m_unit_str._set(CStringTable().translate(unit_str));
	
	LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
	if(texture_minus && xr_strlen(texture_minus))
	{
		m_texture_minus._set(texture_minus);
		
		LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
		m_texture_plus._set(texture_plus);
		VERIFY(m_texture_plus.size());
	}
}

void UIBoosterInfoItem::SetCaption(LPCSTR name)
{
	m_caption->TextItemControl()->SetText(name);
}

void UIBoosterInfoItem::SetValue(float value)
{
	value *= m_magnitude;
	string32 buf;
	// Lex Addon (correct by Suhar_) 11.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах бустеров
	if (m_accuracy==0)
	{
		if(m_show_sign)
			xr_sprintf(buf, "%+.0f", value);
		else
			xr_sprintf(buf, "%.0f", value);
	}
	else if (m_accuracy==1)
	{
		if(m_show_sign)
			xr_sprintf(buf, "%+.1f", value);
		else
			xr_sprintf(buf, "%.1f", value);
	}
	else if (m_accuracy==2)
	{
		if(m_show_sign)
			xr_sprintf(buf, "%+.2f", value);
		else
			xr_sprintf(buf, "%.2f", value);
	}
	else if (m_accuracy>=3)
	{
		if(m_show_sign)
			xr_sprintf(buf, "%+.3f", value);
		else
			xr_sprintf(buf, "%.3f", value);
	}
	// Lex Addon (correct by Suhar_) 11.03.2017		(end)
	
	LPSTR str;
	if(m_unit_str.size())
		STRCONCAT(str, buf, " ", m_unit_str.c_str());
	else
		STRCONCAT(str, buf);

	m_value->SetText(str);

	bool positive = (value >= 0.0f);
	m_value->SetTextColor(color_rgba(170,170,170,255));

	if(m_texture_minus.size())
	{
		if(positive)
			m_caption->InitTexture(m_texture_plus.c_str());
		else
			m_caption->InitTexture(m_texture_minus.c_str());
	}
}
