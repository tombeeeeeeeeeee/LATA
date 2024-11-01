#include "RenderSystem.h"

#include "SceneManager.h"
#include "Scene.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "ResourceManager.h"
#include "Light.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "Skybox.h"
#include "Animator.h"
#include "FrameBuffer.h"
#include "ShaderEnum.h"
#include "ParticleSystem.h"

#include "Utilities.h"
#include "EditorGUI.h"

#include <iostream>

LineRenderer RenderSystem::lines;
LineRenderer RenderSystem::debugLines;

void RenderSystem::Start(
    unsigned int _skyboxTexture,
    std::vector<Shader*>* _shaders,
    Light* _shadowCaster,
    std::string paintStrokeTexturePath
)
{
    if (SCREEN_WIDTH == 0)
    {
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        SCREEN_WIDTH = scrWidth;
        SCREEN_HEIGHT = scrHeight;
    }
    shaders = _shaders;
    skyboxTexture = _skyboxTexture;

    cube = ResourceManager::LoadMesh();
    cube->InitialiseCube(2.0f);

    IBLBufferSetup(skyboxTexture);
    HDRBufferSetUp();
    OutputBufferSetUp();
    BloomSetup();
    SSAOSetup();
    DeferredSetup(); 
    AmibentPassSetup();

    shadowCaster = _shadowCaster;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    screenQuad = ResourceManager::LoadMesh();
    screenQuad->InitialiseQuad(1.f, 0.0f);

    paintStrokeTexture = nullptr;//ResourceManager::LoadTexture(paintStrokeTexturePath, Texture::Type::paint);

    (*shaders)[ShaderIndex::shadowDebug]->Use();
    (*shaders)[ShaderIndex::shadowDebug]->setInt("depthMap", 1);

    (*shaders)[ShaderIndex::lines]->Use();
    lines.Initialise();
    debugLines.Initialise();

    shadowCaster = _shadowCaster;
    shadowCaster->Initialise();


    ResourceManager::BindFlaggedVariables();

    postProcess = ResourceManager::LoadShader("Shaders/screenShader.vert", "Shaders/screenShader.frag");
    
    postFrameTexture = ResourceManager::LoadTexture(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_LINEAR, GL_LINEAR);
    postFrameBuffer = new FrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, postFrameTexture, nullptr, true);

    // TODO: dont reload the texture just to not have it be mip mapped
    baseColourKey = ResourceManager::LoadTexture("images/BlankColourKey.png", Texture::Type::count);
    baseColourKey->mipMapped = false;
    baseColourKey->Load();

    colourKey1 = ResourceManager::LoadTexture("images/GreyScaleColourKey.png", Texture::Type::count);
    colourKey1->mipMapped = false;
    colourKey1->Load();

    colourKey2 = ResourceManager::LoadTexture("images/HueShiftedColourKey.png", Texture::Type::count);
    colourKey2->mipMapped = false;
    colourKey2->Load();

}

