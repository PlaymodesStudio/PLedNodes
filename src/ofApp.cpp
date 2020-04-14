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
#include "valueEaser.h"
#include "envelopeGenerator.h"
#include "manualOscillatorBank.h"
#include "textureMixer.h"
//#include "artnetSender.h"
//#include "texturePixelmap.h"
//#include "dviOutput.h"
#include "midiGateIn.h"
#include "sequentialAnalyzer.h"
#include "delta.h"
//#include "SnowQueenTheFrameTextureRemapper.h"
#include "oscTextureSender.h"
#include "euclideanGenerator.h"
#include "vectorToTexture.h"
#include "thresholdBang.h"
#include "stepSequencer.h"
#include "indexerTexture.h"
#include "chaoticOscillatorTexture.h"
#include "textureResize.h"
#include "dummySliders.h"
#include "sharedInfo.h"

#include "ofxOceanodeMidiController.h"
#include "ofxOceanodeBPMController.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofEnableGLDebugLog();
    
    ofDisableArbTex();
    ofSetVerticalSync(false);
    
//    openedJson = "";
//    ofFile file;
//    string path;
//    if(file.doesFileExist(ofToDataPath("../GIANT.generator"))) {
//        path = ofToDataPath("../GIANT.generator");
//    }else{
//        auto result = ofSystemLoadDialog("Select Generator File", false, ofToDataPath("../"));
//        path = result.getPath();
//    }
//    if(ofSplitString(path, ".").back() != "generator"){
//        ofExit();
//    }
//    float bpm = 120;
//
//    ofJson json = ofLoadJson(path);
//
//    ofRectangle controlsWindowRect(0,0,0,0);
//    if(!json.empty()){
//        openedJson = path;
//
//        string name = json["Name"];
//        ofSetWindowTitle(name);
//        bpm = json["BPM"];
//        int frameRate = json["FPS"];
//        ofSetFrameRate(frameRate);
//        if (frameRate == 60) ofSetVerticalSync(true);
//
//        ofJson windowJson = json["Window"];
//        //Main Window
//        ofRectangle mainWindowRect(0,0,0,0);
//        if(windowJson.count("MainWindowX") == 1){
//            mainWindowRect.x = windowJson["MainWindowX"];
//        }
//        if(windowJson.count("MainWindowY") == 1){
//            mainWindowRect.y = windowJson["MainWindowY"];
//        }
//        if(windowJson.count("MainWindowWidth") == 1){
//            mainWindowRect.width = windowJson["MainWindowWidth"];
//        }
//        if(windowJson.count("MainWindowHeight") == 1){
//            mainWindowRect.height = windowJson["MainWindowHeight"];
//        }
//        if(mainWindowRect.width != 0 && mainWindowRect.height != 0){
//            ofSetWindowPosition(mainWindowRect.x, mainWindowRect.y);
//            ofSetWindowShape(mainWindowRect.width, mainWindowRect.height);
//        }
//
//        //Controls Window
//        if(windowJson.count("ControlsWindowX") == 1){
//            controlsWindowRect.x = windowJson["ControlsWindowX"];
//        }
//        if(windowJson.count("ControlsWindowY") == 1){
//            controlsWindowRect.y = windowJson["ControlsWindowY"];
//        }
//        if(windowJson.count("ControlsWindowWidth") == 1){
//            controlsWindowRect.width = windowJson["ControlsWindowWidth"];
//        }
//        if(windowJson.count("ControlsWindowHeight") == 1){
//            controlsWindowRect.height = windowJson["ControlsWindowHeight"];
//        }
//
//        //Scope Window
//        ofRectangle scopeWindowRect(0,0,0,0);
//        if(windowJson.count("ScopeWindowX") == 1){
//            scopeWindowRect.x = windowJson["ScopeWindowX"];
//        }
//        if(windowJson.count("ScopeWindowY") == 1){
//            scopeWindowRect.y = windowJson["ScopeWindowY"];
//        }
//        if(windowJson.count("ScopeWindowWidth") == 1){
//            scopeWindowRect.width = windowJson["ScopeWindowWidth"];
//        }
//        if(windowJson.count("ScopeWindowHeight") == 1){
//            scopeWindowRect.height = windowJson["ScopeWindowHeight"];
//        }
//        if(scopeWindowRect.width != 0 && scopeWindowRect.height != 0){
//            sharedInfo::getInstance().setRect("ScopeWindowRect", scopeWindowRect);
//        }
//
////        int unifierNum = json["TextureUnifierNumber"];
////        int unifierSpacing = json["TextureUnifierSpacing"];
////            if(unifierNum > 0){
////                new textureUnifier(unifierNum, unifierSpacing);
////            }
//
////        if(json["TextureRecorder"]){
//            //                new dataRecorder();
////        }
//
//        if(json.count("IP")){
//            string ip = json["IP"];
//            sharedInfo::getInstance().setString("IP", ip);
//        }else{
//            string ip = "127.0.0.1";
//            sharedInfo::getInstance().setString("IP", ip);
//        }
//    }
    sharedInfo::getInstance().setString("IP", "127.0.0.1");
    
    oceanode.registerModel<oscillatorTexture>("LedNodes");
    oceanode.registerModel<waveScope>("LedNodes");
    oceanode.registerModel<senderManager>("LedNodes");
    oceanode.registerModel<colorApplier>("LedNodes");
    oceanode.registerModel<textureRecorder>("LedNodes");
    oceanode.registerModel<scriptModule>("Scripting");
    oceanode.registerModel<textureUnifier>("LedNodes");
    oceanode.registerModel<audioEngineController>("Audio");
    oceanode.registerModel<textureReader>("LedNodes");
    oceanode.registerModel<valueEaser>("EASER");
    oceanode.registerModel<manualOscillatorBank>("Generators");
    oceanode.registerModel<envelopeGenerator>("Midi");
    oceanode.registerModel<textureMixer>("LedNodes");
    //oceanode.registerModel<artnetSender>("ARTNET");
    //oceanode.registerModel<texturePixelmap>("ARTNET");
