#include "pch_script.h"
#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "../level.h"
#include "game_base_space.h"
#include "../ai_space.h"
#include "../../xrServerEntities/script_engine.h"

struct SLuaWpnParams{
	luabind::functor<float>		m_functorRPM;
	luabind::functor<float>		m_functorAccuracy;
	luabind::functor<float>		m_functorDamage;
	luabind::functor<float>		m_functorDamageMP;
	luabind::functor<float>		m_functorHandling;
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Инициализация скриптовых функций
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
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamageMP",		m_functorDamageMP);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetHandling",		m_functorHandling);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetAccuracy",		m_functorAccuracy);		VERIFY(functor_exists);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Чтобы подсчитать значение необходимых параметров, обратимся к скриптам (обязательно создать эти функции в скриптах!)
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

CUIWpnParams::CUIWpnParams(){
	AttachChild(&m_textAccuracy);
	AttachChild(&m_textDamage);
	AttachChild(&m_textHandling);
	AttachChild(&m_textRPM);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Инициализация заголовков
	AttachChild(&m_textBulletSpeed);
	AttachChild(&m_textRecoil);

	AttachChild(&m_progressAccuracy);
	AttachChild(&m_progressDamage);
	AttachChild(&m_progressHandling);
	AttachChild(&m_progressRPM);
	// Инициализация ползунков
	AttachChild(&m_progressBulletSpeed);
	AttachChild(&m_progressRecoil);
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)
}

CUIWpnParams::~CUIWpnParams()
{
}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc){
	if (!xml_doc.NavigateToNode("wpn_params", 0))	return;
	CUIXmlInit::InitWindow				(xml_doc, "wpn_params", 0, this);

	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:cap_accuracy",			0, &m_textAccuracy);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:cap_damage",				0, &m_textDamage);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:cap_handling",			0, &m_textHandling);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:cap_rpm",					0, &m_textRPM);
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Читаем необходимые параметры из конфигов
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:cap_bullet_speed",		0, &m_textBulletSpeed);
	CUIXmlInit::InitStatic				(xml_doc, "wpn_params:cap_recoil",				0, &m_textRecoil);

	CUIXmlInit::InitProgressBar			(xml_doc, "wpn_params:progress_accuracy",		0, &m_progressAccuracy);
	CUIXmlInit::InitProgressBar			(xml_doc, "wpn_params:progress_damage",			0, &m_progressDamage);
	CUIXmlInit::InitProgressBar			(xml_doc, "wpn_params:progress_handling",		0, &m_progressHandling);
	CUIXmlInit::InitProgressBar			(xml_doc, "wpn_params:progress_rpm",			0, &m_progressRPM);
	CUIXmlInit::InitProgressBar			(xml_doc, "wpn_params:progress_bullet_speed",	0, &m_progressBulletSpeed);
	CUIXmlInit::InitProgressBar			(xml_doc, "wpn_params:progress_recoil",			0, &m_progressRecoil);

	m_progressAccuracy.SetRange			(0, 100);
	m_progressDamage.SetRange			(0, 100);
	m_progressHandling.SetRange			(0, 100);
	m_progressRPM.SetRange				(0, 100);
	m_progressBulletSpeed.SetRange		(0, 100);
	m_progressRecoil.SetRange			(0, 100);
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)
}

void CUIWpnParams::SetInfo(const shared_str& wpn_section)
{

	if(!g_lua_wpn_params)
		g_lua_wpn_params = xr_new<SLuaWpnParams>();

	m_progressRPM.SetProgressPos			(g_lua_wpn_params->m_functorRPM(*wpn_section));
	m_progressAccuracy.SetProgressPos		(g_lua_wpn_params->m_functorAccuracy(*wpn_section));
	// Lex Addon (correct by Suhar_) 7.08.2018		(begin)
	// Дополнительные свойства в описании оружия (настильность, отдача) 
	// Задаем ползункам значения из скриптовых функций
	m_progressBulletSpeed.SetProgressPos	(g_lua_wpn_params->m_functorBulletSpeed(*wpn_section));
	m_progressRecoil.SetProgressPos			(g_lua_wpn_params->m_functorRecoil(*wpn_section));
	// Lex Addon (correct by Suhar_) 7.08.2018		(end)
	if (GameID() == GAME_SINGLE)
        m_progressDamage.SetProgressPos		(g_lua_wpn_params->m_functorDamage(*wpn_section));
	else
		m_progressDamage.SetProgressPos		(g_lua_wpn_params->m_functorDamageMP(*wpn_section));
	m_progressHandling.SetProgressPos		(g_lua_wpn_params->m_functorHandling(*wpn_section));
}

bool CUIWpnParams::Check(const shared_str& wpn_section){
	if (pSettings->line_exist(wpn_section, "fire_dispersion_base"))
	{
        if (0==xr_strcmp(wpn_section, "wpn_addon_silencer"))
            return false;
        if (0==xr_strcmp(wpn_section, "wpn_binoc"))
            return false;
        if (0==xr_strcmp(wpn_section, "mp_wpn_binoc"))
            return false;

        return true;		
	}
	else
		return false;
}