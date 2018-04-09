//
//  waveScope.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#ifndef waveScope_h
#define waveScope_h

#include "ofxOceanodeNodeModelExternalWindow.h"

class waveScope : public ofxOceanodeNodeModelExternalWindow{
public:
    waveScope();
    ~waveScope(){};
    
    ofxOceanodeAbstractConnection* createConnectionFromCustomType(ofxOceanodeContainer& c, ofAbstractParameter& source, ofAbstractParameter& sink) override;
    
private:
    void drawInExternalWindow(ofEventArgs &e) override;
    
    void inputListener(int index);
    
    map<string, ofEventListener> listeners;
    vector<bool> ifNewCreatedCheker;
    
    vector<ofParameter<ofTexture*>>     texturesInput;
    
    int contentWidthOffset;
    int mousePressInititalX;
    bool isInMovableRegion;
    bool hasColor;
};

#endif /* waveScope_h */
