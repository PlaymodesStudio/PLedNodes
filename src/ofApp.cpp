#include "ofApp.h"
#include "oscillatorTexture.h"
#include "waveScope.h"
#include "senderManager.h"
#include "colorApplier.h"
#include "textureRecorder.h"
#include "scriptModule.h"
#include "vector_op_registrator.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofSetVerticalSync(true);
    ofSetFrameRate(33);
    
    auto reg = make_shared<ofxOceanodeNodeRegistry>();
    auto treg = make_shared<ofxOceanodeTypesRegistry>();
    reg->registerModel<oscillatorTexture>("LedNodes");
    reg->registerModel<waveScope>("LedNodes");
    reg->registerModel<senderManager>("LedNodes");
    reg->registerModel<colorApplier>("LedNodes");
    reg->registerModel<textureRecorder>("LedNodes");
    reg->registerModel<scriptModule>("Scripting");
    registerVectorOp(reg);
    
    treg->registerType<ofTexture*>();
    
    
    container = make_shared<ofxOceanodeContainer>(reg, treg);
    canvas.setContainer(container);
    canvas.setup();
    
    controls = new ofxOceanodeControls(container);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString(ofGetFrameRate(), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
