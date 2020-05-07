//
//  valueEaser.cpp
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 05/07/2018.
//

#include "valueEaser.h"

valueEaser::valueEaser() : ofxOceanodeNodeModel("Value Easer"){
    addParameter(phasor.set("Phasor In", {0}, {0}, {1}));
    addParameter(input.set("Input", {0}, {0}, {1}));
    addParameter(pow.set("In-Out", {0}, {-1}, {1}));
    addParameter(bipow.set("InOut Amout", {0}, {-1}, {1}));
    addParameter(output.set("Output", {0}, {0}, {1}));
    
    color = ofColor::green;
    
    listener = phasor.newListener([this](vector<float> &vf){
        if(input.get().size() != lastInput.size() || input.get().size() != phasorValueOnValueChange.size()){
            output = input;
            lastInput = input;
            lastChangedValue = input;
            phasorValueOnValueChange = (vf.size() == 1) ? vector<float>(input.get().size(), vf[0]) : vf;
            lastPhase = vector<float>(input.get().size(), 0);
            reachedMax = vector<bool>(input.get().size(), false);
        }else{
            int inputSize = input.get().size();
            for(int i = 0; i < inputSize; i++){
                if(lastInput[i] != input.get()[i]){
                    phasorValueOnValueChange[i] = (vf.size() < inputSize) ? vf[0] : vf[i];
                    reachedMax[i] = false;
                    lastPhase[i] = 0;
                    lastChangedValue[i] = output.get()[i];
                }
            }
            vector<float> tempOutput = input.get();
            for(int i = 0; i < inputSize; i++){
                float phase = ((vf.size() < inputSize) ? vf[0] : vf[i]) - phasorValueOnValueChange[i];
                if(phase < 0) phase = 1+phase;
                if(getValueForPosition(pow, i) != 0){
                    customPow(phase, getValueForPosition(pow, i));
                }
                if(getValueForPosition(bipow, i) != 0){
                    phase = (phase*2) - 1;
                    customPow(phase, getValueForPosition(bipow, i));
                    phase = (phase + 1) / 2.0;
                }
                if(phase < lastPhase[i]) reachedMax[i] = true;
                else lastPhase[i] = phase;
                if(!reachedMax[i]){
                    tempOutput[i] = smoothinterpolate(lastChangedValue[i], input.get()[i], phase);
                }else{
                    tempOutput[i] = input.get()[i];
                }
            }
            lastInput = input;
            output = tempOutput;
        }
    });
}

void valueEaser::customPow(float & value, float pow){
    float k1 = 2*pow*0.99999;
    float k2 = (k1/((-pow*0.999999)+1));
    float k3 = k2 * abs(value) + 1;
    value = value * (k2+1) / k3;
}

float valueEaser::smoothinterpolate(float start, float end, float pos){
    float oldRandom = start;
    float pastRandom = start;
    float newRandom = end;
    float futureRandom  = end;
    float L0 = (newRandom - pastRandom) * 0.5;
    float L1 = L0 + (oldRandom-newRandom);
    float L2 = L1 + ((futureRandom - oldRandom)*0.5) + (oldRandom - newRandom);
    return oldRandom + (pos * (L0 + (pos * ((pos * L2) - (L1 + L2)))));
    
}
