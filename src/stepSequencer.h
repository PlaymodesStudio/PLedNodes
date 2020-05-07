//
//  stepSequencer.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 07/08/2019.
//

#ifndef stepSequencer_h
#define stepSequencer_h

class stepSequencer : public ofxOceanodeNodeModel{
public:
    stepSequencer() : ofxOceanodeNodeModel("Step Sequencer"){};
    ~stepSequencer(){};
    
    void setup(){
        addParameter(numSteps.set("Num Steps", 1, 1, 100));
        addParameter(index.set("Index", 0, 0, 1));
        lastNumSteps = 0;
        
        addParameter(output.set("Output", {0}, {0}, {1}));
        addParameter(stepsVec.set("Steps Vec", {0}, {0}, {1}));
        
        numStepsListener = numSteps.newListener([this](int &nsteps){
            if(lastNumSteps == nsteps) return;
            else if(lastNumSteps > nsteps){
                for(int i = nsteps ; i < lastNumSteps; i++){
                    getParameterGroup().remove(steps[i]);
                    sliderListeners.unsubscribe(i);
                }
                steps.resize(nsteps);
                vector<float> tempSteps = stepsVec;
                tempSteps.resize(nsteps);
                stepsVec = tempSteps;
            }else{
                steps.resize(nsteps);
                vector<float> tempSteps = stepsVec;
                tempSteps.resize(nsteps);
                stepsVec = tempSteps;
                
//                parameters->remove(output);
//                parameters->remove(stepsVec);
                
                for(int i = lastNumSteps; i < numSteps; i++){
                    addParameter(steps[i].set("Step " + ofToString(i), 0, 0, 1));
                    sliderListeners.push(steps[i].newListener([this, i](float &f){
                        vector<float> tempOutput = stepsVec;
                        tempOutput[i] = f;
                        stepsVec = tempOutput;
                        if(index == i) index = i;
                    }));
                    steps[i] = 0;
                }
                
//                parameterGroupChanged.notify();
//
//                addParameterToGroupAndInfo(output);
//                addParameterToGroupAndInfo(stepsVec);
            }
            parameterGroupChanged.notify();
            
            string parameterName = index.getName();
            index.setMax(numSteps-1);
            index.set(0);
            parameterChangedMinMax.notify(parameterName);
            
            lastNumSteps = nsteps;
        });
        numSteps = 4;
        
        
        indexListener = index.newListener([this](int &ind){
            ind %= numSteps;
            vector<float> tempOut(numSteps, 0);
            tempOut[ind] = stepsVec.get()[ind];
            output = tempOut;
        });
        
        index = 0;
        
    }
    
    void presetHasLoaded(){
        vector<float> tempOut(numSteps, 0);
        for(int i = 0 ; i < numSteps; i++){
            tempOut[i] = steps[i];
        }
        stepsVec = tempOut;
        tempOut = vector<float>(numSteps, 0);
        tempOut[index] = steps[index];
        output = tempOut;
    }
    
private:
    ofParameter<int> index;
    ofParameter<int> numSteps;
    vector<ofParameter<float>> steps;
    ofParameter<vector<float>> output;
    ofParameter<vector<float>> stepsVec;
    
    int lastNumSteps;
    ofEventListener numStepsListener;
    ofEventListener indexListener;
    ofEventListeners sliderListeners;
};

#endif /* stepSequencer_h */
