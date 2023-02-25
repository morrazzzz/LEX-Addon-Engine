#pragma once
#include "UIWindow.h"
#include "../EntityCondition.h"

class CUIXml;
class CUIStatic;
class CUITextWnd;
class UIBoosterInfoItem;

class CUIBoosterInfo : public CUIWindow
{
public:
					CUIBoosterInfo		();
	virtual			~CUIBoosterInfo		();
			void	InitFromXml				(CUIXml& xml);
			void	SetInfo					(const shared_str& section);

protected:
	UIBoosterInfoItem*	m_booster_items[eBoostExplImmunity];
	// Lex Addon (correct by Suhar_) 8.07.2016		(begin)
	// Физическая имунность
	UIBoosterInfoItem*	m_booster_wound_immunity;
	// Lex Addon (correct by Suhar_) 8.07.2016		(end)
	UIBoosterInfoItem*	m_booster_satiety;
	UIBoosterInfoItem*	m_booster_anabiotic;
	UIBoosterInfoItem*	m_booster_time;
	// Lex Addon (correct by Suhar_) 7.07.2016		(begin)
	// Восстановление здоровья (мгновенное)
	UIBoosterInfoItem*	m_booster_health;
	// Заживление ран (мгновенное)
	UIBoosterInfoItem*	m_booster_wounds_heal_perc;
	// Вывод радиации (мгновенный)
	UIBoosterInfoItem*	m_booster_radiation;
	// Восстановление сил (мгновенное)
	UIBoosterInfoItem*	m_booster_power;
	// Восстановление пси-здоровья (мгновенное)
	UIBoosterInfoItem*	m_booster_psy_health;
	// Восстановление пси-здоровья (длительное)
	UIBoosterInfoItem*	m_booster_psy_health_restore;
	// Изменение фактора сна
	UIBoosterInfoItem*	m_booster_sleep_factor_add;
	// Опьянение
	UIBoosterInfoItem*	m_booster_alcohol;
	// Lex Addon (correct by Suhar_) 7.07.2016		(end)

	CUIStatic*			m_Prop_line;

}; // class CUIBoosterInfo

// -----------------------------------

class UIBoosterInfoItem : public CUIWindow
{
public:
				UIBoosterInfoItem	();
	virtual		~UIBoosterInfoItem();
		
		void	Init				( CUIXml& xml, LPCSTR section );
		void	SetCaption			( LPCSTR name );
		void	SetValue			( float value );
	
private:
	CUIStatic*	m_caption;
	CUITextWnd*	m_value;
	float		m_magnitude;
	bool		m_show_sign;
	// Lex Addon (correct by Suhar_) 11.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах бустеров
	int			m_accuracy;
	// Lex Addon (correct by Suhar_) 11.03.2017		(end)
	shared_str	m_unit_str;
	shared_str	m_texture_minus;
	shared_str	m_texture_plus;

}; // class UIBoosterInfoItem
