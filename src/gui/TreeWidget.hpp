/***************************************************************************
 *  TreeWidget.cpp
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/

#ifndef _TreeWidget_H
#define _TreeWidget_H
 
#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include "TuileWidget.hpp";

class TuileParamGroup;
class TuileWidget;
class SeqWidget;
class AudioTuileWidget;
class LeafTuileWidget;
class ConnectionWidget;

class TreeWidget : public TuileWidget, public Fl_Group {
    public:
        static TreeWidget* getInstance();
        ~TreeWidget();

        void update();
		void notifyUpdate();
        inline void notifyDelete(){}
        void save(const std::string& fileName);
        TuileWidget* load(const std::string& fileName);

        void refreshTuiles();
        void refreshChildrenTuileWidgets();
        void zoom(const float&);

        void draw();
        int handle(int);

        inline void setPixelsPerBeat(int ppb){m_pixelsPerBeat=ppb;}
        inline const float& getPixelsPerFrame(){return m_pixelsPerFrame;}

        virtual bool testMagnetWithTuile(const int& inX, const int& inY,
                                            int& outX, int& outY, 
                                            const std::string& tuileName,
                                            const bool& drop);
        TuileWidget* createTuileWidget(const std::string& tuileName);
        SeqWidget* createSeqWidget(TuileWidget*, TuileWidget*);
        void addTuileWidget(TuileWidget*);
        void removeConnectionsWithWidget(AudioTuileWidget*);
        void markWidgetForRemoval(TuileWidget*);
        TuileWidget* getTuileWidget(const unsigned int& id);

        void selectTuileWidget(TuileWidget* selected);
        void deselectAllTuileWidgets();
        inline void setParamGroup(TuileParamGroup* paramGroup){ 
            m_paramGroup=paramGroup;
        }

        void markConnectionForRemoval(ConnectionWidget*);
        void testConnection(AudioTuileWidget*, const int& x, 
                            const int& y, bool drop=false);

        void clear();

    private:
        TreeWidget();

    private:
        int m_cursorX;
        float m_pixelsPerBeat;
        float m_pixelsPerFrame;
        int m_offsetX, m_offsetY;
        int m_magnetSize;
        int m_zeroPosX;

        std::list<AudioTuileWidget*> m_audioTuileWidgets;
        std::map<unsigned int, TuileWidget*> m_tuileWidgetMap;
        TuileWidget* m_selectedTuile;
        std::vector<TuileWidget*> m_removingWidgets;

        unsigned int m_connectionIDCounter;
        std::vector<ConnectionWidget*> m_connections;
        ConnectionWidget* m_removingConnection;

        TuileParamGroup* m_paramGroup;
};

#endif
