//
//  senderManager.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "senderManager.h"

senderManager::senderManager() : ofxOceanodeNodeModel("Sender Manager"){
    parameters->add(enable.set("Enable", 0));
    parameters->add(syphonName.set("Server Name", "Texture"));
    parameters->add(masterFader.set("Master Fader", 1, 0, 1));
    parameters->add(textureIn.set("Texture In", nullptr));
    
    listeners.push(textureIn.newListener(this, &senderManager::sendTexture));
    
    listeners.push(enable.newListener(this, &senderManager::enableSyphonListener));
    
    enable = true;
}


void senderManager::sendTexture(ofTexture *&info){
    if(syphonServer != NULL && enable && info != nullptr){
        if(colorFbo.getHeight() != info->getHeight() || colorFbo.getWidth() != info->getWidth()){
            colorFbo.allocate(info->getWidth(), info->getHeight(), GL_RGB);
        }
        colorFbo.begin();
        ofClear(0, 0, 0, 255);
        info->draw(0, 0);
        colorFbo.end();
        syphonServer->publishTexture(&colorFbo.getTexture());
    }
}


#pragma mark -- Listeners --
void senderManager::enableSyphonListener(bool &b){
    if(b){
        syphonServer = new ofxSyphonServer;
        
        syphonServer->setName(syphonName);
        
        listeners.push(syphonName.newListener(this, &senderManager::syphonNameListener));
    }else{
        syphonName.removeListener(this, &senderManager::syphonNameListener);
        delete syphonServer;
    }
}

void senderManager::syphonNameListener(string &s){
    syphonServer->setName(syphonName);
}
