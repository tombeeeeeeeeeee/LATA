#include "RenderSystem.h"


#include "SceneManager.h"
#include "Scene.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
#include "PointLight.h"
#include "ResourceManager.h"
#include "Light.h"
#include "Transform.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "Paths.h"
#include "SceneObject.h"
#include "PixelSimulation.h"
#include "Mesh.h"

#include "Utilities.h"
#include "EditorGUI.h"

#include <iostream>

LineRenderer RenderSystem::lines;
LineRenderer RenderSystem::debugLines;

void RenderSystem::Start()
{
    std::srand(2);
    if (SCREEN_WIDTH == 0)
    {
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        SCREEN_WIDTH = scrWidth;
        SCREEN_HEIGHT = scrHeight;
    }

    DeferredSetup();
    CompositeBufferSetUp();
    OutputBufferSetUp();
    BloomSetup();
    SSAOSetup();
    LightPassSetup();
    LinesSetup();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    ResourceManager::shadowDebug->Use();
    ResourceManager::shadowDebug->setInt("depthMap", 1);

    ResourceManager::lines->Use();
    lines.Initialise();
    debugLines.Initialise();

    ResourceManager::BindFlaggedVariables();

    postProcess = ResourceManager::LoadShader("Shaders/screenShader.vert", "Shaders/screenShader.frag");
    
    postFrameTexture = ResourceManager::CreateTexture(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_LINEAR, GL_LINEAR);
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
    
    beamShader = ResourceManager::LoadShader("beam");
    syncAimShader = ResourceManager::LoadShader("syncArrow");
}

void RenderSystem::LevelLoad()
{
    if (roomAmbience) roomAmbience->DeleteTexture();
    roomAmbience = ResourceManager::LoadTexture("Levels/" + SceneManager::scene->windowName + "_Ambience.png", Texture::Type::count, GL_CLAMP_TO_EDGE);
    if (!roomAmbience->loaded)
        roomAmbience = ResourceManager::LoadTexture("Levels/" + SceneManager::scene->windowName + ".png", Texture::Type::count, GL_CLAMP_TO_EDGE);
}

void RenderSystem::PlayStart(std::unordered_map<unsigned long long, PointLight>& pointLights, std::unordered_map<unsigned long long, Spotlight>& spotlights)
{

    for (auto& pair : pointLights)
    {
        if (pair.second.effect == PointLightEffect::Off)
        {
            pair.second.timeInType = 1.0f;
        }
        else
        {
            pair.second.timeInType = 0.0f;
        }
    }
}


void RenderSystem::DeferredUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

    glBindTexture(GL_TEXTURE_2D, normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, albedoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, emissionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, pbrBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, albedoBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emissionBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, pbrBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

    auto whatever = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (whatever != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer is not complete!" << "\n";
    }
}


