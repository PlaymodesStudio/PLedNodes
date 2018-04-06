#include "ofApp.h"
#include "oscillatorTexture.h"
#include "waveScope.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    //ofSetVerticalSync(false);
    ofSetFrameRate(60);
    
    auto reg = make_shared<ofxOceanodeNodeRegistry>();
    reg->registerModel<oscillatorTexture>("LedNodes");
    reg->registerModel<waveScope>("LedNodes");
    
    
    container = make_shared<ofxOceanodeContainer>(reg);
    canvas.setContainer(container);
    canvas.setup();
    
    controls = new ofxOceanodeControls(container);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

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
