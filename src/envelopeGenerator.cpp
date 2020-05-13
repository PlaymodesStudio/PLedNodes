//
//  envelopeGenerator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 12/07/2017.
//
//

#include "envelopeGenerator.h"

envelopeGenerator::envelopeGenerator() : ofxOceanodeNodeModel("Envelope Generator")
{
    easeStringFuncs = {"EASE_LINEAR", " EASE_IN_QUAD", " EASE_OUT_QUAD", " EASE_IN_OUT_QUAD", " EASE_IN_CUBIC", " EASE_OUT_CUBIC", " EASE_IN_OUT_CUBIC", " EASE_IN_QUART", " EASE_OUT_QUART", " EASE_IN_OUT_QUART", " EASE_IN_QUINT", " EASE_OUT_QUINT", " EASE_IN_OUT_QUINT", " EASE_IN_SINE", " EASE_OUT_SINE", " EASE_IN_OUT_SINE", " EASE_IN_EXPO", " EASE_OUT_EXPO", " EASE_IN_OUT_EXPO", " EASE_IN_CIRC", " EASE_OUT_CIRC", "EASE_IN_OUT_CIRC"};
    
    addParameter(phasor.set("Phase", {0}, {0}, {1}));
    addParameter(gateIn.set("GateIn", {0}, {0}, {1}));
    addParameter(hold.set("Hold", {0}, {0}, {1}));
    addParameter(attack.set("A", {0}, {0}, {1}));
    addParameter(decay.set("D", {0}, {0}, {1}));
    addParameter(sustain.set("S", {1}, {0}, {1}));
    addParameter(release.set("R", {0}, {0}, {1}));

    //ofParameter<char> alabel("A.Ctrls", ' ');
    //addParameter(alabel);
    addParameter(attackPow.set("A.Pow", {0}, {-1}, {1}));
    addParameter(attackBiPow.set("A.BiPow", {0}, {-1}, {1}));
    
    //ofParameter<char> dlabel("D.Ctrls", ' ');
    //addParameter(dlabel);
    addParameter(decayPow.set("D.Pow", {0}, {-1}, {1}));
    addParameter(decayBiPow.set("D.BiPow", {0}, {-1}, {1}));
    
    //ofParameter<char> rlabel("R.Ctrls", ' ');
    //addParameter(rlabel);
    addParameter(releasePow.set("R.Pow", {0}, {-1}, {1}));
    addParameter(releaseBiPow.set("R.BiPow", {0}, {-1}, {1}));
    
    addParameter(curvePreview.set("Curve", {0}, {0}, {1}));
    addParameter(output.set("Output", {0}, {0}, {1}));
    oldGateIn = {0};
    
    outputComputeVec = {0};
    
    listener  = phasor.newListener(this, &envelopeGenerator::phasorListener);
    
    curvePreviewListeners.push(hold.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(attack.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(decay.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(sustain.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(release.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(attackPow.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(attackBiPow.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(decayPow.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(decayBiPow.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(releasePow.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    curvePreviewListeners.push(releaseBiPow.newListener([this](vector<float> &vf){recalculatePreviewCurve();}));
    recalculatePreviewCurve();
    
    lastGateInSize = -1;
}

void envelopeGenerator::phasorListener(vector<float> &vf){
    int inputSize = gateIn.get().size();
    if(inputSize != lastGateInSize){
        output = vector<float>(inputSize, 0);
        lastInput = vector<float>(inputSize, 0);
        phasorValueOnValueChange = vector<float>(inputSize, 0);
        lastPhase = vector<float>(inputSize, 0);
        reachedMax = vector<bool>(inputSize, false);
        envelopeStage = vector<int>(inputSize, envelopeEnd);
        maxValue = vector<float>(inputSize, 0);
        initialPhase = vector<float>(inputSize, 0);
        lastSustainValue = vector<float>(inputSize, 0);
        lastGateInSize = inputSize;
    }else{
        for(int i = 0; i < inputSize; i++){
            float f = getValueForIndex(vf, i);
            if(lastInput[i] == 0 && gateIn.get()[i] != 0){
                if(getValueForIndex(attack, i) == 0){
                    if(getValueForIndex(decay, i) == 0){
                        envelopeStage[i] = envelopeSustain;
                    }else{
                        envelopeStage[i] = envelopeDecay;
                    }
                }else{
                    envelopeStage[i] = envelopeAttack;
                }
                phasorValueOnValueChange[i] = f;
                reachedMax[i] = false;
                lastPhase[i] = 0;
                maxValue[i] = gateIn.get()[i];
                initialPhase[i] = f;
                lastSustainValue[i] = gateIn.get()[i];
            }else if(lastInput[i] != 0 && gateIn.get()[i] == 0){
                if(getValueForIndex(hold, i) == 0){
                    if(getValueForIndex(release, i) > 0){
                        envelopeStage[i] = envelopeRelease;
                    }else{
                        envelopeStage[i] = envelopeEnd;
                    }
                    phasorValueOnValueChange[i] = f;
                    reachedMax[i] = false;
                    lastPhase[i] = 0;
                }
            }
        }
        vector<float> tempOutput = gateIn.get();
        for(int i = 0; i < inputSize; i++){
            float f = getValueForIndex(vf, i);
            float phase = f - phasorValueOnValueChange[i];
            if(phase < 0) phase = 1+phase;
            if(phase < lastPhase[i]) reachedMax[i] = true;
            else lastPhase[i] = phase;
            
            if(getValueForIndex(hold, i) > 0){
                float holdPhase = f - initialPhase[i];
                if(holdPhase < 0) holdPhase += 1;
                
                if(holdPhase > getValueForIndex(hold, i) && envelopeStage[i] != envelopeRelease && envelopeStage[i] != envelopeEnd){
                    phasorValueOnValueChange[i] = f;
                    if(getValueForIndex(release, i) > 0){
                        envelopeStage[i] = envelopeRelease;
                    }else{
                        envelopeStage[i] = envelopeEnd;
                    }
                    reachedMax[i] = false;
                    lastPhase[i] = 0;
                    phase = 0;
                }
            }
            
            if(envelopeStage[i] == envelopeAttack){
                if(phase > getValueForIndex(attack, i) || reachedMax[i] == true){
                    phasorValueOnValueChange[i] = f;
                    if(getValueForIndex(decay, i) == 0){
                        envelopeStage[i] = envelopeSustain;
                    }else{
                        envelopeStage[i] = envelopeDecay;
                    }
                    reachedMax[i] = false;
                    lastPhase[i] = 0;
                    phase = 0;
                }else{
                    phase = ofMap(phase, 0, getValueForIndex(attack, i), 0, 1, true);
                    if(getValueForIndex(attackPow, i) != 0){
                        customPow(phase, getValueForIndex(attackPow, i));
                    }
                    if(getValueForIndex(attackBiPow, i) != 0){
                        phase = (phase*2) - 1;
                        customPow(phase, getValueForIndex(attackBiPow, i));
                        phase = (phase + 1) / 2.0;
                    }
                    tempOutput[i] = smoothinterpolate(0, 1, phase);
                    if(phase != 0){
                        lastSustainValue[i] = tempOutput[i];
                    }
                }
            }
            if(envelopeStage[i] == envelopeDecay){
                if(phase > getValueForIndex(decay, i) || reachedMax[i] == true){
                    phasorValueOnValueChange[i] = f;
                    envelopeStage[i] = envelopeSustain;
                    reachedMax[i] = false;
                    lastPhase[i] = 0;
                    phase = 0;
                }else{
                    phase = ofMap(phase, 0, getValueForIndex(decay, i), 0, 1, true);
                    if(getValueForIndex(decayPow, i) != 0){
                        customPow(phase, getValueForIndex(decayPow, i));
                    }
                    if(getValueForIndex(decayBiPow, i) != 0){
                        phase = (phase*2) - 1;
                        customPow(phase, getValueForIndex(decayBiPow, i));
                        phase = (phase + 1) / 2.0;
                    }
                    tempOutput[i] = smoothinterpolate(1, getValueForIndex(sustain, i) * maxValue[i], phase);
                    lastSustainValue[i] = tempOutput[i];
                }
            }
            if(envelopeStage[i] == envelopeSustain){
                tempOutput[i] = maxValue[i] * getValueForIndex(sustain, i);
                lastSustainValue[i] = tempOutput[i];
            }
            if(envelopeStage[i] == envelopeRelease){
                if(phase > getValueForIndex(release, i) || reachedMax[i] == true){
                    phasorValueOnValueChange[i] = f;
                    envelopeStage[i] = envelopeEnd;
                    reachedMax[i] = false;
                    lastPhase[i] = 0;
                    phase = 0;
                }else{
                    phase = ofMap(phase, 0, getValueForIndex(release, i), 0, 1, true);
                    if(getValueForIndex(releasePow, i) != 0){
                        customPow(phase, getValueForIndex(releasePow, i));
                    }
                    if(getValueForIndex(releaseBiPow, i) != 0){
                        phase = (phase*2) - 1;
                        customPow(phase, getValueForIndex(releaseBiPow, i));
                        phase = (phase + 1) / 2.0;
                    }
                    tempOutput[i] = smoothinterpolate(lastSustainValue[i], 0, phase);
                }
            }
            if(envelopeStage[i] == envelopeEnd){
                tempOutput[i] = 0;
            }
        }
        lastInput = gateIn;
        output = tempOutput;
    }
}

void envelopeGenerator::customPow(float & value, float pow){
    float k1 = 2*pow*0.99999;
    float k2 = (k1/((-pow*0.999999)+1));
    float k3 = k2 * abs(value) + 1;
    value = value * (k2+1) / k3;
}

float envelopeGenerator::smoothinterpolate(float start, float end, float pos){
    float oldRandom = start;
    float pastRandom = start;
    float newRandom = end;
    float futureRandom  = end;
    float L0 = (newRandom - pastRandom) * 0.5;
    float L1 = L0 + (oldRandom-newRandom);
    float L2 = L1 + ((futureRandom - oldRandom)*0.5) + (oldRandom - newRandom);
    return oldRandom + (pos * (L0 + (pos * ((pos * L2) - (L1 + L2)))));
    
}

void envelopeGenerator::recalculatePreviewCurve(){
    int maxSize = 100;
    vector<float> tempOutput;
    //attack
    int attackSize = attack->at(0)*maxSize;
    tempOutput.resize(attackSize);
    for(int i = 0; i < attackSize; i++){
        float phase = float(i) / float(attackSize);
        if(attackPow->at(0) != 0){
            customPow(phase, attackPow->at(0));
        }
        if(attackBiPow->at(0) != 0){
            phase = (phase*2) - 1;
            customPow(phase, attackBiPow->at(0));
            phase = (phase + 1) / 2.0;
        }
        tempOutput[i] = smoothinterpolate(0, 1, phase);
    }
    
//    //Hold
//    int holdSize = hold->at(0)*maxSize;
//    int holdPos = tempOutput.size();
//    tempOutput.resize(holdPos + holdSize);
//    fill(tempOutput.begin()+holdPos, tempOutput.end(), 1);
//
    //Decay
    int decaySize = decay->at(0)*maxSize;
    int decayPos = tempOutput.size();
    tempOutput.resize(decayPos + decaySize);
    for(int i = 0; i < decaySize; i++){
        float phase = float(i) / float(decaySize);
        if(decayPow->at(0) != 0){
            customPow(phase, decayPow->at(0));
        }
        if(decayBiPow->at(0) != 0){
            phase = (phase*2) - 1;
            customPow(phase, decayBiPow->at(0));
            phase = (phase + 1) / 2.0;
        }
        tempOutput[decayPos + i] = smoothinterpolate(1, sustain->at(0), phase);
    }
    
    //Sustain
    int sustainSize = maxSize/2;
    int sustainPos = tempOutput.size();
    tempOutput.resize(sustainPos + sustainSize);
    fill(tempOutput.begin()+sustainPos, tempOutput.end(), sustain->at(0));
    
    //Release
    int releaseSize = release->at(0)*maxSize;
    int releasePos = tempOutput.size();
    tempOutput.resize(releasePos + releaseSize);
    for(int i = 0; i < releaseSize; i++){
        float phase = float(i) / float(releaseSize);
        if(releasePow->at(0) != 0){
            customPow(phase, releasePow->at(0));
        }
        if(releaseBiPow->at(0) != 0){
            phase = (phase*2) - 1;
            customPow(phase, releaseBiPow->at(0));
            phase = (phase + 1) / 2.0;
        }
        tempOutput[releasePos + i] = smoothinterpolate(sustain->at(0), 0, phase);
    }
    
    curvePreview = tempOutput;
}
