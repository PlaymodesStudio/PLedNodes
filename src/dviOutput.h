//
//  dviOutput.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 14/01/2019.
//

#ifndef dviOutput_h
#define dviOutput_h

#include "ofxOceanodeNodeModelExternalWindow.h"

class dviOutput : public ofxOceanodeNodeModelExternalWindow{
public:
    dviOutput() : ofxOceanodeNodeModelExternalWindow("Dvi Output"){};
    ~dviOutput(){};
    
    void setup() override{
        parameters->add(texture.set("Texture", nullptr));
        addParameterToGroupAndInfo(masterFader.set("Master Fader", 1, 0, 1)).isSavePreset = false;
    };
    
    void presetHasLoaded() override{
        setShowWindow(true);
        auto currentWin = ofGetCurrentWindow();
        setExternalWindowFullScreen(true);
        currentWin->makeCurrent();
        ofGetMainLoop()->setCurrentWindow(currentWin);
    }
    
    void
    
private:
    void drawInExternalWindow(ofEventArgs &e) override{
        ofBackground(0);
        if(texture.get() != nullptr){
            ofPushStyle();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(masterFader*255);
            texture.get()->setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            texture.get()->draw(ofGetWidth()/2, ofGetHeight()/2);
            ofPopStyle();
        }
    }
    
    
    ofParameter<ofTexture*> texture;
    ofParameter<float> masterFader;
};

#endif /* dviOutput_h */
