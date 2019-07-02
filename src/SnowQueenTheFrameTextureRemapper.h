//
//  SnowQueenTheFrameTextureRemapper.h
//  PLedNodes
//
//  Created by Eduard Frigola on 04/05/2019.
//

#ifndef SnowQueenTheFrameTextureRemapper_h
#define SnowQueenTheFrameTextureRemapper_h

#include "ofxOceanodeNodeModel.h"

class SnowQueenTheFrameTextureRemapper : public ofxOceanodeNodeModel {
public:
    SnowQueenTheFrameTextureRemapper() : ofxOceanodeNodeModel("Texture Remapper"){};
    ~SnowQueenTheFrameTextureRemapper(){};
    
    void setup() override{
        parameters->add(input.set("Input", nullptr));
        parameters->add(output.set("Output", nullptr));
        listener = input.newListener([this](ofTexture* &tex){
            if(input != nullptr){
                fbo.begin();
                input.get()->bind();
                mesh.draw();
                input.get()->unbind();
                fbo.end();
                output = &fbo.getTexture();
            }
        });
        
        addRemapingLine(354, 0);
        addRemapingLine(321, 1);
        addRemapingLine(291, 2);
        addRemapingLine(283, 3);
        addRemapingLine(257, 4);
        addRemapingLine(234, 5);
        addRemapingLine(229, 6);
        addRemapingLine(210, 7);
        addRemapingLine(193, 8);
        
//        addSquareToMesh(ofRectangle(0, 0, 355, 1), ofRectangle(0, 0, 355, 1));
//        addSquareToMesh(ofRectangle(355, 0, 355, 1), ofRectangle(0, 1, 355, 1));
//        addSquareToMesh(ofRectangle(355*2, 0, 355, 1), ofRectangle(0, 2, 355, 1));
//        addSquareToMesh(ofRectangle(355*3, 0, 355, 1), ofRectangle(0, 3, 355, 1));
        
        
        
        ofFbo::Settings settings;
        settings.width = 354;
        settings.height = 36;
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
    void addRemapingLine(float size, float y){
        addSquareToMesh(ofRectangle(0, y, size, 1), ofRectangle(0, 0+(y*4), size, 1));
        addSquareToMesh(ofRectangle(size, y, size, 1), ofRectangle(0, 1+(y*4), size, 1));
        addSquareToMesh(ofRectangle(size*2, y, size, 1), ofRectangle(0, 2+(y*4), size, 1));
        addSquareToMesh(ofRectangle(size*3, y, size, 1), ofRectangle(0, 3+(y*4), size, 1));
    }
    
    void addSquareToMesh(ofRectangle inRect, ofRectangle outRect){
        glm::vec3 dimensions(1416.0, 9.0, 1);
        mesh.addTexCoord(inRect.getTopLeft()/dimensions);
        mesh.addTexCoord(inRect.getTopRight()/dimensions);
        mesh.addTexCoord(inRect.getBottomRight()/dimensions);
        mesh.addTexCoord(inRect.getTopLeft()/dimensions);
        mesh.addTexCoord(inRect.getBottomRight()/dimensions);
        mesh.addTexCoord(inRect.getBottomLeft()/dimensions);
        
        mesh.addVertex(outRect.getTopLeft());
        mesh.addVertex(outRect.getTopRight());
        mesh.addVertex(outRect.getBottomRight());
        mesh.addVertex(outRect.getTopLeft());
        mesh.addVertex(outRect.getBottomRight());
        mesh.addVertex(outRect.getBottomLeft());
    }
    
    ofParameter<ofTexture*> input;
    ofParameter<ofTexture*> output;
    
    ofEventListener listener;
    
    ofVboMesh mesh;
    ofFbo fbo;
};

#endif /* SnowQueenTheFrameTextureRemapper_h */
