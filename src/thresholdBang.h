//
//  thresholdBang.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 07/08/2019.
//

#ifndef thresholdBang_h
#define thresholdBang_h

#include "ofxOceanodeNodeModel.h"

class thresholdBang : public ofxOceanodeNodeModel{
public:
    thresholdBang() : ofxOceanodeNodeModel("Threshold Bang"){};
    
    void setup(){
        addParameter(input.set("Input", {0}, {0}, {1}));
        addParameter(threshold.set("Thrs.", {1}, {0}, {1}));
        addParameter(frameMode.set("Frame", false));
        addParameter(output.set("Output", {0}, {0}, {1}));
        
        listener = input.newListener([this](vector<float> &vf){
            if(vf.size() != lastVal.size()){
                lastVal = vf;
            }
            vector<float> tempOutput(input->size(), 0);
            for(int i = 0; i < input->size(); i++){
                float indexThreshold;
                if(threshold->size() == vf.size()){
                    indexThreshold = threshold.get()[i];
                }else{
                    indexThreshold = threshold.get()[0];
                }
                if(vf[i] >= indexThreshold && lastVal[i] < indexThreshold){
                    tempOutput[i] = 1;
                }
            }
            lastVal = vf;
            output = tempOutput;
            if(!frameMode){
                tempOutput = vector<float>(tempOutput.size(), 0);
                output = tempOutput;
            }
        });
    }
    
private:
    ofParameter<vector<float>> input;
    ofParameter<vector<float>> threshold;
    ofParameter<bool>   frameMode;
    ofParameter<vector<float>> output;
    
    vector<float> lastVal;
    
    ofEventListener listener;
};

#endif /* thresholdBang_h */