void RenderSystem::SetIrradianceMap(unsigned int textureID)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //Convert skybox into an irradance version!
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    unsigned int currShader = (*shaders)[ShaderIndex::irradiance]->GLID;
    glUseProgram(currShader);
    glUniform1i(glGetUniformLocation(currShader, "environmentMap"), 1);
    glUniformMatrix4fv(glGetUniformLocation(currShader, "projection"), 1, GL_FALSE, &captureProjection[0][0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);


    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(currShader, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube->Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


void RenderSystem::DeferredUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

    glBindTexture(GL_TEXTURE_2D, normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, albedoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, emissionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create bloom buffer
    glGenTextures(1, &bloomBuffer);
    glBindTexture(GL_TEXTURE_2D, bloomBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, albedoBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emissionBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, bloomBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

    auto whatever = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (whatever != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer is not complete!" << "\n";
    }
}

void RenderSystem::AmbientPassUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ambientPassFBO);

    glBindTexture(GL_TEXTURE_2D, ambientPassBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, ambientPassFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ambientPassBuffer, 0);
}

void RenderSystem::HDRBufferUpdate()
{
    // create floating point color buffer
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create depth buffer (renderbuffer)
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OutputBufferUpdate();
}

void RenderSystem::OutputBufferUpdate()
{
    if (outputFBO == 0) return;

    // create unsigned int color buffer
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // attach buffer
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::BloomUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glm::vec2 mipSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    glm::ivec2 mipIntSize((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

    for (GLuint i = 0; i < bloomMipMapCount; i++)
    {
        mipSize *= 0.5f;
        mipIntSize /= 2;
        bloomMips[i].size = mipSize;
        bloomMips[i].intSize = mipIntSize;

        glGenTextures(1, &bloomMips[i].texture);
        glBindTexture(GL_TEXTURE_2D, bloomMips[i].texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
            (int)mipSize.x, (int)mipSize.y,
            0, GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D, bloomMips[0].texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::Update(
    std::unordered_map<unsigned long long, ModelRenderer>& renders,
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, ModelRenderer>& shadowCasters,
    std::unordered_map<unsigned long long, Animator>& animators,
    Camera* camera,
    std::vector<Particle*> particles
)
{
    std::unordered_set<unsigned long long> animatedRenderered = {};
    for (auto& i : animators)
    {
        animatedRenderered.emplace(i.first);
    }

    // TODO: There are a few issues when the res is too low
    if (SCREEN_WIDTH <= 64 || SCREEN_HEIGHT <= 64) { return; }

    // TODO: rather then constanty reloading the framebuffer, the texture could link to the framebuffers that need assoisiate with it? or maybe just refresh all framebuffers when a texture is loaded?
    shadowCaster->shadowFrameBuffer->Load();
    unsigned int deferredAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    unsigned int ambientAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    viewMatrix = camera->GetViewMatrix();

    //TODO: TO make more flexible?
    // Render depth of scene to texture (from light's perspective)
    glm::mat4 lightSpaceMatrix;
    lightSpaceMatrix = shadowCaster->getShadowViewProjection();

    (*shaders)[shadowMapDepth]->Use();
    (*shaders)[shadowMapDepth]->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, shadowCaster->shadowTexWidth, shadowCaster->shadowTexHeight);
    shadowCaster->shadowFrameBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //DrawAllRenderers(animators, transforms, renders, animatedRenderered, (*shaders)[shadowMapDepth]);

    // Render scene with shadow map, to the screen framebuffer
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDrawBuffers(4, deferredAttachments);

    DrawAllRenderers(animators, transforms, renders, animatedRenderered);

    (*shaders)[ShaderIndex::lines]->Use();
    lines.Draw();

    glDepthFunc(GL_ALWAYS);
    debugLines.Draw();

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    //glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glBlendEquation(GL_FUNC_ADD);

    ParticleSystem::Draw(particles);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);
    RenderSSAO();

    RenderBloom(bloomBuffer);

    RenderAmbientPass();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Unbind framebuffer
    //FrameBuffer::Unbind();
    glDisable(GL_DEPTH_TEST); // Disable depth test for fullscreen quad


    //FrameBuffer Rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //HDR
    

    (*shaders)[screen]->Use();

    (*shaders)[screen]->setInt("ambientPass", 1);
    (*shaders)[screen]->setInt("albedo", 2);
    (*shaders)[screen]->setInt("normal", 3);
    (*shaders)[screen]->setInt("emission", 4);
    (*shaders)[screen]->setInt("SSAO", 5);
    (*shaders)[screen]->setInt("bloomBlur", 6);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ambientPassBuffer);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, albedoBuffer);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, emissionBuffer);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, bloomMips[0].texture);


    (*shaders)[screen]->setFloat("exposure", exposure);
    (*shaders)[screen]->setInt("bufferIndex", bufferIndex);

    if (postEffectOn) {
        postFrameBuffer->Bind();
    }

    RenderQuad();

    if (postEffectOn) {
        FrameBuffer::Unbind();


        postProcess->Use();
        postFrameTexture->Bind(1);
        postProcess->setSampler("colourTexture", 1);
        baseColourKey->Bind(2);
        colourKey1->Bind(3);
        postProcess->setSampler("colourGradeKey1", 3);
        colourKey2->Bind(4);
        postProcess->setSampler("colourGradeKey2", 4);

        postProcess->setFloat("colourGradeInterpolation", postEffectPercent);

        RenderQuad();
    }

    // Re enable the depth test
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::SSAOUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glBindTexture(GL_TEXTURE_2D, ssaoBluredBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBluredBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::ScreenResize(int width, int height)
{
    SCREEN_HEIGHT = height;
    SCREEN_WIDTH = width;

    postFrameBuffer->setWidthHeight(SCREEN_WIDTH, SCREEN_HEIGHT);
    postFrameTexture->setWidthHeight(SCREEN_WIDTH, SCREEN_HEIGHT);
    postFrameBuffer->Load();

    DeferredUpdate();
    AmbientPassUpdate();
    SSAOUpdate();
    HDRBufferUpdate();
    OutputBufferUpdate();
    BloomUpdate();
    AmbientPassUpdate();
}


void RenderSystem::DrawAllRenderers(
    std::unordered_map<unsigned long long, Animator>& animators,
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, ModelRenderer>& renderers,
    std::unordered_set<unsigned long long> animatedRenderered,
    Shader* givenShader)
{
    //Material* previousMaterial = nullptr;

    for (auto& i : renderers)
    {
        Transform* transform = &transforms.at(i.first);
        i.second.Draw(transform->getGlobalMatrix(), givenShader);
    }
}

void RenderSystem::ActivateFlaggedVariables(
    Shader* shader,
    Material* mat
)
{
    int flag = shader->getFlag();
    shader->Use();

    if (flag & Shader::Flags::Spec)
    {
        glActiveTexture(GL_TEXTURE0 + 7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

        glActiveTexture(GL_TEXTURE0 + 11);
        glBindTexture(GL_TEXTURE_2D, ssaoBluredBuffer);
    }
    if (flag & Shader::Flags::Painted)
    {
        glActiveTexture(GL_TEXTURE0 + 10);
        glBindTexture(GL_TEXTURE_2D, paintStrokeTexture->GLID);
    }
}

void RenderSystem::HDRBufferSetUp()
{
    glGenFramebuffers(1, &hdrFBO);
    glGenTextures(1, &colorBuffer);
    glGenRenderbuffers(1, &rboDepth);

    HDRBufferUpdate();
}

void RenderSystem::OutputBufferSetUp()
{
    glGenFramebuffers(1, &outputFBO);
    glGenTextures(1, &outputTexture);

    OutputBufferUpdate();
}

void RenderSystem::IBLBufferSetup(unsigned int skybox)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    SetIrradianceMap(skybox);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

    //glViewport(0, 0, 512, 512);

    //(*shaders)[brdf]->Use();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    (*shaders)[ShaderIndex::super]->Use();

    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::GUI()
{
    if (!ImGui::Begin("Render System", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }
    if (ImGui::CollapsingHeader("SSAO")) {
        ImGui::DragInt("Kernal Size", &kernelSize);
        ImGui::DragFloat("Radius", &ssaoRadius);
        ImGui::DragFloat("Bias", &ssaoBias);
    }
    ImGui::Combo("Buffer Index", &bufferIndex, "Result\0Albedo\0World Normals\0Emission\0PBR (Roughness, Metallic, AO)\0Roughness\0Metallic\0AO\0SSAO\0Bloom\0\0");

    ImGui::DragFloat("Exposure", &exposure, 0.01f, 0.0f, 5.0f);

    ImGui::Checkbox("Post Effect on", &postEffectOn);
    if (!postEffectOn) { ImGui::BeginDisabled(); }
    ImGui::SliderFloat("Tone Mapping Slider", &postEffectPercent, 0.0f, 1.0f);
    if (!postEffectOn) { ImGui::EndDisabled(); }
    ImGui::End();
}

void RenderSystem::BloomSetup()
{
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glm::vec2 mipSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    glm::ivec2 mipIntSize((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);
    for (GLuint i = 0; i < bloomMipMapCount; i++)
    {
        bloomMip mip;

        mipSize *= 0.5f;
        mipIntSize /= 2;
        mip.size = mipSize;
        mip.intSize = mipIntSize;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
            (int)mipSize.x, (int)mipSize.y,
            0, GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        bloomMips.push_back(mip);
    }
}

void RenderSystem::RenderBloom(unsigned int srcTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    RenderDownSamples(srcTexture);

    RenderUpSamples((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void RenderSystem::RenderDownSamples(unsigned int srcTexture)
{
    (*shaders)[downSample]->Use();

    // Bind srcTexture (HDR color buffer) as initial texture input
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    glm::vec2 inverseRes = { 1.0f / (float)SCREEN_WIDTH, 1.0f / (float)SCREEN_HEIGHT };

    (*shaders)[downSample]->setInt("mipLevel", 0);
    (*shaders)[downSample]->setVec2("srcResolution", inverseRes);

    glDisable(GL_BLEND);

    // Progressively downsample through the mip chain
    for (int i = 0; i < (int)bloomMips.size(); i++)
    {
        const bloomMip& mip = bloomMips[i];
        glViewport(0, 0, (int)mip.size.x, (int)mip.size.y);
        inverseRes = 1.0f / mip.size;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        RenderQuad();

        // Set current mip resolution as srcResolution for next iteration
        (*shaders)[downSample]->setVec2("srcResolution", inverseRes);

        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Disable Karis average for consequent downsamples
        if (i == 0) {
            (*shaders)[downSample]->setInt("mipLevel", 1);
        }
    }
}

void RenderSystem::RenderUpSamples(float aspectRatio)
{
    (*shaders)[upSample]->Use();
    (*shaders)[upSample]->setFloat("aspectRatio", aspectRatio);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (int i = (int)bloomMips.size() - 1; i > 0; i--)
    {
        const bloomMip& mip = bloomMips[i];
        const bloomMip& nextMip = bloomMips[i - 1];

        // Bind viewport and texture from where to read
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, nextMip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        RenderQuad();
    }
    glDisable(GL_BLEND);
}

void RenderSystem::DeferredSetup()
{
    glGenTextures(1, &normalBuffer);
    glGenTextures(1, &albedoBuffer);
    glGenTextures(1, &emissionBuffer);
    glGenTextures(1, &bloomBuffer);
    glGenTextures(1, &depthBuffer);
    glGenFramebuffers(1, &deferredFBO);
    DeferredUpdate();
}

void RenderSystem::AmibentPassSetup()
{
    glGenTextures(1, &ambientPassBuffer);
    glGenFramebuffers(1, &ambientPassFBO);
    AmbientPassUpdate();
    ambientPassShaderIndex = (*shaders).size();
    (*shaders).push_back(ResourceManager::LoadShaderDefaultVert("ambient"));
}

void RenderSystem::RenderQuad()
{
    glDisable(GL_CULL_FACE);

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
}

void RenderSystem::SSAOSetup()
{
    glGenTextures(1, &ssaoColorBuffer);
    glGenFramebuffers(1, &ssaoFBO);

    glGenTextures(1, &ssaoBluredBuffer);
    glGenFramebuffers(1, &ssaoBlurFBO);

    Shader* ssaoShader = (*shaders)[ssao];
    ssaoShader->Use();
    ssaoShader->setInt("depth", 1);
    ssaoShader->setInt("normalColour", 2);
    ssaoShader->setInt("texNoise", 3);

    SSAOUpdate();

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = Utilities::Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderSystem::RenderAmbientPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ambientPassFBO);
    
    Shader* ambientShader = (*shaders)[ambientPassShaderIndex];

    ambientShader->Use();
    ambientShader->setInt("screenDepth", 1);
    ambientShader->setInt("screenAlbedo", 2);
    ambientShader->setInt("screenNormal", 3);
    ambientShader->setInt("screenEmission", 4);
    ambientShader->setInt("screenSSAO", 5);
    ambientShader->setInt("skybox", 6);
    ambientShader->setInt("irradianceMap", 7);

    ambientShader->setMat4("invP", glm::inverse(projection));
    ambientShader->setMat4("invV", glm::inverse(viewMatrix));

    DirectionalLight* dirLight = (DirectionalLight*)SceneManager::scene->lights[0];
    ambientShader->setVec3("lightDirection", dirLight->direction);
    ambientShader->setVec3("lightColour", dirLight->colour);
    ambientShader->setVec3("camPos", SceneManager::camera.transform.getGlobalPosition());

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, albedoBuffer);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);

    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, emissionBuffer);

    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, ssaoBluredBuffer);

    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    glActiveTexture(GL_TEXTURE0 + 7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::RenderSSAO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    //SSAO Pre Blur
    Shader* ssaoShader = (*shaders)[ssao];
    ssaoShader->Use();

    // Send kernel + rotation 
    for (unsigned int i = 0; i < 64; ++i)
        ssaoShader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    ssaoShader->setMat4("projection", projection);
    ssaoShader->setMat4("invP", glm::inverse(projection));
    ssaoShader->setMat4("invV", glm::inverse(viewMatrix));
    ssaoShader->setMat4("view", viewMatrix);

    ssaoShader->setInt("kernelSize", kernelSize);
    ssaoShader->setFloat("radius", ssaoRadius);
    ssaoShader->setFloat("bias", ssaoBias);

    ssaoShader->setFloat("near", SceneManager::camera.nearPlane);
    ssaoShader->setFloat("far", SceneManager::camera.farPlane);

    ssaoShader->setVec2("noiseScale", { SCREEN_WIDTH / 4.0, SCREEN_HEIGHT / 4.0 });

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

    Shader* blur = (*shaders)[ssaoBlur];
    blur->Use();
    blur->setInt("SSAO", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}