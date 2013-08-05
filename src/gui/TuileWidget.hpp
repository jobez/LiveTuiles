/***************************************************************************
 *  TuileWidget.h
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/
 
#ifndef _TuileWidget_H
#define _TuileWidget_H

#include <vector>
#include <string>
#include <FL/Fl_Widget.H>
#include <tuiles/Observer.hpp>

class TuileParamWidget;
namespace tuiles{class Tuile;}

class TuileWidget: public tuiles::Observer {
  public:
	TuileWidget(tuiles::Tuile*);
	virtual ~TuileWidget();

    inline const int& getID(){return m_id;}
    inline tuiles::Tuile* getTuile(){return m_tuile;}

    virtual void notify();

    inline virtual void drawComposition(){}
    inline virtual void drawExecution(const float& alpha=1){}
    inline virtual void refreshChildrenTuileWidgets(){}

    inline virtual int getSyncIn() {return m_sync1X;}
    inline virtual int getSyncOut() {return m_sync2X;}
    inline virtual int getRealIn() {return m_real1X;}
    inline virtual int getRealOut(){return m_real2X;}
    inline virtual int getCenterReal(){return 0;}
    inline void setSync1Y(const int& y){m_sync1Y=y;}
    inline void setSync2Y(const int& y){m_sync2Y=y;}
    inline virtual void resetHighlight(){}
    inline virtual void highlightSyncInLine(){}
    inline virtual void highlightSyncOutLine(){}
    inline virtual void highlightReal(bool high=true){}

    inline virtual void tryForkWithTuile(const std::string& tuileName){}
    inline virtual void trySeqWithTuile(const std::string& tuileName){}
    inline virtual void tryJoinWithTuile(const std::string& tuileName){}
    inline virtual void tryLeftSeqWithTuile(const std::string& tuileName){}
    inline virtual void tryAddTuileChild(const std::string& tuileName){}

    inline virtual Fl_Widget* getWidget(){return NULL;}

  protected:
    int m_id;
    std::vector<TuileWidget*> m_childrenTuileWidgets;
    tuiles::Tuile* m_tuile;

    bool m_muted;

	float m_height;
    float m_width;

	int m_sync1X;
    int m_sync1Y;
	int m_sync2X;
    int m_sync2Y;
    int m_syncWidth;
    int m_syncHeight;
	int m_real1X;
	int m_real2X;
    Fl_Color m_sync1Color;
    Fl_Color m_sync2Color;
    Fl_Color m_syncLColor;
    Fl_Color m_rectColor;
    Fl_Color m_realColor;

	bool m_overSyncIn;
	bool m_overSyncOut;
	bool m_draggingSyncIn;
	bool m_draggingSyncOut;
    bool m_dragging;

	int m_dragPosX;
	int m_dragPosY;
	int m_magnetSize;
	int m_measureDiv;
    bool m_connecting;

	unsigned int m_state;
	Fl_Color m_backgroundColor;
    bool m_selected;

    TuileParamWidget* m_paramWidget; 
};

#endif

