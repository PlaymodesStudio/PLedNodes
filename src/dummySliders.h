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
    dummySliders() : ofxOceanodeNodeModel("Dummy Sliders"){};
    ~dummySliders(){};
    
    void setup(){
        int numSliders = 16;
        sliders.resize(numSliders);
        for(int i = 0; i < numSliders; i++){
            parameters->add(sliders[i].set(ofToString(i), {0}, {0}, {1}));
        }
    }
    
    
private:
    vector<ofParameter<vector<float>>> sliders;
};

#endif /* dummySliders_h */
