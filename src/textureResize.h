//
//  textureResize.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 10/03/2020.
//

#ifndef textureResize_h
#define textureResize_h

#include "ofxOceanodeNodeModel.h"

class textureResize : public ofxOceanodeNodeModel {
public:
    textureResize() : ofxOceanodeNodeModel("Texture Resizer"){};
    ~textureResize(){};
    
    void setup() override{
        parameters->add(input.set("Input", nullptr));
        parameters->add(width.set("Width", 100, 1, 5120));
        parameters->add(height.set("Height", 100, 1, 2880));
        parameters->add(output.set("Output", nullptr));
        listener = input.newListener([this](ofTexture* &tex){
            if(input != nullptr){
                if(width != fbo.getWidth() || height != fbo.getHeight()){
                    ofFbo::Settings settings;
                    settings.width = width;
                    settings.height = height;
                    settings.internalformat = GL_RGBA32F;
                    settings.maxFilter = GL_NEAREST;
                    settings.minFilter = GL_NEAREST;
                    settings.numColorbuffers = 1;
                    settings.useDepth = false;
                    settings.useStencil = false;
                    settings.textureTarget = GL_TEXTURE_2D;
                    
                    fbo.allocate(settings);
                    fbo.begin();
                    ofClear(0, 0, 0, 255);
                    fbo.end();
                }
                fbo.begin();
                input.get()->draw(0,0, width, height);
                fbo.end();
                output = &fbo.getTexture();
            }
        });
        
        ofFbo::Settings settings;
        settings.width = width;
        settings.height = height;
        settings.internalformat = GL_RGBA32F;
        settings.maxFilter = GL_NEAREST;
        settings.minFilter = GL_NEAREST;
        settings.numColorbuffers = 1;
        settings.useDepth = false;
        settings.useStencil = false;
        settings.textureTarget = GL_TEXTURE_2D;
        
        fbo.allocate(settings);
        fbo.begin();
        ofClear(0, 0, 0, 255);
        fbo.end();
    }
private:
    
    ofParameter<ofTexture*> input;
    ofParameter<int> width;
    ofParameter<int> height;
    ofParameter<ofTexture*> output;
    
    ofEventListener listener;
    
    ofFbo fbo;
};

#endif /* textureResize_h */
