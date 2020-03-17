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
        parameters->add(interpolate.set("Interpolate", false));
        parameters->add(output.set("Output", nullptr));
        listener = input.newListener([this](ofTexture* &tex){
            if(input != nullptr){
                if(inputSize.x != input.get()->getWidth() || inputSize.y != input.get()->getHeight()){
                    inputSize.x = input.get()->getWidth();
                    inputSize.y = input.get()->getHeight();
                    resizeMesh();
                }
                if(width != fbo.getWidth() || height != fbo.getHeight()){
                    resizeFbo();
                }
                fbo.begin();
                if(interpolate){
                    input.get()->bind();
                    mesh.draw();
                    input.get()->unbind();
                }else{
                    input.get()->draw(0,0, width, height);
                }
                fbo.end();
                output = &fbo.getTexture();
            }
        });
        
        
        inputSize = glm::vec2(0,0);
        resizeFbo();
    }
private:
    
    void resizeFbo(){
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
        resizeMesh();
    }
    
    void resizeMesh(){
//        auto rect = ofRectangle(0, 0, 1, 1);
//        auto scaledRect = ofRectangle(0, 0, width, height);
//        mesh.addTexCoord(rect.getTopLeft());
//        mesh.addTexCoord(rect.getTopRight());
//        mesh.addTexCoord(rect.getBottomRight());
//        mesh.addTexCoord(rect.getTopLeft());
//        mesh.addTexCoord(rect.getBottomRight());
//        mesh.addTexCoord(rect.getBottomLeft());
//
//        mesh.addVertex(scaledRect.getTopLeft());
//        mesh.addVertex(scaledRect.getTopRight());
//        mesh.addVertex(scaledRect.getBottomRight());
//        mesh.addVertex(scaledRect.getTopLeft());
//        mesh.addVertex(scaledRect.getBottomRight());
//        mesh.addVertex(scaledRect.getBottomLeft());
        
        int itemW = width / (inputSize.x-1);
        int itemH = height / inputSize.y;
        ofLog() << itemW << " " << itemH;
        //for(int i = 0 ; i < inputSize.y; i++){
            for(int j = 0; j < inputSize.x-1 ; j++){
                int x = j*itemW;
//                int y = i*itemH;

                float xx = (j+0.5)/inputSize.x;
//                float yy = (i+0.5)/inputSize.y;
                
                addSquareToMesh(ofRectangle(xx, 0, 1/float(inputSize.x), 1), ofRectangle(x, 0, itemW, height));
            }
//        }
    }
    
    void addSquareToMesh(ofRectangle inRect, ofRectangle outRect){
        mesh.addTexCoord(inRect.getTopLeft());
        mesh.addTexCoord(inRect.getTopRight());
        mesh.addTexCoord(inRect.getBottomRight());
        mesh.addTexCoord(inRect.getTopLeft());
        mesh.addTexCoord(inRect.getBottomRight());
        mesh.addTexCoord(inRect.getBottomLeft());
        
        mesh.addVertex(outRect.getTopLeft());
        mesh.addVertex(outRect.getTopRight());
        mesh.addVertex(outRect.getBottomRight());
        mesh.addVertex(outRect.getTopLeft());
        mesh.addVertex(outRect.getBottomRight());
        mesh.addVertex(outRect.getBottomLeft());
    }
    
    ofParameter<ofTexture*> input;
    ofParameter<bool> interpolate;
    ofParameter<int> width;
    ofParameter<int> height;
    ofParameter<ofTexture*> output;
    
    ofEventListener listener;
    
    ofFbo fbo;
    
    ofVboMesh mesh;
    glm::vec2 inputSize;
};

#endif /* textureResize_h */
