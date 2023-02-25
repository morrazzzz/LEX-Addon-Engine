#include "stdafx.h"
#include "ui_af_params.h"
#include "UIStatic.h"

#include "..\actor.h"
#include "..\ActorCondition.h"
#include "object_broker.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../string_table.h"

u32 const red_clr   = color_argb(255,210,50,50);
u32 const green_clr = color_argb(255,170,170,170);

CUIArtefactParams::CUIArtefactParams()
{
	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		m_immunity_item[i] = NULL;
	}
	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		m_restore_item[i] = NULL;
	}
	m_additional_weight = NULL;
	// Lex Addon (correct by Suhar_) 				(begin)
	// Переменная дополнительного максимального здоровья 4.07.2016
	m_fMaxHealthAdd =  NULL;
	// Переменная дополнительной скорости прыжка 6.07.2016
	m_fJumpSpeedAdd = NULL;
	// Переменная дополнительной скорости передвижения
	m_fWalkAccelAdd = NULL;
	// Переменная гашения удара 7.07.2016
	eHitTypeWound = NULL;
	// Lex Addon (correct by Suhar_) 				(end)
}

CUIArtefactParams::~CUIArtefactParams()
{
	delete_data	( m_immunity_item );
	delete_data	( m_restore_item );
	xr_delete	( m_additional_weight );
	// Lex Addon (correct by Suhar_) 				(begin)
	// 4.07.2016
	// Переменная дополнительного максимального здоровья 4.07.2016
	xr_delete	( m_fMaxHealthAdd );
	// 6.07.2016
	// Переменная дополнительной скорости прыжка 6.07.2016
	xr_delete	( m_fJumpSpeedAdd );
	// Переменная дополнительной скорости передвижения
	xr_delete	( m_fWalkAccelAdd );
	// Переменная гашения удара 7.07.2016
	xr_delete	( eHitTypeWound );
	// Lex Addon (correct by Suhar_) 				(end)
	xr_delete	( m_Prop_line );
}

LPCSTR af_immunity_section_names[] = // ALife::EInfluenceType
{
	"radiation_immunity",		// infl_rad=0
	"burn_immunity",			// infl_fire=1
	"chemical_burn_immunity",	// infl_acid=2
	"telepatic_immunity",		// infl_psi=3
	"shock_immunity",			// infl_electra=4

//	"strike_immunity",
//	"wound_immunity",
//	"explosion_immunity",
//	"fire_wound_immunity",
};

LPCSTR af_restore_section_names[] = // ALife::EConditionRestoreType
{
	"health_restore_speed",			// eHealthRestoreSpeed=0
	"satiety_restore_speed",		// eSatietyRestoreSpeed=1
	"power_restore_speed",			// ePowerRestoreSpeed=2
	"bleeding_restore_speed",		// eBleedingRestoreSpeed=3
	"radiation_restore_speed",		// eRadiationRestoreSpeed=4
};

LPCSTR af_immunity_caption[] =  // ALife::EInfluenceType
{
	"ui_inv_outfit_radiation_protection",		// "(radiation_imm)",
	"ui_inv_outfit_burn_protection",			// "(burn_imm)",
	"ui_inv_outfit_chemical_burn_protection",	// "(chemical_burn_imm)",
	"ui_inv_outfit_telepatic_protection",		// "(telepatic_imm)",
	"ui_inv_outfit_shock_protection",			// "(shock_imm)",

//	"ui_inv_outfit_strike_protection",			// "(strike_imm)",
//	"ui_inv_outfit_wound_protection",			// "(wound_imm)",
//	"ui_inv_outfit_explosion_protection",		// "(explosion_imm)",
//	"ui_inv_outfit_fire_wound_protection",		// "(fire_wound_imm)",
};

LPCSTR af_restore_caption[] =  // ALife::EConditionRestoreType
{
	"ui_inv_health",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_radiation",
};

/*
LPCSTR af_actor_param_names[]=
{
	"satiety_health_v",
	"radiation_v",
	"satiety_v",
	"satiety_power_v",
	"wound_incarnation_v",
};
*/

