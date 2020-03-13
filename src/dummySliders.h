//
//  dummySliders.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 10/03/2020.
//

#ifndef dummySliders_h
#define dummySliders_h

#include "ofxOceanodeNodeModel.h"

class dummySliders : public ofxOceanodeNodeModel{
public:
    dummySliders() : ofxOceanodeNodeModel("Dummy Sliders"){
        color = ofColor::violet;
    };
    ~dummySliders(){};
    
    void update(ofEventArgs &args){
        vector<float> tempChain;
        for(int i = 0; i < sliders.size(); i++){
            tempChain.insert(tempChain.end(), sliders[i]->begin(), sliders[i]->end());
        }
        chain = tempChain;
    }
    
    void setup(){
        int numSliders = 16;
        sliders.resize(numSliders);
        for(int i = 0; i < numSliders; i++){
            parameters->add(sliders[i].set(ofToString(i), {0}, {0}, {1}));
        }
        parameters->add(chain.set("Chain", {0}, {0}, {1}));
    }
    
    
private:
    vector<ofParameter<vector<float>>> sliders;
    ofParameter<vector<float>> chain;
};

#endif /* dummySliders_h */
