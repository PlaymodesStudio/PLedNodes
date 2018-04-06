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
    
    texturesInput.resize(5);
    for(int i = 0; i < 5; i++){
        parameters->add(texturesInput[i].set("Group In " + ofToString(i), nullptr));
    }
    
//    ofAddListener(parameters->parameterChangedE(), this, &waveScope::inputListener);
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
