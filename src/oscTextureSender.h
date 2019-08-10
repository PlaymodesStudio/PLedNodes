//
//  oscTextureSender.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 05/08/2019.
//

#ifndef oscTextureSender_h
#define oscTextureSender_h

#include "ofxOceanodeNodeModel.h"
#include "ofxOsc.h"

class oscTextureSender : public ofxOceanodeNodeModel {
public:
    oscTextureSender();
    ~oscTextureSender(){};
    
    void draw(ofEventArgs &a) override;
private:
    void oscHostAndPortListener(string &s);
        
    ofParameter<string> oscHost;
    ofParameter<string> oscPort;
    ofParameter<string> address;
    ofParameter<int> resample;
    ofParameter<int> resampleMethod;
    ofParameter<ofTexture*> texture;
    
    ofEventListeners listeners;
    
    ofxOscSender oscSender;
};

#endif /* oscTextureSender_h */
