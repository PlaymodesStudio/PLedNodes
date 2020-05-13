//
//  senderManager.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "senderManager.h"

senderManager::senderManager() : ofxOceanodeNodeModel("Syphon Server"){
    addParameter(enable.set("Enable", 1), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(syphonName.set("Server", "Texture"), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(masterFader.set("Opacity", 1, 0, 1), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(textureIn.set("Texture In", nullptr));
    
    listeners.push(textureIn.newListener(this, &senderManager::sendTexture));
    
    bool tempEnable = true;
    enableSyphonListener(tempEnable);
    color = ofColor::lightGray;
}


void senderManager::sendTexture(ofTexture *&info){
#ifdef TARGET_OSX
    if(syphonServer != NULL && enable && info != nullptr){
        if(colorFbo.getHeight() != info->getHeight() || colorFbo.getWidth() != info->getWidth()){
            colorFbo.allocate(info->getWidth(), info->getHeight(), GL_RGB);
        }
        colorFbo.begin();
        ofClear(0, 0, 0, 255);
        ofPushStyle();
        ofSetColor(masterFader * 255);
        info->draw(0, 0);
        ofPopStyle();
        colorFbo.end();
        syphonServer->publishTexture(&colorFbo.getTexture());
    }
#endif
}


#pragma mark -- Listeners --
void senderManager::enableSyphonListener(bool &b){
#ifdef TARGET_OSX
    if(b){
        syphonServer = new ofxSyphonServer;
        
        syphonServer->setName(syphonName);
        
        listeners.push(syphonName.newListener(this, &senderManager::syphonNameListener));
    }else{
        listeners.unsubscribe(1);
        delete syphonServer;
    }
#endif
}

void senderManager::syphonNameListener(string &s){
#ifdef TARGET_OSX
    syphonServer->setName(syphonName);
#endif
}