//    oceanode.registerModel<dviOutput>("OUTPUT");
    oceanode.registerModel<midiGateIn>("Midi");
    oceanode.registerModel<sequentialAnalyzer>("Midi");
    oceanode.registerModel<delta>("OP");
    //oceanode.registerModel<SnowQueenTheFrameTextureRemapper>("LedNodes");
    oceanode.registerModel<oscTextureSender>("Audio");
    oceanode.registerModel<euclideanGenerator>("Generators");
    oceanode.registerModel<vectorToTexture>("LedNodes");
    oceanode.registerModel<thresholdBang>("Modulator");
    oceanode.registerModel<stepSequencer>("Generators");
    oceanode.registerModel<indexerTexture>("LedNodes");
    oceanode.registerModel<chaoticOscillatorTexture>("LedNodes");
    oceanode.registerModel<textureResize>("LedNodes");
    oceanode.registerModel<dummySliders>("Sliders");
    
    oceanode.registerModel<subDimensionCombinator>("Vector Operations");
    oceanode.registerModel<vectorChain>("Vector Operations");
    oceanode.registerModel<vectorGetter>("Vector Operations");
    oceanode.registerModel<vectorOperations>("Vector Operations");
    oceanode.registerModel<vectorItemOperations>("Vector Operations");
    
    
    oceanode.registerType<ofTexture*>();
    
    
//    container = make_shared<ofxOceanodeContainer>(reg, treg);
//    canvas.setContainer(container);
//    canvas.setup();
//
//    controls = make_shared<ofxOceanodeControls>(container);
//    controls->get<ofxOceanodeBPMController>()->setBPM(bpm);
//    container->loadPersistent();
    //container->loadPreset("Presets/Bank_2019-01-20-14-33-32-088/2--cyclo2");
    
//    if(controlsWindowRect.width != 0 && controlsWindowRect.height != 0){
//        controls->setWindowRect(controlsWindowRect);
//    }
    oceanode.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    if(ofGetFrameNum() > 200 && ofGetFrameRate() < 10){
        //ofExit();
    }
    oceanode.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
//    if(controls->get<ofxOceanodeMidiController>()->getIsMidiLearn()){
//        ofSetBackgroundColor(150, 200, 150);
//    }else{
//        ofSetBackgroundColor(127, 127, 127);
//    }
    ofDrawBitmapString(ofGetFrameRate(), 10, 10);
    oceanode.draw();
}

void ofApp::exit(){
    //container->clearContainer();
//    if(openedJson != ""){
//        auto &sharedInfoInstance = sharedInfo::getInstance();
//        ofJson json = ofLoadJson(openedJson);
//        ofJson windowJson;
//        //Main Window
//        ofRectangle mainWinRect = ofGetWindowRect();
//        mainWinRect.setPosition(ofGetWindowPositionX(), ofGetWindowPositionY());
//        windowJson["MainWindowX"] = mainWinRect.x;
//        windowJson["MainWindowY"] = mainWinRect.y;
//        windowJson["MainWindowWidth"] = mainWinRect.width;
//        windowJson["MainWindowHeight"] = mainWinRect.height;
//
//        //Controls Window
////        ofRectangle controlsWinRect = controls->getWindowRect();
////        windowJson["ControlsWindowX"] = controlsWinRect.x;
////        windowJson["ControlsWindowY"] = controlsWinRect.y;
////        windowJson["ControlsWindowWidth"] = controlsWinRect.width;
////        windowJson["ControlsWindowHeight"] = controlsWinRect.height;
//
//        //Texture Scope 1
//        ofRectangle scopeWinRect = sharedInfoInstance.getRect("ScopeWindowRect");
//        windowJson["ScopeWindowX"] = scopeWinRect.x;
//        windowJson["ScopeWindowY"] = scopeWinRect.y;
//        windowJson["ScopeWindowWidth"] = scopeWinRect.width;
//        windowJson["ScopeWindowHeight"] = scopeWinRect.height;
//
//        json["Window"] = windowJson;
//
//        ofSavePrettyJson(openedJson, json);
//    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    /*
#ifdef TARGET_OSX
    if(ofGetKeyPressed(OF_KEY_COMMAND)){
#else
    if(ofGetKeyPressed(OF_KEY_CONTROL)){
#endif
        if(key == 'p') container->savePersistent();
        else if(key == 'u') container->updatePersistent();
//        else if(key == 'k') container->collapseGuis();
//        else if(key == 'e') container->expandGuis();
        else if(key == 's') container->saveCurrentPreset();
        else if(key == 'r') container->resetPhase();
//        else if(key == 'l') controls->get<ofxOceanodeMidiController>()->setIsMidiLearn(!controls->get<ofxOceanodeMidiController>()->getIsMidiLearn());
    }else if(key == 'l'){
        //container->loadPreset("Presets/Bank_2019-01-20-14-33-32-088/1--cyclo");
    }
        */
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
