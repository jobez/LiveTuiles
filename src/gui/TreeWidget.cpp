/***************************************************************************
 *  TreeWidget.hpp
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/

#include "TreeWidget.hpp"

#include <iostream>
#include <math.h>
#include <cassert>

#include "../audio/AudioManager.hpp"
#include <tuiles/SeqTuile.hpp>
#include <tuiles/SwitchTuile.hpp>
#include <tuiles/MonitorTuile.hpp>
#include <tuiles/LoopTuile.hpp>

#include "MainWindow.hpp"
#include "TuileWidget.hpp"
#include "SoundFileWidget.hpp"
#include "FaustWidget.hpp"
#include "AudioInputWidget.hpp"
#include "AudioOutputWidget.hpp"
#include "SeqWidget.hpp"
#include "LoopWidget.hpp"
#include "MonitorWidget.hpp"
#include "SwitchWidget.hpp"
#include "ConnectionWidget.hpp"

#include "TuileParamGroup.hpp"

using namespace std;
using namespace tuiles;

TreeWidget::TreeWidget():   TuileWidget(AudioManager::getInstance()),
                            Fl_Group(0, 0, 100, 100, ""),
                            m_cursorX(0), 
                            m_minPixPerFrame(1),
                            m_offsetX(10), m_offsetY(10), m_magnetSize(5),
                            m_zeroPosX(0), 
                            m_connectionIDCounter(0) {
	end();
    m_id=-1;
    type(Fl_Scroll::BOTH);
    clip_children(1);
    resizable(NULL);
    zoom(0);
}

TreeWidget::~TreeWidget() {}

TreeWidget* TreeWidget::getInstance() {
    static TreeWidget instance;
    return &instance;
}

void TreeWidget::update() {
    //update cursor
    m_cursorX = (AudioManager::getInstance()->getPlayingPos()) 
                    *m_pixelsPerFrame;
    //remove connections
    if(m_removingConnections.size()>0) {
        vector<ConnectionWidget*>::iterator itConWid= 
                                                m_removingConnections.begin();
        for(; itConWid!=m_removingConnections.end(); ++itConWid) {
            m_connections.erase((*itConWid)->getID());
            delete (*itConWid);
        }
        m_removingConnections.clear();
        MainWindow::getInstance()->redraw();
    }
    //remove widgets
    if(m_removingWidgets.size()>0) {
        vector<TuileWidget*>::iterator itWid=m_removingWidgets.begin();
        for(; itWid!=m_removingWidgets.end(); itWid++) {
            m_tuileWidgets.erase((*itWid)->getID());
            m_audioTuileWidgets.erase((*itWid)->getID());
        }
        refreshChildrenTuileWidgets();
        itWid=m_removingWidgets.begin();
        for(; itWid!=m_removingWidgets.end(); itWid++) {
            delete (*itWid);
        }
        m_paramGroup->clear();
        MainWindow::getInstance()->redraw();
        m_removingWidgets.clear();
    }
    //redraw if playing
    if(AudioManager::getInstance()->isPlaying()) {
        redraw();
    }
}

void TreeWidget::zoom(const float& zoom) {
    m_pixelsPerFrame=(1-m_minPixPerFrame)*zoom+m_minPixPerFrame;
    notifyUpdate();
}

void TreeWidget::draw() {
    Fl_Group::draw();

	//box
	fl_draw_box(FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR);
	fl_push_clip(x()+2, y()+2, w()-4, h()-4);

	//score beginning
	fl_rectf(x()+m_zeroPosX, y(), w()-m_zeroPosX, h(), FL_BACKGROUND2_COLOR);

    //connections between tuiles 
    map<unsigned int, ConnectionWidget*>::iterator itCon=m_connections.begin();
    for(; itCon!=m_connections.end(); ++itCon) {
        itCon->second->drawConnection();
    }

	//draw background execution tuiles
    vector<TuileWidget*>::iterator itWidgetNode=
                                            m_childrenTuileWidgets.begin();
    for(; itWidgetNode!=m_childrenTuileWidgets.end(); ++itWidgetNode) {
        (*itWidgetNode)->drawExecution();
    }

    //then draw composition tuiles
    itWidgetNode=m_childrenTuileWidgets.begin();
    for(; itWidgetNode!=m_childrenTuileWidgets.end(); ++itWidgetNode) {
        (*itWidgetNode)->drawComposition();
    }

    //cursor
	fl_color(FL_RED);
    fl_line(x()+m_cursorX+(m_zeroPosX), y(), 
            x()+m_cursorX+(m_zeroPosX), y()+h());
	fl_pop_clip();
}

void TreeWidget::refreshChildrenTuileWidgets() {
    m_childrenTuileWidgets.clear();
    //get new children tuile widgets
    AudioManager* man = AudioManager::getInstance();
    vector<Tuile*>::const_iterator itChild=man->getChildren().begin();
    for(; itChild!=man->getChildren().end(); ++itChild) {
        if(m_tuileWidgets.find((*itChild)->getID())!=m_tuileWidgets.end()){
            m_childrenTuileWidgets.push_back(
                                        m_tuileWidgets[(*itChild)->getID()]);
        }
    }
    //update the children of all the tuilewidgets
    map<unsigned int, TuileWidget*>::iterator itWid=m_tuileWidgets.begin();
    for(; itWid!=m_tuileWidgets.end(); ++itWid) {
        itWid->second->refreshChildrenTuileWidgets();
    }
    //print the trees
    man->printTrees();
    //refresh tuiles
    notifyUpdate();
}

void TreeWidget::notifyUpdate() {
    //notify all tuiles
    map<unsigned int, TuileWidget*>::iterator itWidget=m_tuileWidgets.begin();
    for(;itWidget!=m_tuileWidgets.end(); ++itWidget) {
        itWidget->second->notifyUpdate();
    }
    //update tuiles positions and get total length
    float totalLength=0;
    m_zeroPosX = m_tuile->getLeftOffset()*m_pixelsPerFrame;
    for(unsigned int i=0; i<m_childrenTuileWidgets.size(); ++i) {
        Tuile* childTuile = m_childrenTuileWidgets[i]->getTuile();
        m_childrenTuileWidgets[i]->getWidget()->position(
            (m_tuile->getLeftOffset()-childTuile->getLeftOffset())
                *m_pixelsPerFrame+x(), 
            m_childrenTuileWidgets[i]->getWidget()->y());
        float childMax = max<float>(-childTuile->getLeftOffset(), 0) +
                         max<float>(-childTuile->getRightOffset(), 0) +
                         childTuile->getLength();
        if(childMax > totalLength) {
           totalLength = childMax;
        }
    }
    m_minPixPerFrame=float(w())/(totalLength*10.0);
    //update all connections
    map<unsigned int, ConnectionWidget*>::iterator itCon=m_connections.begin();
    for(; itCon!=m_connections.end(); ++itCon) {
        itCon->second->update();
    }
    redraw();
}

int TreeWidget::handle(int event) {
    bool handled=false;
    map<unsigned int, ConnectionWidget*>::iterator itCon=m_connections.begin();
    for(; itCon!=m_connections.end() && !handled; ++itCon) {
        handled=itCon->second->handle(event);
    }
    if(handled) {
        return 1;
    }
    vector<TuileWidget*>::iterator itChWid=m_childrenTuileWidgets.begin();
    for(; itChWid!=m_childrenTuileWidgets.end() && !handled; ++itChWid) {
        handled=(*itChWid)->getWidget()->handle(event);
    }
    if(handled) {
        return 1;
    }
    switch(event) { 
        case FL_MOUSEWHEEL: {
            if(Fl::event_state(FL_CTRL|FL_COMMAND)) {
                //TODO zoom in/out the tree
                return 1;
            }
            else {
                redraw();
                return Fl_Group::handle(event);
            }
        }
        break;
        case FL_KEYDOWN: {
            switch(Fl::event_key()) {
                case FL_BackSpace: {
                    if(m_selectedTuile) {
                        m_selectedTuile->extract();
                        m_selectedTuile=NULL;
                    }
                    return 1;
                }break;
                case 32: {
                    if(Fl::event_state(FL_SHIFT)) {
                        MainWindow::getInstance()->togglePlayStop();
                    }
                    else {
                        MainWindow::getInstance()->togglePlayPause();
                    }
                    return 1;
                }break;
                case FL_Meta_L:
                case FL_Meta_R:
                case FL_Control_L: 
                case FL_Control_R: {
                    fl_cursor(FL_CURSOR_CROSS);
                    return 1;
                }break;;
                default:break;
            }
        }break;
        case FL_KEYUP: {
            switch(Fl::event_key()) {
                case FL_Meta_L:
                case FL_Meta_R:
                case FL_Control_L: 
                case FL_Control_R: {
                    fl_cursor(FL_CURSOR_DEFAULT);
                    return 1;
                }break;;
            };
        }break;
        case FL_ENTER: {
            Fl::focus(this);
            return 1;
        }break;
        case FL_LEAVE: 
        case FL_FOCUS: 
        case FL_UNFOCUS: {
            return 1;
        }break;
        default:break;
    }
    return 0;
}

void TreeWidget::clear() { 
    selectTuileWidget(NULL);
    map<unsigned int, ConnectionWidget*>::iterator itCon=m_connections.begin();
    for(; itCon!=m_connections.end(); ++itCon) {
        delete itCon->second;
    }
    m_connections.clear();
    m_removingConnections.clear();
    m_connectionIDCounter=0;
    vector<TuileWidget*>::iterator itWid=m_childrenTuileWidgets.begin();
    for(; itWid!=m_childrenTuileWidgets.end(); ++itWid) {
        remove((*itWid)->getWidget());
    }
    m_childrenTuileWidgets.clear();
	map<unsigned int, TuileWidget*>::iterator itWidget=m_tuileWidgets.begin();
	for(;itWidget!=m_tuileWidgets.end(); ++itWidget) {
        delete itWidget->second;
    }
    m_tuileWidgets.clear();
    m_audioTuileWidgets.clear();
    m_removingWidgets.clear();
}

void TreeWidget::selectTuileWidget(TuileWidget* selected) {
    m_selectedTuile=selected;
}

void TreeWidget::deselectAllTuileWidgets() {
    m_selectedTuile=NULL;
    m_paramGroup->setWidget(NULL);
	map<unsigned int, TuileWidget*>::iterator itWidget=m_tuileWidgets.begin();
	for(;itWidget!=m_tuileWidgets.end(); ++itWidget) {
        itWidget->second->deselect();
    }
}

bool TreeWidget::testMagnetWithTuile(const int& inX, const int& inY,
                                    int& outX, int& outY, 
                                    const std::string& tuileName,
                                    const bool& drop) {
    bool magnetized=false;
    outX=inX;
    outY=inY;
    vector<TuileWidget*>::iterator itChWid=m_childrenTuileWidgets.begin();
    for(; itChWid!=m_childrenTuileWidgets.end() && !magnetized; ++itChWid) {
        if((*itChWid)->testMagnetWithTuile(inX, inY, outX, 
                                            outY, tuileName, drop)) {
            magnetized=true;
        }
    }

	//drop without link to other tuiles
    if(inX>x() && inY<x()+w() && inY>y() && inY<y()+h()) {
        if(drop && !magnetized) {
            TuileWidget* newWidget = createTuileWidget(tuileName);
            if(newWidget) {
                newWidget->getTuile()->setLeftOffset(-float(inX)
                                                        /m_pixelsPerFrame);
                newWidget->getWidget()->position(newWidget->getWidget()->x(), 
                                                inY);
                newWidget->getTuile()->setName(tuileName);
                newWidget->load();
            }
        }
    }
    if(drop) {
        refreshChildrenTuileWidgets();
    }

    return false;
}

void TreeWidget::testConnection(AudioTuileWidget* tuile, 
                                        const int& x, 
                                            const int& y, 
                                                bool drop) {
    bool found=false;
	map<unsigned int, AudioTuileWidget*>::iterator itWidget=
                                                    m_audioTuileWidgets.begin();
	for(;itWidget!=m_audioTuileWidgets.end() && !found; ++itWidget) {
        AudioTuileWidget* audioWid=itWidget->second;
        audioWid->resetHighlight(); 
        if(y>audioWid->y() - audioWid->h()/2 
                && y<audioWid->y() + 3*audioWid->h()/2) {
            if(x>audioWid->getRealIn() && x<audioWid->getRealOut()) {
                if(audioWid->canTakeInput()) {
                    if(drop) {
                        DEBUG("Connecting tuile "<<tuile->getID()
                                <<" to "<<audioWid->getID());
                        audioWid->highlightReal(false);
                        m_connections[m_connectionIDCounter]=
                            new ConnectionWidget(m_connectionIDCounter,
                                                    tuile, 
                                                        audioWid);
                        m_connectionIDCounter++;
                        notifyUpdate();
                    }
                    else {
                        audioWid->highlightReal();
                    }
                    found=true;
                }
            }
        }
    }
}

void TreeWidget::markConnectionForRemoval(ConnectionWidget* con) {
    m_removingConnections.push_back(con);
    redraw();
}

void TreeWidget::addTuileWidget(TuileWidget* newWidget) {
    m_tuileWidgets[newWidget->getID()]=newWidget;
    refreshChildrenTuileWidgets();
    deselectAllTuileWidgets();
    newWidget->select();
    selectTuileWidget(newWidget);
    DEBUG("TreeWidget: added the tuile widget to trees");
}

TuileWidget* TreeWidget::getTuileWidget(const unsigned int& id) {
    if(m_tuileWidgets.find(id)!=m_tuileWidgets.end()) {
        return m_tuileWidgets[id];
    }
    else {
        return NULL;
    }
}

void TreeWidget::markWidgetForRemoval(TuileWidget* removedWidget) {
    m_removingWidgets.push_back(removedWidget);
}

SeqWidget* TreeWidget::createSeqWidget(TuileWidget* t1, TuileWidget* t2) {
    AudioManager* audioMan=AudioManager::getInstance();
    SeqTuile* newSTuile = audioMan->addSeqTuile();
    audioMan->insertSeq(newSTuile, t1->getTuile(), t2->getTuile());
    SeqWidget* newWidget = new SeqWidget("seq", newSTuile);
    newWidget->getWidget()->position(t1->getWidget()->x(),t1->getWidget()->y());
    addTuileWidget(newWidget);
    DEBUG("TreeWidget: created a new SeqWidget");
    return newWidget;
}

TuileWidget* TreeWidget::createTuileWidget(const std::string& tuileName) {
    AudioManager* audioMan=AudioManager::getInstance();
    TuileWidget* newWidget=NULL;
    Tuile* newTuile=NULL;
    if(tuileName.find(".tui")!=string::npos) {
        newWidget = load(tuileName);
        if(newWidget) {
            newTuile = newWidget->getTuile();
        }
    }
    else if(tuileName.compare("Seq")==0) {
        SeqTuile* newSTuile = audioMan->addSeqTuile();
        newTuile = (Tuile*)newSTuile;
        newWidget = new SeqWidget(tuileName, newSTuile);
    }
    else if(tuileName.compare("loop")==0 || tuileName.compare("Loop")==0) {
        LoopTuile* newLTuile = audioMan->addLoopTuile();
        newTuile = (Tuile*)newLTuile;
        newWidget = new LoopWidget(tuileName, newLTuile);
    }
    else if(tuileName.compare("monitor")==0 
                || tuileName.compare("MidiOscMonitorTuile")==0) {
        MidiOscMonitorTuile* newMTuile = audioMan->addMidiOscMonitorTuile();
        newTuile = (Tuile*)newMTuile;
        newWidget = new MonitorWidget(tuileName, newMTuile);
    }
    else if(tuileName.compare("switch")==0 
                || tuileName.compare("MidiOscSwitchTuile")==0) {
        MidiOscSwitchTuile* newSwTuile = audioMan->addMidiOscSwitchTuile();
        newTuile = (Tuile*)newSwTuile;
        newWidget = new SwitchWidget(tuileName, newSwTuile);
    }
    else if(tuileName.compare("output")==0 
                || tuileName.compare("AudioOutput")==0) {
        AudioOutputTuile* newAOTuile = audioMan->addAudioOutputTuile(tuileName);
        newTuile = (Tuile*)newAOTuile;
        AudioOutputWidget* newAOWidget = 
                                new AudioOutputWidget(tuileName, newAOTuile);
        newWidget=newAOWidget;
        m_audioTuileWidgets[newAOWidget->getID()]=newAOWidget;
    }
    else if(tuileName.compare("input")==0 
                || tuileName.compare("AudioInput")==0) {
        AudioInputTuile* newAITuile = audioMan->addAudioInputTuile(tuileName);
        newTuile = (Tuile*)newAITuile;
        AudioInputWidget* newAIWidget = 
                            new AudioInputWidget(tuileName, newAITuile);
        newWidget=newAIWidget;
        m_audioTuileWidgets[newAIWidget->getID()]=newAIWidget;
    }
    else if(tuileName.find(".wav")!=string::npos 
                || tuileName.compare("SoundFile")==0) {
        SoundFileTuile* newSFTuile = audioMan->addSoundFileTuile(tuileName);
        newTuile = (Tuile*)newSFTuile;
        SoundFileWidget* newSFWidget = 
                                new SoundFileWidget(tuileName, newSFTuile);
        newWidget=newSFWidget;
        m_audioTuileWidgets[newSFWidget->getID()]=newSFWidget;
    }
    else if(tuileName.find(".dsp")!=string::npos 
                || tuileName.compare("Faust")==0) {
        FaustTuile* newFTuile = audioMan->addFaustTuile(tuileName);
        newTuile = (Tuile*)newFTuile;
        FaustWidget* newFWidget = new FaustWidget(tuileName, newFTuile);
        newWidget=newFWidget;
        m_audioTuileWidgets[newFWidget->getID()]=newFWidget;
    }
    if(newTuile && newWidget) {
        addTuileWidget(newWidget);
        DEBUG("TreeWidget: created the new TuileWidget "<<tuileName);
    }
    else {
        DEBUG("TreeWidget: could not create the new TuileWidget "<<tuileName);
    }
    return newWidget;
}

void TreeWidget::save(const std::string& fileName) {
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr rootNode = xmlNewNode(NULL, BAD_CAST "Tuiles");
    xmlDocSetRootElement(doc, rootNode);
    //save the tuiles
    for(unsigned int c=0; c<m_childrenTuileWidgets.size(); ++c) {
        m_childrenTuileWidgets[c]->save(rootNode);
    }
    //save the connections
    map<unsigned int, ConnectionWidget*>::iterator itCon=m_connections.begin();
    for(; itCon!=m_connections.end(); ++itCon) {
        itCon->second->save(rootNode);
    }
    xmlSaveFormatFileEnc(fileName.c_str(), doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

TuileWidget* TreeWidget::load(const std::string& fileName) {
    TuileWidget* firstWidget=NULL;
    int counter=0;
    unsigned int curIDCounter=AudioManager::getInstance()->getTuileIDCounter();
    xmlDocPtr doc = xmlReadFile(fileName.c_str(), NULL, 0);
    if(doc) {
        xmlNodePtr rootNode = xmlDocGetRootElement(doc);
        if(rootNode && 
                string((const char*)rootNode->name).compare("Tuiles")==0) {
            xmlNodePtr curNode;
            for(curNode= rootNode->children; curNode; 
                    curNode= curNode->next, counter++) {
                if(curNode->type == XML_ELEMENT_NODE) {
                    //try tuile
                    TuileWidget* newWidget = 
                        createTuileWidget(string((const char*)curNode->name));
                    if(newWidget) {
                        newWidget->load(curNode);
                        if(counter==0) {
                            firstWidget=newWidget;
                        }
                    }//otherwise maybe it is a connection
                    else if(string((const char*)curNode->name).
                                                compare("Connection")==0) {
                        loadConnection(curNode, curIDCounter);
                    }
                }
            }
        }
    }
    return firstWidget;
}

void TreeWidget::loadConnection(xmlNodePtr node, const unsigned int& idOffset) {
    char* from=NULL;
    char* to=NULL;
    from = (char*)xmlGetProp(node,(xmlChar*)"from");
    to = (char*)xmlGetProp(node,(xmlChar*)"to");
    if(from && to) {
        unsigned int fromID=atoi(from)+idOffset;
        unsigned int toID=atoi(to)+idOffset;
        if(m_audioTuileWidgets.find(fromID)!=m_audioTuileWidgets.end()
                && m_audioTuileWidgets.find(toID)!=m_audioTuileWidgets.end()) {
            AudioTuileWidget* fromWid= m_audioTuileWidgets[fromID];
            AudioTuileWidget* toWid= m_audioTuileWidgets[toID];
                
            m_connections[m_connectionIDCounter]=
                            new ConnectionWidget(m_connectionIDCounter, 
                                                    fromWid, toWid);
            m_connectionIDCounter++;
        }
    }
}

