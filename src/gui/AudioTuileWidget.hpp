/***************************************************************************
 *  AudioTuileWidget.h
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/
 
 
#ifndef _AudioTuileWidget_H
#define _AudioTuileWidget_H

#include "LeafTuileWidget.hpp"

class AudioTuile;

class AudioTuileWidget: public LeafTuileWidget {
  public:
	AudioTuileWidget(const std::string& name, AudioTuile*);
	virtual ~AudioTuileWidget();

    virtual int handle(int event);
    virtual void drawConnections();

    bool canTakeInput(){return m_canTakeInput;}
    virtual void connectToWidget(AudioTuileWidget*);
    inline AudioTuile* getAudioTuile(){return m_audioTuile;}
    
  protected:
    AudioTuile* m_audioTuile;
    bool m_canTakeInput;
    bool m_connecting;
    
};

#endif

