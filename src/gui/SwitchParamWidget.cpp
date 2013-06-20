/***************************************************************************
 *            SwitchParamWidget.cpp
 *
 *  2012 Florent Berthaut
 *  ANR INEDIT Project
 *
 *  This file is part of libTuiles
 ****************************************************************************/

#include <iostream>
#include <math.h>
#include <cassert>

#include "SwitchParamWidget.hpp"

using namespace std;


SwitchParamWidget::SwitchParamWidget(int x, int y, int w, int h,
								const unsigned int& id, const string& name):
                                TuileParamWidget(x, y, w, h, id, name) {

    m_selectInput = new Fl_Simple_Counter(0, 0, 50, 20, "Selected Child");
    m_selectInput->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
    m_selectInput->callback(statTuileSwitchInputs,this);
    m_selectInput->labelsize(12);
    m_selectInput->step(1);
    m_selectInput->bounds(0,10);
    Fl_Group *tmpGroup = new Fl_Group(0,0,80,20,"");
    tmpGroup->end();
    tmpGroup->add(m_selectInput);
    tmpGroup->resizable(false);
    m_pack->add(tmpGroup);
    end();
}

SwitchParamWidget::~SwitchParamWidget() {}

void SwitchParamWidget::update() {

}

void SwitchParamWidget::cbTuileSwitchInputs(Fl_Widget* w) {
/*
    TuilesManager* tMan = TuilesManager::getInstance();
    tMan->setSwitchTuileSelection(m_id, m_selectInput->value());
*/
}

