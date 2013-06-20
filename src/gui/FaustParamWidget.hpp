/***************************************************************************
 *            FaustParamWidget.h
 *
 *  2012 Florent Berthaut
 *  ANR INEDIT Project
 *
 *  This file is part of libTuiles
 ****************************************************************************/
 
 
#ifndef _FaustParamWidget_H
#define _FaustParamWidget_H

#include <map>

#include <faust/llvm-dsp.h>
#include <faust/gui/UI.h>

#include "TuileParamWidget.hpp"

class FaustParamWidget : public TuileParamWidget, public UI {
  public:
	FaustParamWidget(int ,int , int, int, const unsigned int&, 
                                            const std::string&);
	~FaustParamWidget();

    void init();

    static void statParamVal(Fl_Widget* w,void* f) { 
        FaustParamWidget *fpw = static_cast<FaustParamWidget *>(f);
        fpw->cbParamVal(static_cast<Fl_Valuator*>(w));
    }	
    void cbParamVal(Fl_Valuator*);
    static void statParamBut(Fl_Widget* w,void* f) { 
        FaustParamWidget *fpw = static_cast<FaustParamWidget *>(f);
        fpw->cbParamBut(static_cast<Fl_Button*>(w));
    }	
    void cbParamBut(Fl_Button*);

    static void statConnectChoice(Fl_Widget* w,void* f) { 
        FaustParamWidget *fpw = static_cast<FaustParamWidget *>(f);
        fpw->cbConnectChoice(static_cast<Fl_Button*>(w));
    }	
    void cbConnectChoice(Fl_Widget*);

    inline void addButton(const char* label, FAUSTFLOAT* zone) {
        m_paramsMap[std::string(label)]=zone;
        m_pack->begin();
        m_buttons.push_back(new Fl_Button(0,0,fl_width(label),25,label));
        m_buttons.back()->user_data((void*)zone);
        m_buttons.back()->callback(statParamBut,this);
        m_pack->end();
    }

    inline void addCheckButton(const char* label, FAUSTFLOAT* zone) {
        m_paramsMap[std::string(label)]=zone;
        m_pack->begin();
        m_buttons.push_back(new Fl_Toggle_Button(0,0,fl_width(label),25,label));
        m_buttons.back()->user_data(zone);
        m_buttons.back()->callback(statParamBut,this);
        m_pack->end();
    }

    inline void addVerticalSlider(const char* label, FAUSTFLOAT* zone, 
                                    FAUSTFLOAT init, FAUSTFLOAT min, 
                                    FAUSTFLOAT max, FAUSTFLOAT step) {
        m_paramsMap[std::string(label)]=zone;
        m_pack->begin();
        m_sliders.push_back(new Fl_Slider(100,0,100,20,label));
        m_sliders.back()->user_data((void*)zone);
        m_sliders.back()->callback(statParamVal,this);
        m_sliders.back()->bounds(min,max);
        m_sliders.back()->value(init);
        m_sliders.back()->step(step);
        m_sliders.back()->labelsize(12);
        m_sliders.back()->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);
        m_sliders.back()->type(FL_HOR_NICE_SLIDER);
        m_pack->end();
    }

    inline void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, 
                                    FAUSTFLOAT init, FAUSTFLOAT min, 
                                    FAUSTFLOAT max, FAUSTFLOAT step) {
        m_paramsMap[std::string(label)]=zone;
        m_pack->begin();
        m_sliders.push_back(new Fl_Slider(100,0,100,20,label));
        m_sliders.back()->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);
        m_sliders.back()->bounds(min,max);
        m_sliders.back()->value(init);
        m_sliders.back()->step(step);
        m_sliders.back()->user_data((void*)zone);
        m_sliders.back()->callback(statParamVal,this);
        m_sliders.back()->type(FL_HOR_NICE_SLIDER);
        m_sliders.back()->labelsize(12);
        m_pack->end();
    }
    inline void addNumEntry(const char* label, FAUSTFLOAT* zone, 
                            FAUSTFLOAT init, FAUSTFLOAT min, 
                            FAUSTFLOAT max, FAUSTFLOAT step) {
        m_paramsMap[std::string(label)]=zone;
        m_pack->begin();
        m_inputs.push_back(new Fl_Value_Input(100,0,30,20,label));
        m_inputs.back()->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);
        m_inputs.back()->bounds(min,max);
        m_inputs.back()->value(init);
        m_inputs.back()->step(step);
        m_inputs.back()->user_data((void*)zone);
        m_inputs.back()->callback(statParamVal,this);
        m_inputs.back()->labelsize(12);
        m_pack->end();
    }
    inline void openTabBox(const char* label){};
    inline void openHorizontalBox(const char* label){};
    inline void openVerticalBox(const char* label){};
    inline void closeBox(){};
    inline void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, 
                                        FAUSTFLOAT min, FAUSTFLOAT max){};
    inline void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, 
                                        FAUSTFLOAT min, FAUSTFLOAT max){};

    virtual void setInputList(const std::vector<std::string>&, 
                                const std::vector<int>&);

  protected:

    std::map<std::string, float*> m_paramsMap;

    Fl_Choice *m_inputChoice, *m_outputChoice;
    Fl_Pack *m_connectPack;
    std::vector<int> m_inputIDs;

};

#endif
