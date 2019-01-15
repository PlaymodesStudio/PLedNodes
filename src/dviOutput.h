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
    };
    
    void presetHasLoaded() override{
        setShowWindow(true);
        auto currentWin = ofGetCurrentWindow();
        setExternalWindowFullScreen(true);
        currentWin->makeCurrent();
        ofGetMainLoop()->setCurrentWindow(currentWin);
    }
    
private:
    void drawInExternalWindow(ofEventArgs &e) override{
        ofBackground(0);
        if(texture.get() != nullptr){
            texture.get()->setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            texture.get()->draw(0, 100);
        }
    }
    
    
    
    ofParameter<ofTexture*> texture;
};

#endif /* dviOutput_h */
