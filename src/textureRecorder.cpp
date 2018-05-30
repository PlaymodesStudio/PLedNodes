//
//  textureRecorder.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 24/05/2018.
//
//

#include "textureRecorder.h"

textureRecorder::textureRecorder() : ofxOceanodeNodeModel("Texture Recorder"){
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    parameters->add(record.set("Record", false));
    parameters->add(autoRecLoop.set("Auto Rec Loop", false));
    parameters->add(filename.set("Filename", "recTest"));
    parameters->add(input.set("Input", nullptr));
    
    phasorIn.addListener(this, &textureRecorder::phasorInListener);
    record.addListener(this, &textureRecorder::recordListener);
    input.addListener(this, &textureRecorder::inputListener);
    oldPhasor = 0;
    frameCounter = 0;
}

void textureRecorder::phasorInListener(float &f){
    if(autoRecLoop){
        if(f < oldPhasor){
            record = !record;
        }
    }
    oldPhasor = f;
}

void textureRecorder::inputListener(ofTexture* &texture){
    if(record){
        if(texture != nullptr){
            ofImage img;
            texture->readToPixels(img.getPixels());
            img.save("recordings/" + filename.get() +  "_" + initRecordingTimestamp + "/" + filename.get() + "_" + ofToString(frameCounter, 4, '0') + ".png");
        }
        frameCounter++;
    }
    
}

void textureRecorder::recordListener(bool &b){
    if(b){
        initRecordingTimestamp = ofGetTimestampString();
        frameCounter = 0;
    }else{
        parameters->getBool("Auto Rec Loop") = false;
    }
}
