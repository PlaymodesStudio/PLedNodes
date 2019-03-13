#include "ofApp.h"
#include "oscillatorTexture.h"
#include "waveScope.h"
#include "senderManager.h"
#include "colorApplier.h"
#include "textureRecorder.h"
#include "scriptModule.h"
#include "textureUnifier.h"
#include "audioEngineController.h"
#include "vector_op_registrator.h"
#include "textureReader.h"
#include "swarosky_custom_reindex.h"
#include "valueEaser.h"
#include "envelopeGenerator.h"
#include "manualOscillatorBank.h"
#include "textureMixer.h"
#include "artnetSender.h"
#include "texturePixelmap.h"
#include "dviOutput.h"
#include "midiGateIn.h"
#include "sequentialAnalyzer.h"
#include "delta.h"

#include "ofxOceanodeBPMController.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofEnableGLDebugLog();
    
    ofDisableArbTex();
    ofSetVerticalSync(false);
    ofSetEscapeQuitsApp(false);
    
    ofFile file;
    string path;
    if(file.doesFileExist(ofToDataPath("../GIANT.generator"))) {
        path = ofToDataPath("../GIANT.generator");
    }else{
        auto result = ofSystemLoadDialog("Select Generator File", false, ofToDataPath("../"));
        path = result.getPath();
    }
    if(ofSplitString(path, ".").back() != "generator"){
        ofExit();
    }
    
    float bpm = 120;
    
    ofJson json = ofLoadJson(path);
    if(!json.empty()){
        string name = json["Name"];
        ofSetWindowTitle(name);
        bpm = json["BPM"];
        int frameRate = json["FPS"];
        ofSetFrameRate(frameRate);
		if (frameRate == 60) ofSetVerticalSync(true);
        
        int syphonSenders = json["Syphon"];
        
//            int numSyphonServers = xml.getIntValue("SyphonSenders");
//            for(int i = 0; i < numSyphonServers; i++){
//                bool invert = xml.getBoolValue("SyphonSender"+ ofToString(i+1) + "InvertTexture");
//                string syphonName = xml.getValue("SyphonSender"+ ofToString(i+1) + "Name");
//                senderModules.push_back(new senderManager(i+1, invert, syphonName));
//            }
//
        auto mainPos = json["MainWindowPos"];
        if(mainPos.size() == 2){
            ofSetWindowPosition(ofToInt(mainPos[0]), ofToInt(mainPos[1]));
        }

        auto mainSize = json["MainWindowSize"];
        if(mainSize.size() == 2){
            ofSetWindowShape(ofToInt(mainSize[0]), ofToInt(mainSize[1]));
        }

        auto prevPos = json["PrevWindowPos"];
        if(prevPos.size() == 2){
//            prevWinRect.setPosition(ofToInt(prevPos[0]), ofToInt(prevPos[1]));
        }
        
        auto prevSize = json["PrevWindowSize"];
        if(prevSize.size() == 2){
//            prevWinRect.setSize(ofToInt(prevSize[0]), ofToInt(prevSize[1]));
        }

        int unifierNum = json["TextureUnifierNumber"];
        int unifierSpacing = json["TextureUnifierSpacing"];
//            if(unifierNum > 0){
//                new textureUnifier(unifierNum, unifierSpacing);
//            }
        
        if(json["TextureRecorder"]){
            //                new dataRecorder();
        }
    }
    
    auto reg = make_shared<ofxOceanodeNodeRegistry>();
    auto treg = make_shared<ofxOceanodeTypesRegistry>();
    reg->registerModel<oscillatorTexture>("LedNodes");
    reg->registerModel<waveScope>("LedNodes");
    reg->registerModel<senderManager>("LedNodes");
    reg->registerModel<colorApplier>("LedNodes");
    reg->registerModel<textureRecorder>("LedNodes");
    reg->registerModel<scriptModule>("Scripting");
    reg->registerModel<textureUnifier>("LedNodes");
    reg->registerModel<audioEngineController>("Audio");
    reg->registerModel<textureReader>("LedNodes");
    reg->registerModel<swarosky_custom_reindex>("Swarosky");
    reg->registerModel<valueEaser>("EASER");
    reg->registerModel<manualOscillatorBank>("Generators");
    reg->registerModel<envelopeGenerator>("Midi");
    reg->registerModel<textureMixer>("LedNodes");
    reg->registerModel<artnetSender>("ARTNET");
    reg->registerModel<texturePixelmap>("ARTNET");
    reg->registerModel<dviOutput>("OUTPUT");
    reg->registerModel<midiGateIn>("Midi");
    reg->registerModel<sequentialAnalyzer>("Midi");
    reg->registerModel<delta>("OP");
    
    registerVectorOp(reg);
    
    treg->registerType<ofTexture*>();
    
    
    container = make_shared<ofxOceanodeContainer>(reg, treg);
    canvas.setContainer(container);
    canvas.setup();
    
    controls = make_shared<ofxOceanodeControls>(container);
    controls->get<ofxOceanodeBPMController>()->setBPM(bpm);
    
    //container->loadPreset("Presets/Bank_2019-01-20-14-33-32-088/2--cyclo2");
}

//--------------------------------------------------------------
void ofApp::update(){
    if(ofGetFrameNum() > 200 && ofGetFrameRate() < 10){
        //ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString(ofGetFrameRate(), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(ofGetKeyPressed(OF_KEY_COMMAND)){
        if(key == 'p') container->savePersistent();
        else if(key == 'u') container->updatePersistent();
        else if(key == 'c') container->collapseGuis();
        else if(key == 'e') container->expandGuis();
        else if(key == 's') container->saveCurrentPreset();
        else if(key == 'r') container->resetPhase();
    }else if(key == 'l'){
        //container->loadPreset("Presets/Bank_2019-01-20-14-33-32-088/1--cyclo");
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
