#pragma once
#include "UIWindow.h"
#include "..\..\xrServerEntities\alife_space.h"


class CUIXml;
class CUIStatic;
class CUITextWnd;
class UIArtefactParamItem;

class CUIArtefactParams : public CUIWindow
{
public:
					CUIArtefactParams		();
	virtual			~CUIArtefactParams		();
			void	InitFromXml				(CUIXml& xml);
			bool	Check					(const shared_str& af_section);
			void	SetInfo					(const shared_str& af_section);

protected:
	UIArtefactParamItem*	m_immunity_item[ALife::infl_max_count];
	UIArtefactParamItem*	m_restore_item[ALife::eRestoreTypeMax];
	UIArtefactParamItem*	m_additional_weight;
	// Lex Addon (correct by Suhar_) 				(begin)
	// Переменная дополнительного максимального здоровья 4.07.2016
	UIArtefactParamItem*	m_fMaxHealthAdd;
	// Переменная дополнительной скорости прыжка 6.07.2016
	UIArtefactParamItem*	m_fJumpSpeedAdd;
	// Переменная дополнительной скорости передвижения
	UIArtefactParamItem*	m_fWalkAccelAdd;
	// Переменная гашения удара 7.07.2016
	UIArtefactParamItem*	eHitTypeWound;
	// Lex Addon (correct by Suhar_)				(end)

	CUIStatic*				m_Prop_line;

}; // class CUIArtefactParams

// -----------------------------------

class UIArtefactParamItem : public CUIWindow
{
public:
				UIArtefactParamItem	();
	virtual		~UIArtefactParamItem();
		
		void	Init				( CUIXml& xml, LPCSTR section );
		void	SetCaption			( LPCSTR name );
		void	SetValue			( float value );
	
private:
	CUIStatic*	m_caption;
	CUITextWnd*	m_value;
	float		m_magnitude;
	bool		m_sign_inverse;
	// Lex Addon (correct by Suhar_) 10.03.2017		(begin)
	// Добавлена возможность отображения дробных чисел в свойствах артефактов
	int			m_accuracy;
	// Lex Addon (correct by Suhar_) 10.03.2017		(end)
	shared_str	m_unit_str;
	shared_str	m_texture_minus;
	shared_str	m_texture_plus;

}; // class UIArtefactParamItem
