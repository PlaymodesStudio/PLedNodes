//
//  senderManager.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#ifndef senderManager_h
#define senderManager_h

#include "ofxOceanodeNodeModel.h"
//#include "ofxSyphon.h"

class senderManager : public ofxOceanodeNodeModel{
public:
    
    senderManager();
    ~senderManager(){
        //delete syphonServer;
    };
    void setname(string name){syphonName = name;};
    
    void enableSyphonListener(bool &b);
    void syphonNameListener(string &s);
    
private:
    void sendTexture(ofTexture *&info);
    
    //ofxSyphonServer*   syphonServer;

    ofParameter<bool>   enable;
    ofParameter<string> syphonName;
    ofParameter<float>  masterFader;
    ofParameter<ofTexture*>    textureIn;
    
    ofFbo colorFbo;
    
    bool invert;
    
    ofEventListeners listeners;
};

#endif /* senderManager_h */
