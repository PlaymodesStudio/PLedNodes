//
//  delta.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 14/07/2017.
//
//

#include "delta.h"

delta::delta() : ofxOceanodeNodeModel("Delta"){
    parameters->add(gain.set("Gain", 1, 0, 100));
    parameters->add(invert.set("Invert", false));
    parameters->add(smooth.set("Smooth", 0, 0, 1));
    parameters->add(input.set("Input", {0}, {0}, {1}));
    parameters->add(output.set("Output", {0}, {0}, {1}));
    
    inputStore = {0};
    outputStore = {0};

    listener = input.newListener(this, &delta::computeOutput);
}

void delta::computeOutput(vector<float> &in){
    if(outputStore.size() != input->size()){
        outputStore.resize(input->size(), 0);
    }
    if(inputStore.size() != input->size()){
        inputStore = input;
    }
    else{
        vector<float> tempOut;
        tempOut.resize(input->size(), 0);
        for(int i = 0; i < input->size(); i++){
            tempOut[i] = abs(input.get()[i]-inputStore[i])*gain;
            tempOut[i] = (smooth*outputStore[i]) + ((1-smooth)*tempOut[i]);
            tempOut[i] = ofClamp(tempOut[i], 0, 1);
        }
        inputStore = in;
        outputStore = tempOut;
        output = tempOut;
    }
    
}
