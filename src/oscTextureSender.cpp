//
//  oscTextureSender.cpp
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 05/08/2019.
//

#include "oscTextureSender.h"

oscTextureSender::oscTextureSender() : ofxOceanodeNodeModel("Texture Sender"){
    addParameterToGroupAndInfo(oscHost.set("Osc Host", "localhost"));//.isSavePreset = false;
    addParameterToGroupAndInfo(oscPort.set("Osc Port", "11511"));//.isSavePreset = false;
    addParameterToGroupAndInfo(address.set("Osc Address", "/texture")).isSavePreset = true;
    addParameterToGroupAndInfo(resample.set("Resample", 80, 1, 240));//.isSavePreset = false;
    addParameterToGroupAndInfo(createDropdownAbstractParameter("Method", {"Min", "Max", "Sum", "Mean"}, resampleMethod));
    addParameterToGroupAndInfo(fader.set("Master Fader", 1, 0, 1));
    addParameterToGroupAndInfo(texture.set("Texture", nullptr));
    oscSender.setup(oscHost, ofToInt(oscPort));
    resampleMethod = 1;
    
    listeners.push(oscHost.newListener(this, &oscTextureSender::oscHostAndPortListener));
    listeners.push(oscPort.newListener(this, &oscTextureSender::oscHostAndPortListener));
}

void oscTextureSender::oscHostAndPortListener(string &s){
    oscSender.setup(oscHost, ofToInt(oscPort));
}

void oscTextureSender::draw(ofEventArgs &a){
    if(texture.get() != nullptr){
        ofFloatPixels pixels;
        texture.get()->readToPixels(pixels);
        int texWidth = texture.get()->getWidth();
        int texHeight = texture.get()->getHeight();
        if(resample > texHeight) resample = texHeight;
        float *p = pixels.begin();
        int pixelSize = pixels.getPixelFormat();
        int quantization = texHeight / resample;
        if(resample > 1){
            vector<float> tempOutput(resample, 0);
            for(int i = 0; i < texWidth; i++){
                for(int j = 0; j < texHeight; j+= quantization){
                    vector<float> tempPixel(quantization, 0);
                    for(int k = 0; k < quantization; k++){
                        int index = (((j+k)*texWidth)+(i))*pixelSize;
                        tempPixel[k] = (p[index] + p[index+1] + p[index+2]) / 3.0f;
                    }
                    switch (resampleMethod) {
                        case 0:
                            tempOutput[j/quantization] = *min_element(tempPixel.begin(), tempPixel.end());
                            break;
                        case 1:
                            tempOutput[j/quantization] = *max_element(tempPixel.begin(), tempPixel.end());
                            break;
                        case 2:
                            tempOutput[j/quantization] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0);
                            break;
                        case 3:
                            tempOutput[j/quantization] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0)/quantization;
                            break;
                        default:
                            break;
                    }
                }
                ofxOscMessage message;
                if(texWidth  == 1)
                    message.setAddress(address.get());
                else
                    message.setAddress(address.get() + "/" + ofToString(i));
                
                for(auto f : tempOutput){
                    message.addFloatArg(f * fader);
                }
                oscSender.sendMessage(message);
            }
        }
        else{
            vector<float> tempOutput(texWidth, 0);
            for(int i = 0; i < texWidth; i++){
                int j = 0;
                vector<float> tempPixel(quantization, 0);
                for(int k = 0; k < quantization; k++){
                    int index = (((j+k)*texWidth)+(i))*pixelSize;
                    tempPixel[k] = (p[index] + p[index+1] + p[index+2]) / 3.0f;
                }
                switch (resampleMethod) {
                    case 0:
                        tempOutput[i] = *min_element(tempPixel.begin(), tempPixel.end());
                        break;
                    case 1:
                        tempOutput[i] = *max_element(tempPixel.begin(), tempPixel.end());
                        break;
                    case 2:
                        tempOutput[i] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0);
                        break;
                    case 3:
                        tempOutput[i] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0)/quantization;
                        break;
                    default:
                        break;
                }
            }
            ofxOscMessage message;
            message.setAddress(address.get());
            for(auto f : tempOutput){
                message.addFloatArg(f*fader);
            }
            oscSender.sendMessage(message);
        }
    }
}

