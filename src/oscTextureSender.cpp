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
    addParameterToGroupAndInfo(resampleX.set("Resample X", 100, 1, 100));//.isSavePreset = false;
    addParameterToGroupAndInfo(createDropdownAbstractParameter("Method X", {"Min", "Max", "Sum", "Mean"}, resampleXMethod));
    addParameterToGroupAndInfo(resampleY.set("Resample Y", 100, 1, 100));//.isSavePreset = false;
    addParameterToGroupAndInfo(createDropdownAbstractParameter("Method Y", {"Min", "Max", "Sum", "Mean"}, resampleYMethod));
    addParameterToGroupAndInfo(flip.set("Flip Send", false));
    addParameterToGroupAndInfo(fader.set("Master Fader", 1, 0, 1));
    addParameterToGroupAndInfo(texture.set("Texture", nullptr));
    oscSender.setup(oscHost, ofToInt(oscPort));
    resampleXMethod = 1;
    resampleYMethod = 1;
    
    lastSize = glm::vec2(100, 100);
    
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
        if(lastSize != glm::vec2(texWidth, texHeight)){
            lastSize = glm::vec2(texWidth, texHeight);
            resampleX.setMax(texWidth);
            resampleY.setMax(texHeight);
            string parameterNameMinMaxChanged = resampleX.getName();
            parameterChangedMinMax.notify(parameterNameMinMaxChanged);
            parameterNameMinMaxChanged = resampleY.getName();
            parameterChangedMinMax.notify(parameterNameMinMaxChanged);
            resampleX = texWidth;
            resampleY = texHeight;
            
        }
        if(resampleY > texHeight) resampleX = texWidth;
        if(resampleX > texWidth) resampleY = texHeight;
        float *p = pixels.begin();
        int pixelSize = pixels.getPixelFormat();
        int quantizationY = texHeight / resampleY;
        int quantizationX = texWidth / resampleX;
        if(flip){
            vector<vector<float>> tempCollection(quantizationY, vector<float>(resampleX, 0));
            vector<float> tempUnion(resampleX, 0);
            for(int ii = 0 ; ii < texHeight; ii+= quantizationY){
                for(int i = ii; i < ii+quantizationY; i++){
                    for(int j = 0; j < texWidth; j+= quantizationX){
                        vector<float> tempPixel(quantizationX, 0);
                        for(int k = 0; k < quantizationX; k++){
                            int index = ((j+k)+(texWidth*i))*pixelSize;
                            tempPixel[k] = (p[index] + p[index+1] + p[index+2]) / 3.0f;
                        }
                        switch (resampleXMethod) {
                            case 0:
                                tempCollection[i-ii][j/quantizationX] = *min_element(tempPixel.begin(), tempPixel.end());
                                break;
                            case 1:
                                tempCollection[i-ii][j/quantizationX] = *max_element(tempPixel.begin(), tempPixel.end());
                                break;
                            case 2:
                                tempCollection[i-ii][j/quantizationX] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0);
                                break;
                            case 3:
                                tempCollection[i-ii][j/quantizationX] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0)/quantizationX;
                                break;
                            default:
                                break;
                        }
                    }
                }
                switch (resampleYMethod) {
                    case 0:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = *min_element(comparer.begin(), comparer.end());
                        }
                        break;
                    case 1:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = *max_element(comparer.begin(), comparer.end());
                        }
                        break;
                    case 2:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = accumulate(comparer.begin(), comparer.end(), 0.0);
                        }
                        break;
                    case 3:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = accumulate(comparer.begin(), comparer.end(), 0.0)/comparer.size();
                        }
                        break;
                    default:
                        break;
                }
                
                ofxOscMessage message;
                //            if(texWidth  == 1)
                //                message.setAddress(address.get());
                //            else
                message.setAddress(address.get() + "/" + ofToString(ii/quantizationY));
                
                for(auto f : tempUnion){
                    message.addFloatArg(f * fader);
                }
                oscSender.sendMessage(message);
            }
        }else{
            vector<vector<float>> tempCollection(quantizationX, vector<float>(resampleY, 0));
            vector<float> tempUnion(resampleY, 0);
            for(int ii = 0 ; ii < texWidth; ii+= quantizationX){
                for(int i = ii; i < ii+quantizationX; i++){
                    for(int j = 0; j < texHeight; j+= quantizationY){
                        vector<float> tempPixel(quantizationY, 0);
                        for(int k = 0; k < quantizationY; k++){
                            int index = (((j+k)*texWidth)+(i))*pixelSize;
                            tempPixel[k] = (p[index] + p[index+1] + p[index+2]) / 3.0f;
                        }
                        switch (resampleYMethod) {
                            case 0:
                                tempCollection[i-ii][j/quantizationY] = *min_element(tempPixel.begin(), tempPixel.end());
                                break;
                            case 1:
                                tempCollection[i-ii][j/quantizationY] = *max_element(tempPixel.begin(), tempPixel.end());
                                break;
                            case 2:
                                tempCollection[i-ii][j/quantizationY] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0);
                                break;
                            case 3:
                                tempCollection[i-ii][j/quantizationY] = accumulate(tempPixel.begin(), tempPixel.end(), 0.0)/quantizationY;
                                break;
                            default:
                                break;
                        }
                    }
                }
                switch (resampleXMethod) {
                    case 0:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = *min_element(comparer.begin(), comparer.end());
                        }
                        break;
                    case 1:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = *max_element(comparer.begin(), comparer.end());
                        }
                        break;
                    case 2:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = accumulate(comparer.begin(), comparer.end(), 0.0);
                        }
                        break;
                    case 3:
                        for(int kk = 0 ; kk < tempUnion.size(); kk++){
                            vector<float> comparer(tempCollection.size());
                            for(int kkk = 0; kkk < tempCollection.size(); kkk++){
                                comparer[kkk] = tempCollection[kkk][kk];
                            }
                            tempUnion[kk] = accumulate(comparer.begin(), comparer.end(), 0.0)/comparer.size();
                        }
                        break;
                    default:
                        break;
                }
                
                ofxOscMessage message;
                //            if(texWidth  == 1)
                //                message.setAddress(address.get());
                //            else
                message.setAddress(address.get() + "/" + ofToString(ii/quantizationX));
                
                for(auto f : tempUnion){
                    message.addFloatArg(f * fader);
                }
                oscSender.sendMessage(message);
            }
        }
    }
}

