

#include "textureMixer.h"
#define STRINGIFY(x) #x



textureMixer::textureMixer() : ofxOceanodeNodeModel("Texture Mixer"){
    
    parameters->add(triggerTextureIndex.set("Trigger Index", 0, 0, 1));
    parameters->add(mixMode.set("Mix Mode",0,0,4));
    parameters->add(crossFader.set("Crossfader",0,0.0,1.0));

    inputs.resize(2);
    for(int i = 0; i < inputs.size() ; i++){
        parameters->add(inputs[i].set("Input " + ofToString(i), nullptr));
        inputs[i].addListener(this, &textureMixer::computeOutput);
    }
    parameters->add(output.set("Output", nullptr));
    
    
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
            
            out_Color = ((texColor*op1) + (texColor2*op2));
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
//            abs(base - blend)
            vec4 one = vec4(1.0,1.0,1.0,1.0);
            out_Color = abs(texColor2 - texColor);
        }


    }
    )";
    
    mixerShader.setupShaderFromSource(GL_VERTEX_SHADER, vertSource);
    mixerShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragSource);
    mixerShader.bindDefaults();
    mixerShader.linkProgram();
    
    
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
}
