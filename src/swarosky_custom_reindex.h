//
//  swarosky_custom_reindex.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 22/12/2018.
//

#ifndef swarosky_custom_reindex_h
#define swarosky_custom_reindex_h

#include "ofxOceanodeNodeModel.h"

class swarosky_custom_reindex : public ofxOceanodeNodeModel{
public:
    swarosky_custom_reindex() : ofxOceanodeNodeModel("Swarosky Custom Reindex"){};
    ~swarosky_custom_reindex(){};
    
    void setup() override{
        parameters->add(input.set("Input s:65", {0}, {0}, {1}));
        addOutputParameterToGroupAndInfo(output.set("Output s:875", {0}, {0}, {1}));
        
        inputListener = input.newListener([this](vector<float> &vf){
            if(vf.size() == 65){
                vector<float> tempOutput(875,0);
                vector<int> indexs = {21, 24, 71, 74, 103, 147, 124, 152, 155, 162, 197, 174, 179, 189, 216, 195, 209, 236, 237, 286, 266, 276, 313, 350, 331, 336, 363, 340, 402, 379, 431, 414, 416, 451, 479, 488, 465, 529, 506, 538, 604, 556, 607, 584, 611, 561, 588, 620, 596, 598, 653, 679, 655, 685, 662, 694, 646, 647, 673, 736, 762, 714, 798, 783, 860};
                int i = 0;
                for(auto index : indexs){
                    tempOutput[index] = vf[i];
                    i++;
                }
                output = tempOutput;
            }
        });
    }
    
private:
    ofEventListener inputListener;
    
    ofParameter<vector<float>> input;
    ofParameter<vector<float>> output;
};


#endif /* swarosky_custom_reindex_h */
