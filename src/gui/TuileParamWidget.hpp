/***************************************************************************
 *  TuileParamWidget.hpp
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/

#ifndef _TuileParamWidget_H
#define _TuileParamWidget_H
 
#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Valuator.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Choice.H>

#include <tuiles/Observer.hpp>

#include "HitPack.hpp"

class TuileWidget;
namespace tuiles{class Tuile;}

class TuileParamWidget : public Fl_Group, public tuiles::Observer {
  public:
	TuileParamWidget(TuileWidget*, tuiles::Tuile*);
	virtual ~TuileParamWidget();

    inline virtual void init(){}
    virtual void update();

    virtual void notifyUpdate();
    inline virtual void notifyDelete(){}

    void updateTuileProps();

    static void statTuileInputs(Fl_Widget* w, void* f){ 
        TuileParamWidget *tpw = static_cast<TuileParamWidget *>(f); 
        tpw->cbTuileInputs(w);
    }	
    void cbTuileInputs(Fl_Widget*);

  protected:
    tuiles::Tuile* m_tuile;
    TuileWidget* m_tuileWidget;

    HitPack* m_pack;
    HitPack* m_tuilePack;
    Fl_Input* m_nameInput;
    Fl_Value_Input* m_lengthInput;
    Fl_Value_Input* m_lOffsetInput;
    Fl_Value_Input* m_rOffsetInput;
    Fl_Button* m_removeButton;

    std::list<Fl_Slider*> m_sliders;
    std::list<Fl_Button*> m_buttons;
    std::list<Fl_Value_Input*> m_inputs;
};

#endif
