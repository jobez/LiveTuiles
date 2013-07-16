/***************************************************************************
 *  AudioOutputTuile.cpp
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/

#include "AudioOutputTuile.hpp"

#include <iostream>
#include <stdexcept>

#include "AudioManager.hpp"

using namespace std;

AudioOutputTuile::AudioOutputTuile(): AudioTuile(){}

AudioOutputTuile::~AudioOutputTuile() {}

void AudioOutputTuile::load(const std::string& output) {
    AudioManager* man=AudioManager::getInstance();
    m_outputPortLeft=jack_port_register(man->getJackClient(),
                                        (output+"-L").c_str(),
                                        JACK_DEFAULT_AUDIO_TYPE, 
                                        JackPortIsOutput, 0);
    m_outputPortRight=jack_port_register(man->getJackClient(),
                                        (output+"-R").c_str(),
                                        JACK_DEFAULT_AUDIO_TYPE, 
                                        JackPortIsOutput, 0);
    m_internalBuffer.resize(2, vector<jack_default_audio_sample_t>(0, 0));

    //connect to the soundcard by default
    jack_connect(man->getJackClient(), 
                    string("LiveTuiles"+output+"-L").c_str(), 
                    "system:playback_1");
    jack_connect(man->getJackClient(), 
                    string("LiveTuiles"+output+"-R").c_str(), 
                    "system:playback_2");
	m_loaded=true;
}

void AudioOutputTuile::unload() {
	m_loaded=false;
}

void AudioOutputTuile::processBuffers(const int& nbFrames) {
    if(!m_computed) {
        m_internalBuffer[0].assign(nbFrames, 0);
        m_internalBuffer[1].assign(nbFrames, 0);
        if(m_procActive) {
            cout<<"process output"<<endl;
            jack_default_audio_sample_t* bufL=(jack_default_audio_sample_t *)
                            jack_port_get_buffer(m_outputPortLeft, nbFrames);
            jack_default_audio_sample_t* bufR=(jack_default_audio_sample_t *)
                            jack_port_get_buffer(m_outputPortRight, nbFrames);

            vector<AudioTuile*>::iterator itTui=m_inputTuiles.begin();
            for(; itTui!=m_inputTuiles.end(); ++itTui) {
                (*itTui)->processBuffers(nbFrames);
                for(int f=0; f<nbFrames; ++f) {
                    m_internalBuffer[0][f]+=(*itTui)->getBuffer()[0][f];
                    m_internalBuffer[1][f]+=(*itTui)->getBuffer()[1][f];
                }
            }
            for(int f=0; f<nbFrames; ++f) {
                bufL[f]=m_internalBuffer[0][f];
                bufR[f]=m_internalBuffer[1][f];
            }
        }
        m_computed=true;
     }
}

