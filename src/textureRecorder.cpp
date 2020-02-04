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
    
    listeners.push(phasorIn.newListener(this, &textureRecorder::phasorInListener));
    listeners.push(record.newListener(this, &textureRecorder::recordListener));
    listeners.push(input.newListener(this, &textureRecorder::inputListener));
    oldPhasor = 0;
    frameCounter = 0;
    recorderIsSetup = false;
    recorder = nullptr;
    lastFrame = false;
}

void textureRecorder::phasorInListener(float &f){
    if(autoRecLoop){
        if(f < oldPhasor){
            if(!record) record = true;
            else lastFrame = true;
        }
    }
    oldPhasor = f;
}

void textureRecorder::draw(ofEventArgs &a){

}

void textureRecorder::inputListener(ofTexture* &texture){
    if(input != nullptr){
        if(!recorderIsSetup || input.get()->getWidth() != width || input.get()->getHeight() != height){
            width = input.get()->getWidth();
            height = input.get()->getHeight();
            fbo.allocate(width, height, GL_RGB);
            fbo.begin();
            ofClear(0);
            input.get()->draw(0,0);
            fbo.end();
            ofxTextureRecorder::Settings settings(fbo.getTexture());
            settings.imageFormat = OF_IMAGE_FORMAT_PNG;
            settings.textureInternalFormat = GL_RGB32F;
//            settings.numThreads = 4;
            settings.maxMemoryUsage = 9000000000;
            settings.folderPath = "recordings/" + filename.get() +  "_" + initRecordingTimestamp + "/" + filename.get() + "_";
            if(recorder != nullptr){
                delete recorder;
            }
            recorder = new ofxTextureRecorder();
            recorder->setup(settings);
            recorderIsSetup = true;
        }
        if(record){
            if(input != nullptr){
                fbo.begin();
                input.get()->draw(0,0);
                fbo.end();
                recorder->save(fbo.getTexture(), frameCounter);
            }
            frameCounter++;
            if(lastFrame){
                record = false;
                lastFrame = false;
            }
        }
    }
}

void textureRecorder::recordListener(bool &b){
    if(b){
        if(recorder == nullptr){
            record = false;
            return;
        }
        recorder->setFolderPath("recordings/" + filename.get() +  "_" + ofGetTimestampString() + "/" + filename.get() + "_");
        initRecordingTimestamp = ofGetTimestampString();
        frameCounter = 0;
    }else{
        parameters->getBool("Auto Rec Loop") = false;
        recorderIsSetup = false;
    }
}