void RenderSystem::LightPassUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, lightPassFBO);

    glBindTexture(GL_TEXTURE_2D, lightPassBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, lightPassFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightPassBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

    auto whatever = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (whatever != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer is not complete!" << "\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::LinesUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, linesFBO);

    glBindTexture(GL_TEXTURE_2D, linesBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, linesFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, linesBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

    auto whatever = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (whatever != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer is not complete!" << "\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderSystem::CompositeBufferUpdate()
{
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, bloomBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        glDeleteTextures(1, &bloomMips[i].texture);
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
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, PointLight>& pointLights,
    std::unordered_map<unsigned long long, Spotlight>& spotlights,
    Camera* camera,
    float delta,
    Pixels::Simulation& pixelSim,
    FrameBuffer* chunkFrameBuffer,
    Shader* pixelShader,
    Mesh& quad,
    Shader* simple2dShader,
    Texture* chunkTexture
)
{
    frameCountInSixteen = (frameCountInSixteen + 1) % 16;

    std::unordered_set<unsigned long long> animatedRenderered = {};


    // TODO: There are a few issues when the res is too low
    if (SCREEN_WIDTH <= 64 || SCREEN_HEIGHT <= 64) { return; }

    // TODO: rather then constanty reloading the framebuffer, the texture could link to the framebuffers that need assoisiate with it? or maybe just refresh all framebuffers when a texture is loaded?
    unsigned int deferredAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    viewMatrix = camera->GetViewMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //DrawAllRenderers(animators, transforms, renders, animatedRenderered, (*shaders)[shadowMapDepth]);

    // Render scene with shadow map, to the screen framebuffer
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDrawBuffers(4, deferredAttachments);

    glCullFace(GL_BACK);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glDisable(GL_DEPTH_TEST);

    DrawPixelSim(pixelSim, camera, chunkFrameBuffer, pixelShader, quad, simple2dShader, chunkTexture, deferredFBO);



    //RenderSSAO();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glBlendEquation(GL_FUNC_ADD);
    glDepthMask(GL_FALSE);

    glBindFramebuffer(GL_FRAMEBUFFER, lightPassFBO);

    glClear(GL_COLOR_BUFFER_BIT);

    //RenderPointLights(pointLights, transforms, delta);
    RenderAmbientPass();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);

    glDepthMask(GL_TRUE);

    RenderBeams(delta);

    RenderLinePass();

    RenderComposite();

    glDisable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    RenderBloom(bloomBuffer);

    // Unbind framebuffer
    //FrameBuffer::Unbind();
    glDisable(GL_DEPTH_TEST); // Disable depth test for fullscreen quad


    //FrameBuffer Rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //HDR
    

    ResourceManager::screen->Use();

    ResourceManager::screen->setInt("colour", 1);
    ResourceManager::screen->setInt("albedo", 2);
    ResourceManager::screen->setInt("normal", 3);
    ResourceManager::screen->setInt("emission", 4);
    ResourceManager::screen->setInt("SSAO", 5);
    ResourceManager::screen->setInt("bloomBlur", 6);
    ResourceManager::screen->setInt("lightBuffer", 7);
    ResourceManager::screen->setInt("pbr", 8);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

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

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, lightPassBuffer);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, pbrBuffer);


    ResourceManager::screen->setFloat("exposure", exposure);
    ResourceManager::screen->setInt("bufferIndex", bufferIndex);

    if (postEffectOn) {
        postFrameBuffer->Bind();
    }
    else {
        glViewport(0, 0, SCREEN_WIDTH / superSampling, SCREEN_HEIGHT / superSampling);
    }
    
    RenderQuad();

    if (postEffectOn) {
        FrameBuffer::Unbind();

        glViewport(0, 0, SCREEN_WIDTH / superSampling, SCREEN_HEIGHT / superSampling);

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

void RenderSystem::DrawPixelSim(
    const Pixels::Simulation& pixelSim, 
    const Camera* camera,
    FrameBuffer* chunkFrameBuffer,
    Shader* pixelShader,
    Mesh& quad,
    Shader* simple2dShader,
    Texture* chunkTexture,
    unsigned int defaultFrameBuffer
    )
{
    // Update the gpu version in preperation to send to GPU
    pixelSim.PrepareDraw(camera->transform.getPosition().x, camera->transform.getPosition().y);

    for (auto& i : pixelSim.getChunks())
    {
        chunkFrameBuffer->Bind();
        glViewport(0, 0, Pixels::chunkWidth, Pixels::chunkHeight);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, i.ssbo);


        // Draw pixels
        pixelShader->Use();
        quad.Draw();
        
        FrameBuffer::Unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        simple2dShader->Use();
        simple2dShader->setMat4("vp", SceneManager::viewProjection);
        glm::mat4 model = glm::mat4(0.5f);
        model = glm::translate(model, glm::vec3(i.x * 2.0f + 1.0f, i.y * 2.0f + 1.0f, 0.0f));
        simple2dShader->setMat4("model", model);
        chunkTexture->Bind(1);
        simple2dShader->setSampler("tex", 1);
        quad.Draw();

        //break;
    }

    //RenderLinePass();
    //RenderComposite();


    // Note: The framebuffer is not required for the current set up, is here for the sake of it at the moment
    // Bind framebuffer for drawing pixels



    // Unbind framebuffer
    // TODO: Is there a rendersystem function for this, if not maybe there should be something similar

    ;
    //// Draw framebuffer texture to screen
    //overlayShader->Use();
    //overlayShader->setFloat("material.alpha", 1.0f);

    //texture->Bind(1);
    //overlayShader->setSampler("material.albedo", 1);

    //quad.Draw();


}

void RenderSystem::SSAOUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glBindTexture(GL_TEXTURE_2D, ssaoBluredBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBluredBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::ScreenResize(int width, int height)
{
    SCREEN_HEIGHT = height * superSampling;
    SCREEN_WIDTH = width * superSampling;

    postFrameBuffer->setWidthHeight(SCREEN_WIDTH, SCREEN_HEIGHT);
    postFrameTexture->setWidthHeight(SCREEN_WIDTH, SCREEN_HEIGHT);
    postFrameBuffer->Load();

    DeferredUpdate();
    LightPassUpdate();
    LinesUpdate();
    SSAOUpdate();
    CompositeBufferUpdate();
    OutputBufferUpdate();
    BloomUpdate();
}


void RenderSystem::RenderPointLights(
std::unordered_map<unsigned long long, PointLight>& pointLights,
std::unordered_map<unsigned long long, Transform>& transforms,
float delta
)
{
    glBindFramebuffer(GL_FRAMEBUFFER, lightPassFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glCullFace(GL_FRONT);
    pointLightPassShader->Use();
    pointLightPassShader->setMat4("vp", projection * viewMatrix);
    pointLightPassShader->setMat4("invVP", glm::inverse(projection * viewMatrix));
    pointLightPassShader->setMat4("view", viewMatrix);
    pointLightPassShader->setMat4("proj", projection);
    pointLightPassShader->setMat4("invV", glm::inverse(viewMatrix));
    pointLightPassShader->setMat4("invP", glm::inverse(projection));
    pointLightPassShader->setVec2("invViewPort", glm::vec2(1.0f/SCREEN_WIDTH, 1.0f/SCREEN_HEIGHT));
    pointLightPassShader->setVec3("camPos", SceneManager::camera.transform.getGlobalPosition());
    pointLightPassShader->setInt("albedo", 1);
    pointLightPassShader->setInt("normal", 2);
    pointLightPassShader->setInt("depth", 3);
    pointLightPassShader->setInt("pbr", 4);
    pointLightPassShader->setInt("lightLerp", 5);
    pointLightPassShader->setInt("SSAO", 6);
    pointLightPassShader->setInt("frameCount", frameCountInSixteen);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, albedoBuffer);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);

    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, pbrBuffer);

    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_2D, ssaoBluredBuffer);

    for (auto& pair : pointLights)
    {
        Transform transform = Transform();
        transform.setPosition(transforms[pair.first].getGlobalPosition());
        transform.setScale(pair.second.range * 900.0f);
        pointLightPassShader->setMat4("model", transform.getGlobalMatrix());
        pointLightPassShader->setVec3("lightPos", transforms[pair.first].getGlobalPosition());
        pointLightPassShader->setVec3("colour", pair.second.colour * pair.second.intensity);
        pointLightPassShader->setFloat("linear", pair.second.linear);
        pointLightPassShader->setFloat("quad", pair.second.quadratic);
        pointLightPassShader->setBool("castsShadows", pair.second.castsShadows);
        pair.second.timeInType += delta;
        float lerpAmount;
        switch (pair.second.effect)
        {
        case PointLightEffect::On:
            pair.second.timeInType = glm::clamp(pair.second.timeInType, 0.0f, lightTimeToOn);
            lerpAmount = pair.second.timeInType / lightTimeToOn;
            break;

        case PointLightEffect::OffDelete:
            if (pair.second.timeInType >= lightTimeToOff) SceneManager::scene->DeleteSceneObjectAndChildren(pair.first);
        case PointLightEffect::Off:
            pair.second.timeInType = glm::clamp(pair.second.timeInType, 0.0f, lightTimeToOff);
            lerpAmount = pair.second.timeInType / lightTimeToOff;
            break;

        case PointLightEffect::Explosion:
            pair.second.timeInType = glm::clamp(pair.second.timeInType, 0.0f, lightTimeToExplode);
            if (pair.second.timeInType >= lightTimeToExplode) SceneManager::scene->DeleteSceneObjectAndChildren(pair.first);
            lerpAmount = pair.second.timeInType / lightTimeToExplode;
            break;

        case PointLightEffect::Flickering:
            pair.second.timeInType = fmod(pair.second.timeInType, lightTimeToFlicker);
            lerpAmount = fmod(pair.second.timeInType + (pair.first % 100) / 100.0f, lightTimeToFlicker) / lightTimeToFlicker;
            break;

        case PointLightEffect::SpotLightSpawnRoom:
            pair.second.timeInType = glm::clamp(pair.second.timeInType, 0.0f, lightTimeToOn);
            lerpAmount = pair.second.timeInType / lightTimeToOn;
        pointLightPassShader->setFloat("lerpAmount", glm::clamp(lerpAmount, 0.0f, 1.0f));
    }
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);

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
}

