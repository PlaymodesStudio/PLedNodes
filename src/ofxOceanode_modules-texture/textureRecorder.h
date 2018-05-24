//
//  textureRecorder.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 24/05/2018.
//
//

#ifndef textureRecorder_h
#define textureRecorder_h

#include "ofxOceanodeNodeModel.h"

class textureRecorder : public ofxOceanodeNodeModel{
public:
    textureRecorder();
    ~textureRecorder(){};
    
private:
    void phasorInListener(float &f);
    void inputListener(ofTexture* &texture);
    void recordListener(bool &b);
    
    ofParameter<float>  phasorIn;
    ofParameter<bool>   record;
    ofParameter<bool>   autoRecLoop;
    ofParameter<string> filename;
    ofParameter<ofTexture*>    input;
    
    float oldPhasor;
    int frameCounter;
    string initRecordingTimestamp;
};

#endif /* textureRecorder_h */
