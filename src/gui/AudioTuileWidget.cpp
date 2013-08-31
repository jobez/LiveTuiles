/***************************************************************************
 *  AudioTuileWidget.cpp
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/

#include "AudioTuileWidget.hpp"

#include <iostream>
#include <math.h>
#include <cassert>

#include <Fl/fl_draw.H>

#include "../audio/AudioTuile.hpp"
#include "TreeWidget.hpp"

using namespace std;

AudioTuileWidget::AudioTuileWidget(const std::string& name, 
                                    AudioTuile* tuile):  
                                            LeafTuileWidget(name, tuile), 
                                            m_audioTuile(tuile), 
                                            m_canTakeInput(true),
                                            m_connecting(false) {
}

AudioTuileWidget::~AudioTuileWidget() {}

void AudioTuileWidget::load() {}

void AudioTuileWidget::notifyDelete() {
    TreeWidget::getInstance()->removeConnectionsWithWidget(this);
    LeafTuileWidget::notifyDelete();
}

void AudioTuileWidget::drawComposition() {
    LeafTuileWidget::drawComposition();
    fl_color(FL_BLUE);
    if(m_connecting) {
        fl_begin_line();
        fl_curve(getCenterReal(), y()+h(),
                    getCenterReal(), y()+h()*2,
                    m_dragPosX, m_dragPosY-h(),
                    m_dragPosX, m_dragPosY);
        fl_end_line();
    }
}

int AudioTuileWidget::handle(int event) {
    switch(event) { 
        case FL_MOVE: { 
            if(Fl::event_state(FL_CTRL|FL_COMMAND) 
                && Fl::event_x()>x()+m_real1X && Fl::event_x()<x()+m_real2X) {
                return 1;
            }
            else {
                return LeafTuileWidget::handle(event);
            }
        }break;
        case FL_DRAG: {
            if(m_connecting) {
                TreeWidget::getInstance()->testConnection(  this, 
                                                            Fl::event_x(),
                                                            Fl::event_y());
                m_dragPosX=Fl::event_x();
                m_dragPosY=Fl::event_y();
                return 1;
            }
            return LeafTuileWidget::handle(event);
        }break;
        case FL_PUSH: {
            if(Fl::event_state(FL_CTRL|FL_COMMAND)) {
                m_connecting=true;
            }
            else {
                m_connecting=false;
            }
            return LeafTuileWidget::handle(event);
        }break;
        case FL_RELEASE: {
            if(m_connecting) {
                TreeWidget::getInstance()->testConnection(this, 
                                                            Fl::event_x(),
                                                            Fl::event_y(), 
                                                            true);
            }
            m_connecting=false;
        }break;
        default:break;
    }
    return LeafTuileWidget::handle(event);
}
