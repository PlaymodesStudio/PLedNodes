//
//  audioEngineController.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 26/07/2017.
//
//

#ifndef audioEngineController_h
#define audioEngineController_h

#include "ofxOceanodeNodeModel.h"
#include "ofxOsc.h"

class audioEngineController : public ofxOceanodeNodeModel {
public:
    audioEngineController();
    ~audioEngineController(){};
    
    void loadBeforeConnections(ofJson &json){
        deserializeParameter(json, oscHost);
        deserializeParameter(json, oscPort);
    }
    
private:
    void oscHostAndPortListener(string &s);
    
    void presetNumSender(int &i);
    void param_aSender(vector<float> &vf);
    void param_bSender(vector<float> &vf);
    void param_cSender(vector<float> &vf);
        
    ofParameter<string> oscHost;
    ofParameter<string> oscPort;
    ofParameter<int>    presetNum;
    ofParameter<vector<float>>  param_a;
    ofParameter<vector<float>>  param_b;
    ofParameter<vector<float>>  param_c;
    
    ofxOscSender oscSender;
};

#endif /* audioEngineController_h */
