//
//  audioEngineController.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 26/07/2017.
//
//

#include "audioEngineController.h"
#include "sharedInfo.h"

audioEngineController::audioEngineController() : ofxOceanodeNodeModel("OSC Audio Controller"){
    addParameter(oscHost.set("Host", "localhost"), ofxOceanodeParameterFlags_DisableSavePreset);
    addParameter(oscPort.set("Port", "11511"));//.isSavePreset = false;
    addParameter(presetNum.set("Preset.N", 0, 0, 100));
    addParameter(param_a.set("Param.A", {0}, {0}, {1}));
    addParameter(param_b.set("Param.B", {0}, {0}, {1}));
    addParameter(param_c.set("Param.C", {0}, {0}, {1}));
    addParameter(param_d.set("Param.D", {0}, {0}, {1}));
    oscSender.setup(oscHost, ofToInt(oscPort));
    
    presetNum.addListener(this, &audioEngineController::presetNumSender);
    
    oscHost.addListener(this, &audioEngineController::oscHostAndPortListener);
    oscPort.addListener(this, &audioEngineController::oscHostAndPortListener);
    
    param_a.addListener(this, &audioEngineController::param_aSender);
    param_b.addListener(this, &audioEngineController::param_bSender);
    param_c.addListener(this, &audioEngineController::param_cSender);
    param_d.addListener(this, &audioEngineController::param_dSender);
    
    color = ofColor::violet;
    oscHost = sharedInfo::getInstance().getString("IP");
}

void audioEngineController::oscHostAndPortListener(string &s){
    oscSender.setup(oscHost, ofToInt(oscPort));
}

void audioEngineController::presetNumSender(int &i){
    ofxOscMessage message;
    message.setAddress("/audio/preset");
    message.addIntArg(i);
    oscSender.sendMessage(message);
}

void audioEngineController::param_aSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("/audio/va");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

void audioEngineController::param_bSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("/audio/vb");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

void audioEngineController::param_cSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("/audio/vc");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

void audioEngineController::param_dSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("/audio/vd");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

