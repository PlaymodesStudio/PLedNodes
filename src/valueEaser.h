//
//  valueEaser.h
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 05/07/2018.
//

#ifndef valueEaser_h
#define valueEaser_h

#include "ofxOceanodeNodeModel.h"

class valueEaser : public ofxOceanodeNodeModel{
public:
    valueEaser();
    ~valueEaser(){};
    
private:
    void customPow(float & value, float pow);
    float smoothinterpolate(float start, float end, float pos);
    
    auto getValueForPosition(const vector<float> &param, int index) -> float{
        if(param.size() == 1 || param.size() <= index){
            return param[0];
        }
        else{
            return param[index];
        }
    };
    
    ofParameter<vector<float>> input;
    ofParameter<vector<float>> phasor;
    ofParameter<vector<float>> pow;
    ofParameter<vector<float>> bipow;
    ofParameter<vector<float>> output;
    
    ofEventListener listener;
    vector<float> lastInput;
    vector<float> lastChangedValue;
    vector<float> phasorValueOnValueChange;
    vector<float> lastPhase;
    vector<bool> reachedMax;
};

#endif /* valueEaser_h */
