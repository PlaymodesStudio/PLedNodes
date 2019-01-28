//
//  textureUnifier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 22/09/2017.
//
//

#include "textureUnifier.h"

textureUnifier::textureUnifier() : ofxOceanodeNodeModel("Texture Unifier"){
    
}

void textureUnifier::setup(){
    parameters->add(triggerTextureIndex.set("Trigger Index", 0, 0, 5));
    parameters->add(spacing.set("Tex Spacing", 1, 0, 10));
    inputs.resize(5);
    for(int i = 0; i < inputs.size() ; i++){
        parameters->add(inputs[i].set("Input " + ofToString(i), nullptr));
        listeners.push(inputs[i].newListener(this, &textureUnifier::computeOutput));
    }
    parameters->add(output.set("Output", nullptr));
}

void textureUnifier::computeOutput(ofTexture* &in){
    if(in != nullptr){
        if(&in == &inputs[triggerTextureIndex].get()){
            int totalHeight = -spacing;
            int maxWidth = 0;
            for(auto t : inputs){
                if(t != nullptr){
                    totalHeight += t.get()->getHeight() + spacing;
                    if(t.get()->getWidth() > maxWidth){
                        maxWidth = t.get()->getWidth();
                    }
                }
            }
            if(outputFbo.getHeight() != totalHeight || outputFbo.getWidth() != maxWidth || !outputFbo.isAllocated()){
                if(totalHeight != 0 && maxWidth != 0){  
//                    outputFbo.allocate(maxWidth, totalHeight, GL_RGBA32F);
//                    outputFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
                    
                    outputFbo.clear();
    
                    ofFbo::Settings settings;
                    settings.height = totalHeight;
                    settings.width = maxWidth;
                    settings.internalformat = GL_RGBA32F;
                    settings.maxFilter = GL_NEAREST;
                    settings.minFilter = GL_NEAREST;
                    settings.numColorbuffers = 1;
                    settings.useDepth = false;
                    settings.useStencil = false;
                    
                    outputFbo.allocate(settings);
                    outputFbo.begin();
                    ofClear(255, 255, 255, 0);
                    outputFbo.end();
                }else{
                    return;
                }
            }
            
            outputFbo.begin();
            ofClear(0, 0, 0);
            int currentLine = 0;//spacing;
            for(auto t : inputs){
                if(t != nullptr){
                    t.get()->draw(0, currentLine);
                    currentLine += t.get()->getHeight() + spacing;
                }
            }
            outputFbo.end();
            
            output = &outputFbo.getTexture();
        }
    }
}
