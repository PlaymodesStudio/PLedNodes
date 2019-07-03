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
    if(texture != nullptr){
        if(!recorderIsSetup || texture->getWidth() != width || texture->getHeight() != height){
            width = texture->getWidth();
            height = texture->getHeight();
            fbo.allocate(width, height, GL_RGB);
            ofxTextureRecorder::Settings settings(fbo.getTexture());
            settings.imageFormat = OF_IMAGE_FORMAT_PNG;
            settings.textureInternalFormat = GL_RGB32F;
            settings.numThreads = 6;
            settings.maxMemoryUsage = 9000000000;
            settings.folderPath = "recordings/" + filename.get() +  "_" + initRecordingTimestamp + "/";
            if(recorder != nullptr){
                delete recorder;
            }
            recorder = new ofxTextureRecorder();
            recorder->setup(settings);
            recorderIsSetup = true;
        }
        if(record){
            if(texture != nullptr){
//                            ofImage img;
//                            texture->readToPixels(img.getPixels());
//                            img.save("recordings/" + filename.get() +  "_" + initRecordingTimestamp + "/" + filename.get() + "_" + ofToString(frameCounter, 4, '0') + ".png");
                fbo.begin();
                texture->draw(0,0);
                fbo.end();
                recorder->save(fbo.getTexture(), frameCounter);
//
                recorder->save(*texture, frameCounter);
            }
            frameCounter++;
        }
    }
}

void textureRecorder::recordListener(bool &b){
    if(b){
        recorder->setFolderPath("recordings/" + filename.get() +  "_" + ofGetTimestampString() + "/");
        initRecordingTimestamp = ofGetTimestampString();
        frameCounter = 0;
    }else{
        parameters->getBool("Auto Rec Loop") = false;
    }
}