void CUIArtefactParams::InitFromXml( CUIXml& xml )
{
	LPCSTR base	= "af_params";

	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if ( !base_node )
	{
		return;
	}
	CUIXmlInit::InitWindow( xml, base, 0, this );
	xml.SetLocalRoot( base_node );
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild( m_Prop_line );
	m_Prop_line->SetAutoDelete( false );	
	CUIXmlInit::InitStatic( xml, "prop_line", 0, m_Prop_line );

	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		m_immunity_item[i] = xr_new<UIArtefactParamItem>();
		m_immunity_item[i]->Init( xml, af_immunity_section_names[i] );
		m_immunity_item[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(af_immunity_caption[i]).c_str();
		m_immunity_item[i]->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	
	// Lex Addon (correct by Suhar_) 7.07.2016		(begin)
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) гашения удара (если значение не равно 0)
	{
		eHitTypeWound = xr_new<UIArtefactParamItem>();
		eHitTypeWound->Init( xml, "wound_immunity" );
		eHitTypeWound->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_outfit_wound_protection" ).c_str();
		eHitTypeWound->SetCaption( name );
		
		xml.SetLocalRoot( base_node );
	}
	// Lex Addon (correct by Suhar_) 7.07.2016		(end)

	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		m_restore_item[i] = xr_new<UIArtefactParamItem>();
		m_restore_item[i]->Init( xml, af_restore_section_names[i] );
		m_restore_item[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(af_restore_caption[i]).c_str();
		m_restore_item[i]->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	
	{
		m_additional_weight = xr_new<UIArtefactParamItem>();
		m_additional_weight->Init( xml, "additional_weight" );
		m_additional_weight->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_weight" ).c_str();
		m_additional_weight->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	
	// Lex Addon (correct by Suhar_) 				(begin)
	// 4.07.2016
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) здоровья (если значение не равно 0)
	{
		m_fMaxHealthAdd = xr_new<UIArtefactParamItem>();
		m_fMaxHealthAdd->Init( xml, "additional_max_health" );
		m_fMaxHealthAdd->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_max_health_add" ).c_str();
		m_fMaxHealthAdd->SetCaption( name );
		
		xml.SetLocalRoot( base_node );
	}
	// 6.07.2016
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) скорости прыжка (если значение не равно 0)
	{
		m_fJumpSpeedAdd = xr_new<UIArtefactParamItem>();
		m_fJumpSpeedAdd->Init( xml, "additional_jump_speed" );
		m_fJumpSpeedAdd->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_jump_speed_add" ).c_str();
		m_fJumpSpeedAdd->SetCaption( name );
		
		xml.SetLocalRoot( base_node );
	}
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) скорости передвижения (если значение не равно 0)
	{
		m_fWalkAccelAdd = xr_new<UIArtefactParamItem>();
		m_fWalkAccelAdd->Init( xml, "additional_walk_accel" );
		m_fWalkAccelAdd->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_walk_accel_add" ).c_str();
		m_fWalkAccelAdd->SetCaption( name );
		
		// xml.SetLocalRoot( base_node );
	}
	// Lex Addon (correct by Suhar_) 				(end)

	xml.SetLocalRoot( stored_root );
}

bool CUIArtefactParams::Check(const shared_str& af_section)
{
	return !!pSettings->line_exist(af_section, "af_actor_properties");
}

void CUIArtefactParams::SetInfo( shared_str const& af_section )
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		shared_str const& sect = pSettings->r_string( af_section, "hit_absorbation_sect" );
		val	= pSettings->r_float( sect, af_immunity_section_names[i] );
		if ( fis_zero(val) )
		{
			continue;
		}
		max_val = actor->conditions().GetZoneMaxPower( (ALife::EInfluenceType)i );
		val /= max_val;
		m_immunity_item[i]->SetValue( val );

		pos.set( m_immunity_item[i]->GetWndPos() );
		pos.y = h;
		m_immunity_item[i]->SetWndPos( pos );

		h += m_immunity_item[i]->GetWndSize().y;
		AttachChild( m_immunity_item[i] );
	}

	// Lex Addon (correct by Suhar_) 7.07.2016		(begin)
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) гашения удара (если значение не равно 0)
	{
		// Перемещаемся к чтению параметра из нужной секции
		shared_str const& sect = pSettings->r_string( af_section, "hit_absorbation_sect" );
		val	= pSettings->r_float( sect, "wound_immunity" );
		if ( !fis_zero(val) )
		{
			eHitTypeWound->SetValue( val );

			pos.set( eHitTypeWound->GetWndPos() );
			pos.y = h;
			eHitTypeWound->SetWndPos( pos );

			h += eHitTypeWound->GetWndSize().y;
			AttachChild( eHitTypeWound );
		}
	}
	// Lex Addon (correct by Suhar_) 7.07.2016		(end)
	
	{
		val	= pSettings->r_float( af_section, "additional_inventory_weight" );
		if ( !fis_zero(val) )
		{
			m_additional_weight->SetValue( val );

			pos.set( m_additional_weight->GetWndPos() );
			pos.y = h;
			m_additional_weight->SetWndPos( pos );

			h += m_additional_weight->GetWndSize().y;
			AttachChild( m_additional_weight );
		}
	}
	
	// Lex Addon (correct by Suhar_) 				(begin)
	// 4.07.2016
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) здоровья (если значение не равно 0)
	{
		val	= pSettings->r_float( af_section, "max_health_add" );
		if ( !fis_zero(val) )
		{
			m_fMaxHealthAdd->SetValue( val );

			pos.set( m_fMaxHealthAdd->GetWndPos() );
			pos.y = h;
			m_fMaxHealthAdd->SetWndPos( pos );

			h += m_fMaxHealthAdd->GetWndSize().y;
			AttachChild( m_fMaxHealthAdd );
		}
	}
	// 6.07.2016
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) скорости прыжка (если значение не равно 0)
	{
		val	= pSettings->r_float( af_section, "jump_speed_add" );
		if ( !fis_zero(val) )
		{
			m_fJumpSpeedAdd->SetValue( val );

			pos.set( m_fJumpSpeedAdd->GetWndPos() );
			pos.y = h;
			m_fJumpSpeedAdd->SetWndPos( pos );

			h += m_fJumpSpeedAdd->GetWndSize().y;
			AttachChild( m_fJumpSpeedAdd );
		}
	}
	// Отображаем в списке свойств артефакта значение увеличения (или уменьшения) скорости передвижения (если значение не равно 0)
	{
		val	= pSettings->r_float( af_section, "walk_accel_add" );
		if ( !fis_zero(val) )
		{
			m_fWalkAccelAdd->SetValue( val );

			pos.set( m_fWalkAccelAdd->GetWndPos() );
			pos.y = h;
			m_fWalkAccelAdd->SetWndPos( pos );

			h += m_fWalkAccelAdd->GetWndSize().y;
			AttachChild( m_fWalkAccelAdd );
		}
	}
	// Lex Addon (correct by Suhar_) 				(end)

	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		val	= pSettings->r_float( af_section, af_restore_section_names[i] );
		if ( fis_zero(val) )
		{
			continue;
		}
		m_restore_item[i]->SetValue( val );

		pos.set( m_restore_item[i]->GetWndPos() );
		pos.y = h;
		m_restore_item[i]->SetWndPos( pos );

		h += m_restore_item[i]->GetWndSize().y;
		AttachChild( m_restore_item[i] );
	}
	
	SetHeight( h );
}

