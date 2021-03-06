
#include "textureMixer.h"
#define STRINGIFY(x) #x



textureMixer::textureMixer() : ofxOceanodeNodeModel("Texture Mixer")
{

    mixModeName = {"FADE","MIN","MAX","ADD","MULTIPLY","DIFFERENCE","TEST"};
    
    parameters->add(triggerTextureIndex.set("Trigger Index", 0, 0, 1));
    inputs.resize(2);
    for(int i = 0; i < inputs.size() ; i++){
        parameters->add(inputs[i].set("Input " + ofToString(i), nullptr));
        inputs[i].addListener(this, &textureMixer::computeOutput);
    }
//    parameters->add(mixMode.set("Mix Mode",0,0,6));
    parameters->add(createDropdownAbstractParameter("Mix Function", mixModeName, mixMode));
    parameters->add(crossFader.set("Crossfader",0,0.0,1.0));

    parameters->add(output.set("Output", nullptr));
    
    isSetup = false;
}
textureMixer::~textureMixer(){
    if(isSetup){
        resources->makeTextureLocationAvailable(texture0Location);
        resources->makeTextureLocationAvailable(texture1Location);
    }
}

void textureMixer::setup()
{
//    string defaultVertSource =
//    #include "defaultVertexShader.h";
//
//    string oscillatorFragSource =
//    #include "oscillatorFragShader.h";
    
    resources = &sharedResources::getInstance();
    texture0Location = resources->getNextAvailableShaderTextureLocation();
    texture1Location = resources->getNextAvailableShaderTextureLocation();

    const string vertSource=R"(#version 410
    
    // these are for the programmable pipeline system and are passed in
    // by default from OpenFrameworks
    uniform mat4 modelViewProjectionMatrix;
    //
    in vec4 position;
    in vec2 texcoord;
    
    // this is something we're creating for this shader
    out vec2 out_texCoord;
    
    void    main()
    {
        out_texCoord = texcoord;
        gl_Position = modelViewProjectionMatrix * position;
    }

    )";
    
    const string fragSource=R"(#version 410
    
    // In
    smooth in vec2      out_texCoord;
    
    // Uni
    uniform sampler2D   tex0;
    uniform sampler2D   tex1;
    uniform float       u_crossfade;
    uniform int         u_mixMode;
    // Out
    out vec4            out_Color;
    
    void    main()
    {
        vec4    texColor = texelFetch(tex0, ivec2(gl_FragCoord.x,gl_FragCoord.y),0);
        vec4    texColor2 = texelFetch(tex1, ivec2(gl_FragCoord.x,gl_FragCoord.y),0);
        vec4    one = vec4(1.0,1.0,1.0,1.0);

        // FADE ALPHA
        if(u_mixMode==0)
        {
            float op1 = u_crossfade;
            float op2 = 1.0 - u_crossfade;
            out_Color = (texColor*op1) + (texColor2*op2);
        }
        // MIN
        else if(u_mixMode==1)
        {
            float op1 = (u_crossfade *2);
            if(op1>1.0) op1 = 1.0;
            float op2 = ((1.0 - u_crossfade) * 2);
            if(op2>1.0) op2 = 1.0;
            
            out_Color = min ((texColor*op1),(texColor2*op2));
        }

        // MAX
        else if(u_mixMode==2)
        {
            float op1 = (u_crossfade *2);
            if(op1>1.0) op1 = 1.0;
            float op2 = ((1.0 - u_crossfade) * 2);
            if(op2>1.0) op2 = 1.0;
            
            out_Color = max ((texColor*op1),(texColor2*op2));
        }
        // ADDITIVE
        else if(u_mixMode==3)
        {
            float op1 = (u_crossfade *2);
            if(op1>1.0) op1 = 1.0;
            float op2 = ((1.0 - u_crossfade) * 2);
            if(op2>1.0) op2 = 1.0;
            
            out_Color = min((texColor*op1) + (texColor2*op2),1.0);
        }
        // MULTIPLY
        else if(u_mixMode==4)
        {
            float op1 = 0 + (1 - 0) * ((u_crossfade - 0) / (0.5 - 0));
            op1 = clamp(op1, 0, 1);
            float op2 = 1 + (0 - 1) * ((u_crossfade - 0.5) / (1 - 0.5));
            op2 = clamp(op2, 0, 1);
            
            out_Color = (texColor*op2 + (1-op2)) * (texColor2*op1 + (1-op1));
        }
        // DIFFERENCE
        else if(u_mixMode==5)
        {
            //abs(base - blend)
            vec3 outAux = abs(texColor2.xyz - texColor.xyz);
            out_Color = vec4(outAux.xyz, 1.0);
        }
//        // NEGATION
//        //(vec3(1.0) - abs(vec3(1.0) - base - blend))
//        else if(u_mixMode==6)
//        {
//            //abs(base - blend)
//            vec3 outAux = one.xyz - abs(one.xyz - texColor2.xyz - texColor.xyz);
//            out_Color = vec4(outAux.xyz, 1.0);
//        }
        // SUBSTRACT
        //max(base + blend - 1.0, 0.0)
        else if(u_mixMode==6)
        {
            //abs(base - blend)
            vec3 outAux = max(texColor2.xyz*u_crossfade + texColor.xyz*u_crossfade - one.xyz, 0.0);
            out_Color = vec4(outAux.xyz, 1.0);
        }

        


    }
    )";
    
    mixerShader.setupShaderFromSource(GL_VERTEX_SHADER, vertSource);
    mixerShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragSource);
    mixerShader.bindDefaults();
    mixerShader.linkProgram();
    
    isSetup = true;
}

