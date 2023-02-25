#pragma once

#include "UIWindow.h"
#include "UIDoubleProgressBar.h"
#include "..\..\xrServerEntities\alife_space.h"

class CCustomOutfit;
class CHelmet;
class CUIStatic;
class CUIDoubleProgressBar;
class CUIXml;

class CUIOutfitImmunity : public CUIWindow
{
public:
					CUIOutfitImmunity	();
	virtual			~CUIOutfitImmunity	();

			void	InitFromXml			( CUIXml& xml_doc, LPCSTR base_str, u32 hit_type );
			void	SetProgressValue	( float cur, float comp );

protected:
	CUIStatic				m_name; // texture + name
	CUIDoubleProgressBar	m_progress;
	CUITextWnd				m_value; // 100%
	float					m_magnitude;

}; // class CUIOutfitImmunity

// -------------------------------------------------------------------------------------

class CUIOutfitInfo : public CUIWindow
{
public:
					CUIOutfitInfo		();
	virtual			~CUIOutfitInfo		();

			void 	InitFromXml			( CUIXml& xml_doc );
			void 	UpdateInfo			( CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit = NULL );	
			void 	UpdateInfo			( CHelmet* cur_helmet, CHelmet* slot_helmet = NULL );
			
protected:
	enum				{ max_count = ALife::eHitTypeMax-4 };
	
//	CUIStatic*			m_caption;
	CUIStatic*			m_Prop_line;
	CUIOutfitImmunity*	m_items[max_count];
	
	// Lex Addon (correct by Suhar_) 5.08.2018		(begin)
	// Расширение алгоритма отображения свойств у костюмов и шлемов
	// Регистрация визуальных элементов
	CUIStatic			m_staticHeadArmor;
	CUITextWnd			m_textHeadArmorName;
	CUITextWnd			m_textHeadArmorValue;
	CUIStatic			m_staticBodyArmor;
	CUITextWnd			m_textBodyArmorName;
	CUITextWnd			m_textBodyArmorValue;
	CUIStatic			m_staticPowerLoss;
	CUITextWnd			m_textPowerLossName;
	CUITextWnd			m_textPowerLossValue;
	CUIStatic			m_staticSprintAllowed;
	CUITextWnd			m_textSprintAllowedValue;
	CUIStatic			m_staticAdditionalWeight;
	CUITextWnd			m_textAdditionalWeightName;
	CUITextWnd			m_textAdditionalWeightValue;
	CUIStatic			m_staticAdditionalWalkSpeed;
	CUITextWnd			m_textAdditionalWalkSpeedName;
	CUITextWnd			m_textAdditionalWalkSpeedValue;
	CUIStatic			m_staticAdditionalJumpSpeed;
	CUITextWnd			m_textAdditionalJumpSpeedName;
	CUITextWnd			m_textAdditionalJumpSpeedValue;
	CUIStatic			m_staticPowerRestoreSpeed;
	CUITextWnd			m_textPowerRestoreSpeedName;
	CUITextWnd			m_textPowerRestoreSpeedValue;
	CUIStatic			m_staticRadiationRestoreSpeed;
	CUITextWnd			m_textRadiationRestoreSpeedName;
	CUITextWnd			m_textRadiationRestoreSpeedValue;
	CUIStatic			m_staticBleedingRestoreSpeed;
	CUITextWnd			m_textBleedingRestoreSpeedName;
	CUITextWnd			m_textBleedingRestoreSpeedValue;
	CUIStatic			m_staticHealthRestoreSpeed;
	CUITextWnd			m_textHealthRestoreSpeedName;
	CUITextWnd			m_textHealthRestoreSpeedValue;
	CUIStatic			m_staticMaxHealthAdd;
	CUITextWnd			m_textMaxHealthAddName;
	CUITextWnd			m_textMaxHealthAddValue;
	CUIStatic			m_staticArtefactCount;
	CUITextWnd			m_textArtefactCountName;
	CUITextWnd			m_textArtefactCountValue;
	CUIStatic			m_staticNightvision;
	CUITextWnd			m_textNightvisionName;
	CUITextWnd			m_textNightvisionValue;
	CUIStatic			m_staticEnemiesScanner;
	CUITextWnd			m_textEnemiesScannerValue;
	CUIStatic			m_staticDozimeter;
	CUITextWnd			m_textDozimeterValue;
	CUIStatic			m_staticPsyController;
	CUITextWnd			m_textPsyControllerValue;
	// Добавочная высота окна
	int					AdditionalHeight;
	// Lex Addon (correct by Suhar_) 5.08.2018		(end)

}; // class CUIOutfitInfo
