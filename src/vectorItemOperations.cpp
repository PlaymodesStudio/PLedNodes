//
//  vectorItemOperations.cpp
//  Espills_controller
//
//  Created by Eduard Frigola on 01/08/2018.
//

#include "vectorItemOperations.h"

vectorItemOperations::vectorItemOperations() : ofxOceanodeNodeModel("Vector Item Operationsa"){
    parameters->add(triggerIndex.set("Trigger Index", 0, 0, 1));
    parameters->add(in1.set("Input 1", {0}, {0}, {1}));
    parameters->add(in2.set("Input 2", {0}, {0}, {1}));
    parameters->add(createDropdownAbstractParameter("Operation", {"Bypass", "Sum", "Multiply", "Mean", "Diff", "Max", "Min"}, operation));
    parameters->add(output.set("Output", {0}, {0}, {1}));
    

    
    listeners.push(in1.newListener([this](vector<float> &in){
        if(triggerIndex == 0){
            computeOutput(in);
        }
    }));
    listeners.push(in2.newListener([this](vector<float> &in){
        if(triggerIndex == 1){
            computeOutput(in);
        }
    }));
}

void vectorItemOperations::computeOutput(vector<float> &in){
    if(in1.get().size() == in2.get().size()){
        vector<float> tempOut;
        tempOut.resize(in1.get().size(), 0);
        switch(operation){
            case 0:
            {
                if(triggerIndex == 0){
                    tempOut = in1;
                }else{
                    tempOut = in2;
                }
                break;
            }
            case 1:
            {
                for(int i = 0; i < tempOut.size(); i++){
                    tempOut[i] = ofClamp(in1.get()[i] + in2.get()[i], 0, 1);
                }
                break;
            }
            case 2:
            {
                for(int i = 0; i < tempOut.size(); i++){
                    tempOut[i] = ofClamp(in1.get()[i] * in2.get()[i], 0, 1);
                }
                break;
            }
            case 3:
            {
                for(int i = 0; i < tempOut.size(); i++){
                    tempOut[i] = ofClamp((in1.get()[i] + in2.get()[i]) / 2, 0, 1);
                }
                break;
            }
            case 4:
            {
                for(int i = 0; i < tempOut.size(); i++){
                    tempOut[i] = ofClamp(abs(in1.get()[i] - in2.get()[i]), 0, 1);
                }
                break;
            }
            case 5:
            {
                for(int i = 0; i < tempOut.size(); i++){
                    tempOut[i] = max(in1.get()[i], in2.get()[i]);
                }
                break;
            }
            case 6:
            {
                for(int i = 0; i < tempOut.size(); i++){
                    tempOut[i] = min(in1.get()[i], in2.get()[i]);
                }
                break;
            }
            default:
            {
                tempOut = in1;
                break;
            }
        }
        output = tempOut;
    }
    
}
