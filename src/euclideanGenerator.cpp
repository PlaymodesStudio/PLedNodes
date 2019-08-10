//
//  euclideanGenerator.cpp

//
//  Created by Eduard Frigola on 25/08/2018.
//
//

#include "euclideanGenerator.h"
#include <algorithm>


euclideanGenerator::euclideanGenerator() : ofxOceanodeNodeModel("Euclidean Generator"){
    parameters->add(input.set("Input", 0, 0, 1));
    parameters->add(onsets.set("Onsets", 1, 1, 32));
    parameters->add(divisions.set("Divisions", 1, 1, 32));
    parameters->add(offset.set("Offset", 0, 0, 1));
    parameters->add(output.set("Output", {0}, {0}, {1}));
    parameters->add(outputBool.set("Bool Output", false));
    parameters->add(outputSteps.set("Steps Output", {0}, {0}, {1}));
    parameters->add(outputIndex.set("Current Index", 0, 0, 1));
    listeners.push(input.newListener(this, &euclideanGenerator::computeOutput));
    listeners.push(onsets.newListener(this, &euclideanGenerator::computeAlgorithm));
    listeners.push(divisions.newListener(this, &euclideanGenerator::computeAlgorithm));
}

void euclideanGenerator::computeOutput(float &f){
    if(offset != 0){
        f += offset*(1.0/divisions);
        f = f - int(f);
    }
    outputBool = sequence[int(f/ (1.0/divisions))];
    vector<float> tempOutput(onsets, 0);
    for(int i = 1; i < onsets+1; i++){
        if(f < divisionPositions[i] && f > divisionPositions[i-1]){
            tempOutput[i-1] = ofMap(f, divisionPositions[i-1], divisionPositions[i], 0, 1, true);
            outputIndex = i-1;
        }else{
            tempOutput[i-1] = 0;
        }
    }
    output = tempOutput;
}

void euclideanGenerator::computeAlgorithm(int &i){
    if(offset.getMax() != divisions){
        string offsetName = offset.getName();
        offset.setMax(divisions);
        offset.set(0);
        parameterChangedMinMax.notify(this, offsetName);
    }
    if(outputIndex.getMax() != onsets-1){
        string outputindexName = outputIndex.getName();
        outputIndex.setMax(onsets-1);
        outputIndex.set(0);
        parameterChangedMinMax.notify(this, outputindexName);
    }
    
//    ofLog() << "NEW SEQ";
    if(onsets <= divisions){
        vector<vector<bool>> ones(onsets, vector<bool>(1, true));
        vector<vector<bool>> zeros(divisions-onsets, vector<bool>(1, false));
        ones.insert(ones.end(), zeros.begin(), zeros.end());
        vector<vector<bool>> result = moveZeros(ones);
        while(result.size() != 1){
            result[0].insert(result[0].end(), result[1].begin(), result[1].end());
            result.erase(result.begin()+1);
        }
        divisionPositions.clear();
        sequence = result[0];
        for(int i = 0; i < sequence.size(); i++){
//            cout<<sequence[i];
            if(sequence[i]){
                divisionPositions.push_back((1.0/divisions) * i);
            }
        }
        divisionPositions.push_back(1);
//        cout<<endl;
    }
    vector<float> tempOutSteps(sequence.size(), 0);
    for(int i = 0; i < sequence.size(); i++) tempOutSteps[i] = sequence[i];
    outputSteps = tempOutSteps;
}

vector<vector<bool>> euclideanGenerator::moveZeros(vector<vector<bool>> in){
    for(auto v : in){
//        cout << "[";
        for(auto _v : v){
//            cout << _v;
        }
//        cout << "] ";
    }
//    cout << endl;
    bool lastSequenceHasFalse = false;
    for(auto b : in.back()) if(!b) lastSequenceHasFalse = true;
    if(in.size() == 1 || in.back() != in.at(in.size()-2) || !lastSequenceHasFalse){
        return in;
    }else{
        vector<bool> lastMovedSequence = in.back();
        int i = 0;
        while(lastMovedSequence == in.back() && in[i] != vector<bool>(1, false)){
            lastMovedSequence = in.back();
            in[i].insert(in[i].end(), lastMovedSequence.begin(), lastMovedSequence.end());
            in.pop_back();
            i++;
        }
        return moveZeros(in);
    }
}