/// ----------------------------------------------------------------

UIArtefactParamItem::UIArtefactParamItem()
{
	m_caption   = NULL;
	m_value     = NULL;
	m_magnitude = 1.0f;
	m_sign_inverse = false;
	// Lex Addon (correct by Suhar_) 10.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах артефактов
	m_accuracy = 0;
	// Lex Addon (correct by Suhar_) 10.03.2017		(end)
	
	m_unit_str._set( "" );
	m_texture_minus._set( "" );
	m_texture_plus._set( "" );
}

UIArtefactParamItem::~UIArtefactParamItem()
{
}

void UIArtefactParamItem::Init( CUIXml& xml, LPCSTR section )
{
	CUIXmlInit::InitWindow( xml, section, 0, this );
	xml.SetLocalRoot( xml.NavigateToNode( section ) );

	m_caption   = UIHelper::CreateStatic( xml, "caption", this );
	m_value     = UIHelper::CreateTextWnd( xml, "value",   this );
	m_magnitude = xml.ReadAttribFlt( "value", 0, "magnitude", 1.0f );
	m_sign_inverse = (xml.ReadAttribInt( "value", 0, "sign_inverse", 0 ) == 1);
	// Lex Addon (correct by Suhar_) 10.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах артефактов
	m_accuracy = xml.ReadAttribInt( "value", 0, "accuracy", 0 );
	// Lex Addon (correct by Suhar_) 10.03.2017		(end)
	
	LPCSTR unit_str = xml.ReadAttrib( "value", 0, "unit_str", "" );
	m_unit_str._set( CStringTable().translate( unit_str ) );
	
	LPCSTR texture_minus = xml.Read( "texture_minus", 0, "" );
	if ( texture_minus && xr_strlen(texture_minus) )
	{
		m_texture_minus._set( texture_minus );
		
		LPCSTR texture_plus = xml.Read( "caption:texture", 0, "" );
		m_texture_plus._set( texture_plus );
		VERIFY( m_texture_plus.size() );
	}
}

void UIArtefactParamItem::SetCaption( LPCSTR name )
{
	m_caption->TextItemControl()->SetText( name );
}

void UIArtefactParamItem::SetValue( float value )
{
	value *= m_magnitude;
	string32	buf;
	// Lex Addon (correct by Suhar_) 10.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах артефактов
	if (m_accuracy==0)
		xr_sprintf( buf, "%+.0f", value );
	else if (m_accuracy==1)
		xr_sprintf( buf, "%+.1f", value );
	else if (m_accuracy==2)
		xr_sprintf( buf, "%+.2f", value );
	else if (m_accuracy>=3)
		xr_sprintf( buf, "%+.3f", value );
	// Lex Addon (correct by Suhar_) 10.03.2017		(end)
	
	LPSTR		str;
	if ( m_unit_str.size() )
	{
		STRCONCAT( str, buf, " ", m_unit_str.c_str() );
	}
	else // = ""
	{
		STRCONCAT( str, buf );
	}
	m_value->SetText( str );

	bool positive = (value >= 0.0f);
	positive      = (m_sign_inverse)? !positive : positive;
	u32 color     = (positive      )? green_clr : red_clr;
	m_value->SetTextColor( color );

	if ( m_texture_minus.size() )
	{
		if ( positive )
		{
			m_caption->InitTexture( m_texture_plus.c_str() );
		}
		else
		{
			m_caption->InitTexture( m_texture_minus.c_str() );
		}
	}

}
