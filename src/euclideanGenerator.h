//
//  euclideanGenerator.h
//
//
//  Created by Eduard Frigola on 25/08/2018.
//
//

#ifndef euclidianGenerator_h
#define euclidianGenerator_h

#include "ofxOceanodeNodeModel.h"

class euclideanGenerator : public ofxOceanodeNodeModel{
public:
    euclideanGenerator();
    ~euclideanGenerator(){};
    
private:
    void computeOutput(float &f);
    void computeAlgorithm(int &i);
    ofEventListeners listeners;
    
    vector<vector<bool>> moveZeros(vector<vector<bool>> in);
    
    ofParameter<float>  input;
    ofParameter<int>    onsets;
    ofParameter<int>    offset;
    ofParameter<int>    divisions;
    ofParameter<vector<float>>  output;
    ofParameter<bool> outputBool;
    ofParameter<int> outputIndex;
    ofParameter<vector<float>> outputSteps;
    
    vector<bool> sequence;
    vector<float> divisionPositions;
};

#endif /* euclideanGenerator_h */