void textureMixer::computeOutput(ofTexture* &in)
{
    if((inputs[0] != nullptr)&&(inputs[1] != nullptr))
    {
        if(&in == &inputs[triggerTextureIndex].get())
        {
            if(outputFbo.getWidth() != inputs[triggerTextureIndex].get()->getWidth() || outputFbo.getHeight() != inputs[triggerTextureIndex].get()->getHeight() || !outputFbo.isAllocated())
            {
                outputFbo.allocate(inputs[triggerTextureIndex].get()->getWidth(), inputs[triggerTextureIndex].get()->getHeight(), GL_RGBA32F);
                outputFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            }
            
            outputFbo.begin();
            ofClear(0, 0, 0);
            mixerShader.begin();
            mixerShader.setUniform1i("u_mixMode",mixMode);
            mixerShader.setUniform1f("u_crossfade",crossFader);
            mixerShader.setUniformTexture("tex0", *inputs[0].get(),texture0Location);
            mixerShader.setUniformTexture("tex1", *inputs[1].get(),texture1Location);

//            for(auto t : inputs){
//                if(t != nullptr)
//                {
//                    ofSetColor(255);
//                    t.get()->draw(0,0,inputs[triggerTextureIndex].get()->getWidth(), inputs[triggerTextureIndex].get()->getHeight());
//                }
//            }
            
            ofSetColor(255,255,255,255);
            ofDrawRectangle(0,0,outputFbo.getWidth(),outputFbo.getHeight());
            mixerShader.end();
            
            outputFbo.end();
            
            output = &outputFbo.getTexture();
        }
    }
    //GLenum err;
    //while ((err = glGetError()) != GL_NO_ERROR) {
    //    ofLog() << "OpenGL error: " << err;
    //}
}

