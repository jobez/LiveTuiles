/***************************************************************************
 *  FaustTuile.cpp
 *  2012- Florent Berthaut
 *  ANR INEDIT Project
 *  This file is part of LiveTuiles
 ****************************************************************************/

#include "FaustTuile.hpp"

#include <sstream>
#include <fstream>
#include <stdexcept>

#include "AudioManager.hpp"

using namespace std;

FaustTuile::FaustTuile():   AudioTuile(), 
                            m_processing(false),
                            m_bufferSize(4096) {}

FaustTuile::~FaustTuile() {}

void FaustTuile::load(const std::string& fileName) {
	m_fileName=fileName;

    //get string with content of file
    ifstream fileStream(m_fileName.c_str());
    string fileContent, line;
    if (!fileStream.is_open()) {
        throw std::logic_error("Error opening file "+fileName);
    }
    while (fileStream.good() ) {
        getline (fileStream,line);
        fileContent+=line+'\n';
    }
    fileStream.close();

    char errorMsg[256];
    m_dspFactory = createDSPFactory(0, NULL, "", "", m_fileName.c_str(), 
                                        fileContent.c_str(), "", errorMsg, 3);
    if(!m_dspFactory) {
        throw std::logic_error("Error compiling file "+fileName+" "+errorMsg);
    }
    m_dsp = createDSPInstance(m_dspFactory);
    if(!m_dsp) {
        throw std::logic_error("Error compiling file "+fileName+" "+errorMsg);
    }
    m_dsp->init(AudioManager::getInstance()->getSampleRate());
    m_bufferSize = AudioManager::getInstance()->getBufferSize();
    m_inputChannels = m_dsp->getNumInputs();
    m_dspInputBuffer = new float*[m_inputChannels];
    for(unsigned int c=0; c<m_inputChannels; ++c) {
        m_dspInputBuffer[c]=new float[m_bufferSize];
    }
    m_outputChannels = m_dsp->getNumOutputs();
    m_dspOutputBuffer = new float*[m_outputChannels];
    for(unsigned int c=0; c<m_outputChannels; ++c) {
        m_dspOutputBuffer[c]=new float[m_bufferSize];
    }

    m_internalBuffer.resize(m_outputChannels, vector<float>(m_bufferSize, 0));

    m_loaded=true;
}

void FaustTuile::unload() {
	m_loaded=false;
}

void FaustTuile::activate() {
    if(m_loaded) {
        m_position=0;
        m_active=true;
        m_activateAsked=true;
    }
}

void FaustTuile::deactivate() {
	m_active=false;
}

void FaustTuile::processBuffers(const int& nbFrames) {
/*
    if(!m_computed) {
        for(unsigned int c=0; c<m_internalBuffer.size(); ++c) {
            m_internalBuffer[c].assign(nbFrames, 0);
            for(unsigned int f=0; f<nbFrames && f<m_bufferSize; ++f) {
                m_dspInputBuffer[c][f]=0;
            }
        }
        if(m_active && m_inputTuiles.size()>0) {
            const vector<vector<float> >& inpBuf = 
                        m_inputTuiles.front()->getBuffer(); 

            if(m_activateAsked) { //crossfade when beginning the process
                float fade=0, fadeStep=1.0/float(nbFrames);
                for(unsigned int f=0; f<nbFrames && f<m_bufferSize; 
                                                        ++f, fade+=fadeStep) {
                    for(unsigned int c=0; c<m_inputChannels 
                                            && c<inpBuf.size(); ++c) {

                        m_dspInputBuffer[c][f]=inpBuf[c][f]*fade;
                    }
                }
                m_activateAsked=false;
            }
            else {
                for(unsigned int c=0; c<m_inputChannels 
                                        && c<inpBuf.size(); ++c) {
                    for(unsigned int f=0; f<nbFrames && f<m_bufferSize; ++f) {
                        m_dspInputBuffer[c][f]=inpBuf[c][f];
                    }
                }
            }
            m_dsp->compute(nbFrames, m_dspInputBuffer, m_dspOutputBuffer);
            for(unsigned int c=0; c<m_internalBuffer.size(); ++c) {
                for(unsigned int f=0; f<nbFrames && f<m_bufferSize; ++f) {
                    m_internalBuffer[c][f]=m_dspOutputBuffer[c][f];
                }
            }
        }
        m_computed=true;
    }
*/
}

