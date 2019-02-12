//
//  audioEngineController.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 26/07/2017.
//
//

#include "audioEngineController.h"

audioEngineController::audioEngineController() : ofxOceanodeNodeModel("Audio Controller"){
    addParameterToGroupAndInfo(oscHost.set("Osc Host", "localhost")).isSavePreset = false;
    addParameterToGroupAndInfo(oscPort.set("Osc Port", "11511")).isSavePreset = false;
    parameters->add(presetNum.set("Preset Num", 0, 0, 100));
    parameters->add(param_a.set("Param a", {0}, {0}, {1}));
    parameters->add(param_b.set("Param b", {0}, {0}, {1}));
    parameters->add(param_c.set("Param c", {0}, {0}, {1}));
    oscSender.setup(oscHost, ofToInt(oscPort));
    
    presetNum.addListener(this, &audioEngineController::presetNumSender);
    
    oscHost.addListener(this, &audioEngineController::oscHostAndPortListener);
    oscPort.addListener(this, &audioEngineController::oscHostAndPortListener);
    
    param_a.addListener(this, &audioEngineController::param_aSender);
    param_b.addListener(this, &audioEngineController::param_bSender);
    param_c.addListener(this, &audioEngineController::param_cSender);
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