/*
 
 //
 //  ofxPSBlend.h
 //
 //  Created by Akira Hayasaka on 5/14/12.
 //  Copyright (c) 2012 ﾟ･:,｡ﾟ･:,｡★ﾟ･:,｡ﾟ･:,｡☆ﾟ･:,｡ﾟ･:,｡★ﾟ･:,｡ﾟ･:,｡☆. All rights reserved.
 //
 #pragma once
 
 #include "ofMain.h"
 
 #define STRINGIFY(x) #x
 
 static string psBlendFragShaderGL2 = "#version 120\n \
 #extension GL_ARB_texture_rectangle : enable\n \
 \
 #define BlendLinearDodgef BlendAddf\n \
 #define BlendLinearBurnf BlendSubstractf\n \
 #define BlendAddf(base, blend) min(base + blend, 1.0)\n \
 #define BlendSubstractf(base, blend) max(base + blend - 1.0, 0.0)\n \
 #define BlendLightenf(base, blend) max(blend, base)\n \
 #define BlendDarkenf(base, blend) min(blend, base)\n \
 #define BlendLinearLightf(base, blend) (blend < 0.5 ? BlendLinearBurnf(base, (2.0 * blend)) : BlendLinearDodgef(base, (2.0 * (blend - 0.5))))\n \
 #define BlendScreenf(base, blend) (1.0 - ((1.0 - base) * (1.0 - blend)))\n \
 #define BlendOverlayf(base, blend) (base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))\n \
 #define BlendSoftLightf(base, blend) ((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))\n \
 #define BlendColorDodgef(base, blend) ((blend == 1.0) ? blend : min(base / (1.0 - blend), 1.0))\n \
 #define BlendColorBurnf(base, blend) ((blend == 0.0) ? blend : max((1.0 - ((1.0 - base) / blend)), 0.0))\n \
 #define BlendVividLightf(base, blend) ((blend < 0.5) ? BlendColorBurnf(base, (2.0 * blend)) : BlendColorDodgef(base, (2.0 * (blend - 0.5))))\n \
 #define BlendPinLightf(base, blend) ((blend < 0.5) ? BlendDarkenf(base, (2.0 * blend)) : BlendLightenf(base, (2.0 *(blend - 0.5))))\n \
 #define BlendHardMixf(base, blend) ((BlendVividLightf(base, blend) < 0.5) ? 0.0 : 1.0)\n \
 #define BlendReflectf(base, blend) ((blend == 1.0) ? blend : min(base * base / (1.0 - blend), 1.0))\n \
 #define Blend(base, blend, funcf) vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))\n \
 #define BlendLightenBlendLightenf\n \
 #define BlendDarkenBlendDarkenf\n \
 #define BlendNormal(base, blend) (blend)\n \
 #define BlendMultiply(base, blend) (base * blend)\n \
 #define BlendAverage(base, blend) ((base + blend) / 2.0)\n \
 #define BlendAdd(base, blend) min(base + blend, vec3(1.0))\n \
 #define BlendSubstract(base, blend) max(base + blend - vec3(1.0), vec3(0.0))\n \
 #define BlendDifference(base, blend) abs(base - blend)\n \
 #define BlendNegation(base, blend) (vec3(1.0) - abs(vec3(1.0) - base - blend))\n \
 #define BlendExclusion(base, blend) (base + blend - 2.0 * base * blend)\n \
 #define BlendScreen(base, blend) Blend(base, blend, BlendScreenf)\n \
 #define BlendOverlay(base, blend) Blend(base, blend, BlendOverlayf)\n \
 #define BlendSoftLight(base, blend) Blend(base, blend, BlendSoftLightf)\n \
 #define BlendHardLight(base, blend) BlendOverlay(blend, base)\n \
 #define BlendColorDodge(base, blend) Blend(base, blend, BlendColorDodgef)\n \
 #define BlendColorBurn(base, blend) Blend(base, blend, BlendColorBurnf)\n \
 #define BlendLinearDodgeBlendAdd\n \
 #define BlendLinearBurnBlendSubstract\n \
 #define BlendLinearLight(base, blend) Blend(base, blend, BlendLinearLightf)\n \
 #define BlendVividLight(base, blend) Blend(base, blend, BlendVividLightf)\n \
 #define BlendPinLight(base, blend) Blend(base, blend, BlendPinLightf)\n \
 #define BlendHardMix(base, blend) Blend(base, blend, BlendHardMixf)\n \
 #define BlendReflect(base, blend) Blend(base, blend, BlendReflectf)\n \
 #define BlendGlow(base, blend) BlendReflect(blend, base)\n \
 #define BlendPhoenix(base, blend) (min(base, blend) - max(base, blend) + vec3(1.0))\n \
 #define BlendOpacity(base, blend, F, O) (F(base, blend) * O + blend * (1.0 - O))\n \
 #define GammaCorrection(color, gamma)pow(color, 1.0 / gamma)\n \
 #define LevelsControlInputRange(color, minInput, maxInput)min(max(color - vec3(minInput), vec3(0.0)) / (vec3(maxInput) - vec3(minInput)), vec3(1.0))\n \
 #define LevelsControlInput(color, minInput, gamma, maxInput)GammaCorrection(LevelsControlInputRange(color, minInput, maxInput), gamma)\n \
 #define LevelsControlOutputRange(color, minOutput, maxOutput) mix(vec3(minOutput), vec3(maxOutput), color)\n \
 #define LevelsControl(color, minInput, gamma, maxInput, minOutput, maxOutput) LevelsControlOutputRange(LevelsControlInput(color, minInput, gamma, maxInput), minOutput, maxOutput)\n" STRINGIFY(
 
 vec4 Desaturate(vec3 color, float Desaturation)
 {
 vec3 grayXfer = vec3(0.3, 0.59, 0.11);
 vec3 gray = vec3(dot(grayXfer, color));
 return vec4(mix(color, gray, Desaturation), 1.0);
 }
 
 vec3 RGBToHSL(vec3 color)
 {
 vec3 hsl;
 float fmin = min(min(color.r, color.g), color.b);
 float fmax = max(max(color.r, color.g), color.b);
 float delta = fmax - fmin;
 hsl.z = (fmax + fmin) / 2.0;
 if (delta == 0.0)
 {
 hsl.x = 0.0;
 hsl.y = 0.0;
 }
 else
 {
 if (hsl.z < 0.5)
 hsl.y = delta / (fmax + fmin);
 else
 hsl.y = delta / (2.0 - fmax - fmin);
 float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
 float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
 float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;
 if (color.r == fmax )
 hsl.x = deltaB - deltaG;
 else if (color.g == fmax)
 hsl.x = (1.0 / 3.0) + deltaR - deltaB;
 else if (color.b == fmax)
 hsl.x = (2.0 / 3.0) + deltaG - deltaR;
 if (hsl.x < 0.0)
 hsl.x += 1.0;
 else if (hsl.x > 1.0)
 hsl.x -= 1.0;
 }
 return hsl;
 }
 
 float HueToRGB(float f1, float f2, float hue)
 {
 if (hue < 0.0)
 hue += 1.0;
 else if (hue > 1.0)
 hue -= 1.0;
 float res;
 if ((6.0 * hue) < 1.0)
 res = f1 + (f2 - f1) * 6.0 * hue;
 else if ((2.0 * hue) < 1.0)
 res = f2;
 else if ((3.0 * hue) < 2.0)
 res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
 else
 res = f1;
 return res;
 }
 
 vec3 HSLToRGB(vec3 hsl)
 {
 vec3 rgb;
 if (hsl.y == 0.0)
 rgb = vec3(hsl.z);
 else
 {
 float f2;
 if (hsl.z < 0.5)
 f2 = hsl.z * (1.0 + hsl.y);
 else
 f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);
 float f1 = 2.0 * hsl.z - f2;
 rgb.r = HueToRGB(f1, f2, hsl.x + (1.0/3.0));
 rgb.g = HueToRGB(f1, f2, hsl.x);
 rgb.b= HueToRGB(f1, f2, hsl.x - (1.0/3.0));
 }
 return rgb;
 }
 
 vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con)
 {
 const float AvgLumR = 0.5;
 const float AvgLumG = 0.5;
 const float AvgLumB = 0.5;
 const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
 vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
 vec3 brtColor = color * brt;
 vec3 intensity = vec3(dot(brtColor, LumCoeff));
 vec3 satColor = mix(intensity, brtColor, sat);
 vec3 conColor = mix(AvgLumin, satColor, con);
 return conColor;
 }
 
 vec3 BlendHue(vec3 base, vec3 blend)
 {
 vec3 baseHSL = RGBToHSL(base);
 return HSLToRGB(vec3(RGBToHSL(blend).r, baseHSL.g, baseHSL.b));
 }
 
 vec3 BlendSaturation(vec3 base, vec3 blend)
 {
 vec3 baseHSL = RGBToHSL(base);
 return HSLToRGB(vec3(baseHSL.r, RGBToHSL(blend).g, baseHSL.b));
 }
 
 vec3 BlendColor(vec3 base, vec3 blend)
 {
 vec3 blendHSL = RGBToHSL(blend);
 return HSLToRGB(vec3(blendHSL.r, blendHSL.g, RGBToHSL(base).b));
 }
 
 vec3 BlendLuminosity(vec3 base, vec3 blend)
 {
 vec3 baseHSL = RGBToHSL(base);
 return HSLToRGB(vec3(baseHSL.r, baseHSL.g, RGBToHSL(blend).b));
 }
 
 uniform sampler2DRect base;
 uniform sampler2DRect blendTgt;
 uniform int mode;
 uniform float opacity;
 
 void main()
 {
 vec4 baseCol = texture2DRect(base, gl_TexCoord[0].st);
 vec4 blendCol = texture2DRect(blendTgt, gl_TexCoord[0].st);
 blendCol.rgb *= opacity;
 
 vec3 result;
 if (mode == 0)
 {
 result = BlendNormal(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 1)
 {
 result = BlendMultiply(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 2)
 {
 result = BlendAverage(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 3)
 {
 result = BlendAdd(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 4)
 {
 result = BlendSubstract(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 5)
 {
 result = BlendDifference(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 6)
 {
 result = BlendNegation(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 7)
 {
 result = BlendExclusion(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 8)
 {
 result = BlendScreen(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 9)
 {
 result = BlendOverlay(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 10)
 {
 result = BlendSoftLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 11)
 {
 result = BlendHardLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 12)
 {
 result = BlendColorDodge(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 13)
 {
 result = BlendColorBurn(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 14)
 {
 result = BlendLinearLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 15)
 {
 result = BlendVividLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 16)
 {
 result = BlendPinLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 17)
 {
 result = BlendHardMix(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 18)
 {
 result = BlendReflect(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 19)
 {
 result = BlendGlow(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 20)
 {
 result = BlendPhoenix(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 21)
 {
 result = BlendHue(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 22)
 {
 result = BlendSaturation(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 23)
 {
 result = BlendColor(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 24)
 {
 result = BlendLuminosity(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 25)
 {
 result = mix(baseCol.rgb, blendCol.rgb,opacity);
 }
 else
 {
 result = BlendNormal(baseCol.rgb, blendCol.rgb);
 }
 
 if (blendCol.a <= 0.0)
 {
 gl_FragColor = vec4(0.0,0.0,0.0,0.0);
 }
 else
 {
 gl_FragColor = vec4(result, 1.0);
 }
 }
 );
 
 static string psBlendVertShaderGL3 = "#version 150\n" STRINGIFY(
 
 uniform mat4 projectionMatrix;
 uniform mat4 modelViewMatrix;
 uniform mat4 textureMatrix;
 uniform mat4 modelViewProjectionMatrix;
 
 in vec4  position;
 in vec2  texcoord;
 in vec4  color;
 in vec3  normal;
 
 out vec2 texCoordVarying;
 
 void main()
 {
 texCoordVarying = texcoord;
 gl_Position = modelViewProjectionMatrix * position;
 }
 );
 
 static string psBlendFragShaderGL3 = "#version 150\n \
 \
 #define BlendLinearDodgef BlendAddf\n \
 #define BlendLinearBurnf BlendSubstractf\n \
 #define BlendAddf(base, blend) min(base + blend, 1.0)\n \
 #define BlendSubstractf(base, blend) max(base + blend - 1.0, 0.0)\n \
 #define BlendLightenf(base, blend) max(blend, base)\n \
 #define BlendDarkenf(base, blend) min(blend, base)\n \
 #define BlendLinearLightf(base, blend) (blend < 0.5 ? BlendLinearBurnf(base, (2.0 * blend)) : BlendLinearDodgef(base, (2.0 * (blend - 0.5))))\n \
 #define BlendScreenf(base, blend) (1.0 - ((1.0 - base) * (1.0 - blend)))\n \
 #define BlendOverlayf(base, blend) (base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))\n \
 #define BlendSoftLightf(base, blend) ((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))\n \
 #define BlendColorDodgef(base, blend) ((blend == 1.0) ? blend : min(base / (1.0 - blend), 1.0))\n \
 #define BlendColorBurnf(base, blend) ((blend == 0.0) ? blend : max((1.0 - ((1.0 - base) / blend)), 0.0))\n \
 #define BlendVividLightf(base, blend) ((blend < 0.5) ? BlendColorBurnf(base, (2.0 * blend)) : BlendColorDodgef(base, (2.0 * (blend - 0.5))))\n \
 #define BlendPinLightf(base, blend) ((blend < 0.5) ? BlendDarkenf(base, (2.0 * blend)) : BlendLightenf(base, (2.0 *(blend - 0.5))))\n \
 #define BlendHardMixf(base, blend) ((BlendVividLightf(base, blend) < 0.5) ? 0.0 : 1.0)\n \
 #define BlendReflectf(base, blend) ((blend == 1.0) ? blend : min(base * base / (1.0 - blend), 1.0))\n \
 #define Blend(base, blend, funcf) vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))\n \
 #define BlendLightenBlendLightenf\n \
 #define BlendDarkenBlendDarkenf\n \
 #define BlendNormal(base, blend) (blend)\n \
 #define BlendMultiply(base, blend) (base * blend)\n \
 #define BlendAverage(base, blend) ((base + blend) / 2.0)\n \
 #define BlendAdd(base, blend) min(base + blend, vec3(1.0))\n \
 #define BlendSubstract(base, blend) max(base + blend - vec3(1.0), vec3(0.0))\n \
 #define BlendDifference(base, blend) abs(base - blend)\n \
 #define BlendNegation(base, blend) (vec3(1.0) - abs(vec3(1.0) - base - blend))\n \
 #define BlendExclusion(base, blend) (base + blend - 2.0 * base * blend)\n \
 #define BlendScreen(base, blend) Blend(base, blend, BlendScreenf)\n \
 #define BlendOverlay(base, blend) Blend(base, blend, BlendOverlayf)\n \
 #define BlendSoftLight(base, blend) Blend(base, blend, BlendSoftLightf)\n \
 #define BlendHardLight(base, blend) BlendOverlay(blend, base)\n \
 #define BlendColorDodge(base, blend) Blend(base, blend, BlendColorDodgef)\n \
 #define BlendColorBurn(base, blend) Blend(base, blend, BlendColorBurnf)\n \
 #define BlendLinearDodgeBlendAdd\n \
 #define BlendLinearBurnBlendSubstract\n \
 #define BlendLinearLight(base, blend) Blend(base, blend, BlendLinearLightf)\n \
 #define BlendVividLight(base, blend) Blend(base, blend, BlendVividLightf)\n \
 #define BlendPinLight(base, blend) Blend(base, blend, BlendPinLightf)\n \
 #define BlendHardMix(base, blend) Blend(base, blend, BlendHardMixf)\n \
 #define BlendReflect(base, blend) Blend(base, blend, BlendReflectf)\n \
 #define BlendGlow(base, blend) BlendReflect(blend, base)\n \
 #define BlendPhoenix(base, blend) (min(base, blend) - max(base, blend) + vec3(1.0))\n \
 #define BlendOpacity(base, blend, F, O) (F(base, blend) * O + blend * (1.0 - O))\n \
 #define GammaCorrection(color, gamma)pow(color, 1.0 / gamma)\n \
 #define LevelsControlInputRange(color, minInput, maxInput)min(max(color - vec3(minInput), vec3(0.0)) / (vec3(maxInput) - vec3(minInput)), vec3(1.0))\n \
 #define LevelsControlInput(color, minInput, gamma, maxInput)GammaCorrection(LevelsControlInputRange(color, minInput, maxInput), gamma)\n \
 #define LevelsControlOutputRange(color, minOutput, maxOutput) mix(vec3(minOutput), vec3(maxOutput), color)\n \
 #define LevelsControl(color, minInput, gamma, maxInput, minOutput, maxOutput) LevelsControlOutputRange(LevelsControlInput(color, minInput, gamma, maxInput), minOutput, maxOutput)\n" STRINGIFY(
 
 vec4 Desaturate(vec3 color, float Desaturation)
 {
 vec3 grayXfer = vec3(0.3, 0.59, 0.11);
 vec3 gray = vec3(dot(grayXfer, color));
 return vec4(mix(color, gray, Desaturation), 1.0);
 }
 
 vec3 RGBToHSL(vec3 color)
 {
 vec3 hsl;
 float fmin = min(min(color.r, color.g), color.b);
 float fmax = max(max(color.r, color.g), color.b);
 float delta = fmax - fmin;
 hsl.z = (fmax + fmin) / 2.0;
 if (delta == 0.0)
 {
 hsl.x = 0.0;
 hsl.y = 0.0;
 }
 else
 {
 if (hsl.z < 0.5)
 hsl.y = delta / (fmax + fmin);
 else
 hsl.y = delta / (2.0 - fmax - fmin);
 float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
 float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
 float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;
 if (color.r == fmax )
 hsl.x = deltaB - deltaG;
 else if (color.g == fmax)
 hsl.x = (1.0 / 3.0) + deltaR - deltaB;
 else if (color.b == fmax)
 hsl.x = (2.0 / 3.0) + deltaG - deltaR;
 if (hsl.x < 0.0)
 hsl.x += 1.0;
 else if (hsl.x > 1.0)
 hsl.x -= 1.0;
 }
 return hsl;
 }
 
 float HueToRGB(float f1, float f2, float hue)
 {
 if (hue < 0.0)
 hue += 1.0;
 else if (hue > 1.0)
 hue -= 1.0;
 float res;
 if ((6.0 * hue) < 1.0)
 res = f1 + (f2 - f1) * 6.0 * hue;
 else if ((2.0 * hue) < 1.0)
 res = f2;
 else if ((3.0 * hue) < 2.0)
 res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
 else
 res = f1;
 return res;
 }
 
 vec3 HSLToRGB(vec3 hsl)
 {
 vec3 rgb;
 if (hsl.y == 0.0)
 rgb = vec3(hsl.z);
 else
 {
 float f2;
 if (hsl.z < 0.5)
 f2 = hsl.z * (1.0 + hsl.y);
 else
 f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);
 float f1 = 2.0 * hsl.z - f2;
 rgb.r = HueToRGB(f1, f2, hsl.x + (1.0/3.0));
 rgb.g = HueToRGB(f1, f2, hsl.x);
 rgb.b= HueToRGB(f1, f2, hsl.x - (1.0/3.0));
 }
 return rgb;
 }
 
 vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con)
 {
 const float AvgLumR = 0.5;
 const float AvgLumG = 0.5;
 const float AvgLumB = 0.5;
 const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
 vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
 vec3 brtColor = color * brt;
 vec3 intensity = vec3(dot(brtColor, LumCoeff));
 vec3 satColor = mix(intensity, brtColor, sat);
 vec3 conColor = mix(AvgLumin, satColor, con);
 return conColor;
 }
 
 vec3 BlendHue(vec3 base, vec3 blend)
 {
 vec3 baseHSL = RGBToHSL(base);
 return HSLToRGB(vec3(RGBToHSL(blend).r, baseHSL.g, baseHSL.b));
 }
 
 vec3 BlendSaturation(vec3 base, vec3 blend)
 {
 vec3 baseHSL = RGBToHSL(base);
 return HSLToRGB(vec3(baseHSL.r, RGBToHSL(blend).g, baseHSL.b));
 }
 
 vec3 BlendColor(vec3 base, vec3 blend)
 {
 vec3 blendHSL = RGBToHSL(blend);
 return HSLToRGB(vec3(blendHSL.r, blendHSL.g, RGBToHSL(base).b));
 }
 
 vec3 BlendLuminosity(vec3 base, vec3 blend)
 {
 vec3 baseHSL = RGBToHSL(base);
 return HSLToRGB(vec3(baseHSL.r, baseHSL.g, RGBToHSL(blend).b));
 }
 
 in vec2 texCoordVarying;
 uniform sampler2D base;
 uniform sampler2D blendTgt;
 uniform int mode;
 uniform float opacityBase;
 uniform float opacityTarget;
 
 out vec4 fragColor;
 
 void main()
 {
 vec4 baseCol = texture(base, texCoordVarying)*opacityBase;
 vec4 blendCol = texture(blendTgt, texCoordVarying)*opacityTarget;
 
 vec3 result;
 if (mode == 0)
 {
 result = BlendNormal(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 1)
 {
 result = BlendMultiply(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 2)
 {
 result = BlendAverage(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 3)
 {
 result = BlendAdd(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 4)
 {
 result = BlendSubstract(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 5)
 {
 result = BlendDifference(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 6)
 {
 result = BlendNegation(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 7)
 {
 result = BlendExclusion(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 8)
 {
 result = BlendScreen(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 9)
 {
 result = BlendOverlay(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 10)
 {
 result = BlendSoftLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 11)
 {
 result = BlendHardLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 12)
 {
 result = BlendColorDodge(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 13)
 {
 result = BlendColorBurn(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 14)
 {
 result = BlendLinearLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 15)
 {
 result = BlendVividLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 16)
 {
 result = BlendPinLight(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 17)
 {
 result = BlendHardMix(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 18)
 {
 result = BlendReflect(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 19)
 {
 result = BlendGlow(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 20)
 {
 result = BlendPhoenix(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 21)
 {
 result = BlendHue(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 22)
 {
 result = BlendSaturation(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 23)
 {
 result = BlendColor(baseCol.rgb, blendCol.rgb);
 }
 else if (mode == 24)
 {
 result = BlendLuminosity(baseCol.rgb, blendCol.rgb);
 }
 else
 {
 result = BlendNormal(baseCol.rgb, blendCol.rgb);
 }
 
 if (blendCol.a <= 0.0)
 {
 fragColor = vec4(0.0,0.0,0.0,0.0);
 }
 else
 {
 fragColor = vec4(result, 1.0);
 }
 }
 );
 
 class ofxPSBlend
 {
 
 public:
 
 void setup(int width, int height)
 {
 base.allocate(width, height, GL_RGBA, ofFbo::maxSamples());
 quad.getVertices().resize(4);
 quad.getTexCoords().resize(4);
 quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
 this->width = width;
 this->height = height;
 loadShader();
 }
 
 void allocate(int width, int height)
 {
 base.allocate(width, height, GL_RGBA, ofFbo::maxSamples());
 quad.getVertices().resize(4);
 quad.getTexCoords().resize(4);
 quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
 this->width = width;
 this->height = height;
 }
 
 void loadShader()
 {
 if (ofIsGLProgrammableRenderer())
 {
 psBlendShader.setupShaderFromSource(GL_VERTEX_SHADER, psBlendVertShaderGL3);
 psBlendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, psBlendFragShaderGL3);
 psBlendShader.bindDefaults();
 }
 else
 {
 psBlendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, psBlendFragShaderGL2);
 }
 psBlendShader.linkProgram();
 }
 
 void begin()
 {
 ofPushStyle();
 ofPushMatrix();
 base.begin();
 ofClear(0);
 }
 
 void end()
 {
 base.end();
 ofPopStyle();
 ofPopMatrix();
 }
 
 void setOpacityBase(float _opacity)
 {
 opacityBase = _opacity;
 }
 void setOpacityTarget(float _opacity)
 {
 opacityTarget = _opacity;
 }
 
 void draw(ofTexture& blendTgt, int blendMode = 0)
 {
 psBlendShader.begin();
 psBlendShader.setUniformTexture("base", base, 1);
 psBlendShader.setUniformTexture("blendTgt", blendTgt, 2);
 psBlendShader.setUniform1i("mode", blendMode);
 psBlendShader.setUniform1f("opacityTarget", opacityTarget);
 psBlendShader.setUniform1f("opacityBase", opacityBase);
 drawPlane(width, height);
 psBlendShader.end();
 }
 void draw(ofTexture& blendTgt, int blendMode, int x, int y, int width, int height)
 {
 psBlendShader.begin();
 psBlendShader.setUniformTexture("base", base, 1);
 psBlendShader.setUniform1i("blendTgt", 0);
 psBlendShader.setUniform1i("mode", blendMode);
 psBlendShader.setUniform1f("opacityTarget", opacityTarget);
 psBlendShader.setUniform1f("opacityBase", opacityBase);
 ofEnableAlphaBlending();
 blendTgt.draw(x, y, width, height);
 psBlendShader.end();
 }
 
 string getBlendMode(int blendMode)
 {
 string blendName = "no such blend hiii";
 if (blendMode == 0)
 {
 blendName = "BlendNormal";
 }
 else if (blendMode == 1)
 {
 blendName = "BlendMultiply";
 }
 else if (blendMode == 2)
 {
 blendName = "BlendAverage";
 }
 else if (blendMode == 3)
 {
 blendName = "BlendAdd";
 }
 else if (blendMode == 4)
 {
 blendName = "BlendSubstract";
 }
 else if (blendMode == 5)
 {
 blendName = "BlendDifference";
 }
 else if (blendMode == 6)
 {
 blendName = "BlendNegation";
 }
 else if (blendMode == 7)
 {
 blendName = "BlendExclusion";
 }
 else if (blendMode == 8)
 {
 blendName = "BlendScreen";
 }
 else if (blendMode == 9)
 {
 blendName = "BlendOverlay";
 }
 else if (blendMode == 10)
 {
 blendName = "BlendSoftLight";
 }
 else if (blendMode == 11)
 {
 blendName = "BlendHardLight";
 }
 else if (blendMode == 12)
 {
 blendName = "BlendColorDodge";
 }
 else if (blendMode == 13)
 {
 blendName = "BlendColorBurn";
 }
 else if (blendMode == 14)
 {
 blendName = "BlendLinearLight";
 }
 else if (blendMode == 15)
 {
 blendName = "BlendVividLight";
 }
 else if (blendMode == 16)
 {
 blendName = "BlendPinLight";
 }
 else if (blendMode == 17)
 {
 blendName = "BlendHardMix";
 }
 else if (blendMode == 18)
 {
 blendName = "BlendReflect";
 }
 else if (blendMode == 19)
 {
 blendName = "BlendGlow";
 }
 else if (blendMode == 20)
 {
 blendName = "BlendPhoenix";
 }
 else if (blendMode == 21)
 {
 blendName = "BlendHue";
 }
 else if (blendMode == 22)
 {
 blendName = "BlendSaturation";
 }
 else if (blendMode == 23)
 {
 blendName = "BlendColor";
 }
 else if (blendMode == 24)
 {
 blendName = "BlendLuminosity";
 }
 else if (blendMode == 25)
 {
 blendName = "BlendOpacity";
 }
 return blendName;
 }
 
 ofFbo getBaseFbo(){return base;};
 
 
 protected: // Dragon!
 
 void drawPlane(float _width, float _height)
 {
 quad.setVertex(0, ofVec3f(0,0,0));
 quad.setVertex(1, ofVec3f(_width,0,0));
 quad.setVertex(2, ofVec3f(_width,_height,0));
 quad.setVertex(3, ofVec3f(0,_height,0));
 
 quad.setTexCoord(0, ofVec2f(0,0));
 quad.setTexCoord(1, ofVec2f(1.0,0));
 quad.setTexCoord(2, ofVec2f(1.0,1.0));
 quad.setTexCoord(3, ofVec2f(0,1.0));
 
 quad.draw();
 }
 
 ofFbo base;
 ofShader psBlendShader;
 float width;
 float height;
 ofVboMesh quad;
 float opacityTarget;
 float opacityBase;
 
 };

 */


