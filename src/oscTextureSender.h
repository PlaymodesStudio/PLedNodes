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
    ofParameter<bool> flip;
    ofParameter<int> resampleX;
    ofParameter<int> resampleY;
    ofParameter<int> resampleXMethod;
    ofParameter<int> resampleYMethod;
    ofParameter<float> fader;
    ofParameter<ofTexture*> texture;
    
    glm::vec2 lastSize;
    
    ofEventListeners listeners;
    
    ofxOscSender oscSender;
};

#endif /* oscTextureSender_h */
