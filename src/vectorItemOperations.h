//
//  vectorItemOperations.h
//  Espills_controller
//
//  Created by Eduard Frigola on 01/08/2018.
//

#ifndef vectorItemOperations_h
#define vectorItemOperations_h

#include "ofxOceanodeNodeModel.h"

class vectorItemOperations : public ofxOceanodeNodeModel{
public:
    vectorItemOperations();
    ~vectorItemOperations(){};
    
private:
    void computeOutput(vector<float> &in);
    ofEventListeners listeners;
    
    ofParameter<vector<float>>  in1;
    ofParameter<vector<float>>  in2;
    ofParameter<int> triggerIndex;
    ofParameter<int>   operation;
    ofParameter<vector<float>>   output;
};

#endif /* vectorItemOperations_hpp */
