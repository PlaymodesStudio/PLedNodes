//
//  waveScope.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#include "waveScope.h"
#include "ofxOceanodeContainer.h"

waveScope::waveScope() : ofxOceanodeNodeModelExternalWindow("Texture Scope"){
    
    texturesInput.resize(1);
    for(int i = 0; i < 1; i++){
        parameters->add(texturesInput[i].set("Group In " + ofToString(i), nullptr));
    }
    ifNewCreatedCheker.push_back(false);
    
    listeners[texturesInput.back().getName()] = texturesInput.back().newListener([&](ofTexture* &tex){
        inputListener(0);
    });
}

void waveScope::inputListener(int index){
    if(texturesInput[index].get() != nullptr && !ifNewCreatedCheker[index]){
        texturesInput.push_back(ofParameter<ofTexture*>());
        ifNewCreatedCheker.push_back(false);
        int newCreatedIndex = texturesInput.size()-1;
        parameters->add(texturesInput.back().set("Group In " + ofToString(newCreatedIndex), nullptr));
        listeners[texturesInput.back().getName()] = texturesInput.back().newListener([&, newCreatedIndex](ofTexture* &tex){
            inputListener(newCreatedIndex);
        });
        ifNewCreatedCheker[index] = true;
        ofNotifyEvent(parameterGroupChanged);
    }
    else if(texturesInput[index].get() == nullptr){
//        parameters->remove(texturesInput[index].getEscapedName());
//        listeners.erase(texturesInput[index].getName());
//        texturesInput.erase(texturesInput.begin()+index);
//        ifNewCreatedCheker.erase(ifNewCreatedCheker.begin()+index);
        parameters->remove(texturesInput.back().getEscapedName());
        listeners.erase(texturesInput.back().getName());
        texturesInput.erase(texturesInput.end()-1);
        ifNewCreatedCheker.erase(ifNewCreatedCheker.end()-1);
        ifNewCreatedCheker[index] = false;
//        for(int ii = 0; ii < texturesInput.size(); ii++){
            //parameters->remove(texturesInput[ii].getEscapedName());
//        }
//        for(int ii = 0; ii < texturesInput.size(); ii++){
//            texturesInput[ii].setName("Group In " + ofToString(ii));
//            parameters->add(texturesInput[ii]);
//        }
        ofNotifyEvent(parameterGroupChanged);
    }
}

void waveScope::drawInExternalWindow(ofEventArgs &e){
    ofBackground(0);
    ofSetColor(255);
    //
    int contentWidth = 2*ofGetWidth()/3 + contentWidthOffset;
    
    
    int numActiveGroups = 0;
    for(auto texture : texturesInput){
        if(texture != nullptr){
            numActiveGroups++;
        }
    }
    
    if(numActiveGroups > 0){
        int elementHeight = ofGetHeight() / numActiveGroups;
        int oscDrawIndex = 0;
        
        //Draw the groups
        for(int i = 0; i < texturesInput.size(); i++){
            if(texturesInput[i] != nullptr){
                int topPosition = (elementHeight * oscDrawIndex);
                texturesInput[i].get()->draw(0,topPosition, ofGetWidth(), elementHeight);
                ofPushStyle();
                ofSetColor(ofColor::indianRed);
                ofNoFill();
                ofSetLineWidth(2);
                ofDrawRectangle(0, topPosition, ofGetWidth(), elementHeight);
                ofPopStyle();
                oscDrawIndex++;
            }
        }
    }
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}

ofxOceanodeAbstractConnection* waveScope::createConnectionFromCustomType(ofxOceanodeContainer& c, ofAbstractParameter& source, ofAbstractParameter& sink){
    if(source.type() == typeid(ofParameter<ofTexture*>).name()){
        return c.connectConnection(source.cast<ofTexture*>(), sink.cast<ofTexture*>());
    }
    return nullptr;
}
