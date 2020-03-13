//
//  sharedInfo.h
//  PLedNodes
//
//  Created by Eduard Frigola Bagué on 13/03/2020.
//

#ifndef sharedInfo_h
#define sharedInfo_h

//class abstractType{
//public:
//    virtual ~abstractType() = default;
//};
//
//template <typename T>
//class templatedType : public abstractType {
//public:
//    templatedType(T _var){var = _var;}
//    ~templatedType(){};
//    T get(){return var;};
//public:
//    T var;
//};
#include "ofRectangle.h"

class sharedInfo{
public:
    sharedInfo(){}
    ~sharedInfo(){};
    
    static sharedInfo &getInstance()
    {
        static sharedInfo instance;
        return instance;
    }
    
//    template <typename T>
//    void set(string name, T val){
//        typedMap[name] = templatedType<T>(val);
//    }
//
//    template <typename T>
//    T get(string name){
//        return dynamic_cast<templatedType<T>*>(&typedMap[name])->get();
//    }
    
    void setString(string name, string val){
        stringMap[name] = val;
    }
    
    string getString(string name){
        return stringMap[name];
    }
    
    void setRect(string name, ofRectangle val){
        rectMap[name] = val;
    }
    
    ofRectangle getRect(string name){
        return rectMap[name];
    }
    
private:
//    map<string, abstractType> typedMap;
    map<string, string> stringMap;
    map<string, ofRectangle> rectMap;
};

#endif /* sharedInfo_h */
