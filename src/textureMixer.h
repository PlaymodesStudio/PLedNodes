
#ifndef textureMixer_h
#define textureMixer_h

#include "ofxOceanodeNodeModel.h"
#include "sharedResources.h"

class textureMixer : public ofxOceanodeNodeModel{
public:
    textureMixer();
    ~textureMixer();
    void setup() override;
    
private:
    void computeOutput(ofTexture* &in);
    
    ofParameter<int>                    triggerTextureIndex;
    ofParameter<int>                    mixMode;
    vector<string>                      mixModeName;
    ofParameter<float>                  crossFader;

    vector<ofParameter<ofTexture*>>     inputs;
    ofParameter<ofTexture*>             output;
    
    ofFbo                               outputFbo;
    ofShader                            mixerShader;
    
    int                                 texture0Location;
    int                                 texture1Location;

    sharedResources*                    resources;
    bool    isSetup;

};

#endif /* textureMixer_h */