void RenderSystem::CompositeBufferSetUp()
{
    glGenFramebuffers(1, &compositeFBO);
    glGenTextures(1, &colorBuffer);
    glGenTextures(1, &bloomBuffer);
    compositeShader = ResourceManager::LoadShaderDefaultVert("composite");

    CompositeBufferUpdate();
}

void RenderSystem::RenderComposite()
{

    unsigned int compositeAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawBuffers(2, compositeAttachments);

    compositeShader->Use();
    compositeShader->setInt("lines", 1);
    compositeShader->setInt("lights", 2);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, linesBuffer);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, lightPassBuffer);

    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::OutputBufferSetUp()
{
    glGenFramebuffers(1, &outputFBO);
    glGenTextures(1, &outputTexture);

    OutputBufferUpdate();
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
    ImGui::Combo("Buffer Index", &bufferIndex,
        "Result\0Albedo\0World Normals\0Emission\0PBR (Roughness, Metallic, AO)\0Roughness\0Metallic\0AO\0SSAO\0Bloom\0Colour\0Lights\0\0");
    ImGui::DragFloat("Exposure", &exposure, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Ambient Light Intensity", &ambientIntensity, 0.1f, 0);
    ImGui::Text("Light Speeds");
    ImGui::DragFloat("On Light Time", &lightTimeToOn);
    ImGui::DragFloat("Off Light Time", &lightTimeToOff);
    ImGui::DragFloat("Exploding Light Time", &lightTimeToExplode);
    ImGui::DragFloat("Flicker Light Time", &lightTimeToFlicker);

    int previousSuper = superSampling;
    if (ImGui::DragInt("Super Sampling", &superSampling, 0.2f, 1, 4)) {
        ScreenResize(SCREEN_WIDTH / previousSuper, SCREEN_HEIGHT / previousSuper);
    }
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
    glClear(GL_COLOR_BUFFER_BIT);
    RenderDownSamples(srcTexture);

    RenderUpSamples((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void RenderSystem::RenderDownSamples(unsigned int srcTexture)
{
    ResourceManager::downSample->Use();

    // Bind srcTexture (HDR color buffer) as initial texture input
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    glm::vec2 inverseRes = { 1.0f / (float)SCREEN_WIDTH, 1.0f / (float)SCREEN_HEIGHT };

    ResourceManager::downSample->setInt("mipLevel", 0);
    ResourceManager::downSample->setVec2("srcResolution", inverseRes);

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
        ResourceManager::downSample->setVec2("srcResolution", inverseRes);

        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Disable Karis average for consequent downsamples
        if (i == 0) {
            ResourceManager::downSample->setInt("mipLevel", 1);
        }
    }
}

void RenderSystem::RenderUpSamples(float aspectRatio)
{
    ResourceManager::upSample->Use();
    ResourceManager::upSample->setFloat("aspectRatio", aspectRatio);

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
    glGenTextures(1, &pbrBuffer);
    glGenTextures(1, &depthBuffer);
    glGenFramebuffers(1, &deferredFBO);
    DeferredUpdate();
}


void RenderSystem::LightPassSetup()
{
    glGenTextures(1, &lightPassBuffer);
    glGenFramebuffers(1, &lightPassFBO);
    LightPassUpdate();
    ambientPassShader = ResourceManager::LoadShaderDefaultVert("ambient");
    pointLightPassShader = ResourceManager::LoadShader("pointLight");
    spotlightPassShader = ResourceManager::LoadShader("spotlight");
    spotlightShadowPassShader = ResourceManager::LoadShader("spotlightShadows");
}

void RenderSystem::LinesSetup()
{
    glGenTextures(1, &linesBuffer);
    glGenFramebuffers(1, &linesFBO);
    LinesUpdate();
}

void RenderSystem::RenderLinePass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, linesFBO);
    ResourceManager::lines->Use();
    lines.Draw();

    glDepthFunc(GL_ALWAYS);
    debugLines.Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    Shader* ssaoShader = ResourceManager::ssao;
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
    glBindFramebuffer(GL_FRAMEBUFFER, lightPassFBO);
    
    ambientPassShader->Use();
    ambientPassShader->setInt("screenDepth", 1);
    ambientPassShader->setInt("screenAlbedo", 2);
    ambientPassShader->setInt("screenNormal", 3);
    ambientPassShader->setInt("screenEmission", 4);
    ambientPassShader->setInt("screenSSAO", 5);
    ambientPassShader->setInt("skybox", 6);
    ambientPassShader->setInt("roomLight", 7);

    ambientPassShader->setMat4("invP", glm::inverse(projection));
    ambientPassShader->setMat4("invV", glm::inverse(viewMatrix));

    DirectionalLight dirLight = SceneManager::scene->directionalLight;
    ambientPassShader->setVec3("lightDirection", glm::normalize(dirLight.direction));
    ambientPassShader->setVec3("lightColour", dirLight.colour);
    ambientPassShader->setVec3("camPos", SceneManager::camera.transform.getGlobalPosition());
    ambientPassShader->setVec2("mapMins", mapMin);
    ambientPassShader->setVec2("mapDimensions", mapDelta);
    ambientPassShader->setFloat("ambientIntensity", ambientIntensity);

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

    if (roomAmbience)
    {
        glActiveTexture(GL_TEXTURE0 + 7);
        glBindTexture(GL_TEXTURE_2D, roomAmbience->GLID);
    }

    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderSystem::RenderBeams(float delta)
{
    glBindFramebuffer(GL_FRAMEBUFFER, linesFBO);
    glDepthMask(GL_FALSE);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    beamShader->Use();
    beamShader->setMat4("vp", projection * viewMatrix);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::RenderSSAO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    //SSAO Pre Blur
    Shader* ssaoShader = ResourceManager::ssao;
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

    Shader* blur = ResourceManager::ssaoBlur;
    blur->Use();
    blur->setInt("SSAO", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}