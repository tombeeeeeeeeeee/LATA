#include "RenderSystem.h"

#include "ResourceManager.h"
#include "Light.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "Skybox.h"
#include "Animator.h"
#include "FrameBuffer.h"

#include "Utilities.h"

#include <random>

RenderSystem::RenderSystem(GLFWwindow* _window)
{
    window = _window;
}

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
        SCREEN_WIDTH  = scrWidth;
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
    ForwardSetup();

    shadowCaster = _shadowCaster;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    shadowDebugQuad = ResourceManager::LoadMesh();
    screenQuad = ResourceManager::LoadMesh();
    shadowDebugQuad->InitialiseQuad(0.5f, 0.5f);
    screenQuad->InitialiseQuad(1.f, 0.0f);

    // Create colour attachment texture for fullscreen framebuffer
    screenColourBuffer = ResourceManager::LoadTexture(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_LINEAR, GL_LINEAR);

    // Make fullscreen framebuffer
    screenFrameBuffer = new FrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, screenColourBuffer, nullptr, true);

    paintStrokeTexture = nullptr;//ResourceManager::LoadTexture(paintStrokeTexturePath, Texture::Type::paint);
    
    (*shaders)[ShaderIndex::shadowDebug]->Use();
    (*shaders)[ShaderIndex::shadowDebug]->setInt("depthMap", 1);

    (*shaders)[ShaderIndex::lines]->Use();
    lines.Initialise();

    shadowCaster = _shadowCaster;
    shadowCaster->Initialise();


    ResourceManager::BindFlaggedVariables();
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

void RenderSystem::SetPrefilteredMap(unsigned int textureID)
{
    glViewport(0, 0, 32, 32);
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    (*shaders)[ShaderIndex::prefilter]->Use();

    (*shaders)[ShaderIndex::prefilter]->setInt("environmentMap", 1);
    (*shaders)[ShaderIndex::prefilter]->setMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < 5; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        (*shaders)[ShaderIndex::prefilter]->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            //ColouredOutput("view in prefilter is: ", Colour::green);
            //ColouredOutput(glGetUniformLocation(currShader, "view") == -1, Colour::red, false);
            (*shaders)[ShaderIndex::prefilter]->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube->Draw();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void RenderSystem::ForwardUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);

    glBindTexture(GL_TEXTURE_2D, positionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // normal colour buffer

    glBindTexture(GL_TEXTURE_2D, normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
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

    // create bloom buffer
    glGenTextures(1, &bloomBuffer);
    glBindTexture(GL_TEXTURE_2D, bloomBuffer);
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
    Camera* camera
)
{
    // TODO: rather then constanty reloading the framebuffer, the texture could link to the framebuffers that need assoisiate with it? or maybe just refresh all framebuffers when a texture is loaded?
    shadowCaster->shadowFrameBuffer->Load();
    unsigned int forwardAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
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

    //RENDER SCENE FOR SHADOWS

    for(auto i = shadowCasters.begin(); i != shadowCasters.end(); i++)
    {
        // TODO: This is only using the first material found on the model, each mesh could potentially have a different material?
        ModelRenderer currentRenderer = i->second;
        if (currentRenderer.model == nullptr) { continue; }
        if (currentRenderer.materials[0] == nullptr) { continue; }
        Texture* alphaMap = currentRenderer.materials[0]->getFirstTextureOfType(Texture::Type::diffuse);
        if (!alphaMap) {
            alphaMap = currentRenderer.materials[0]->getFirstTextureOfType(Texture::Type::albedo);
        }
        if (alphaMap) {
            // TODO: Really should be using a Texture bind function here.
            alphaMap->Bind(1);
            (*shaders)[shadowMapDepth]->setSampler("alphaDiscardMap", 1);
        }
        else {
            // TODO:
            (*shaders)[shadowMapDepth]->setSampler("alphaDiscardMap", 0);
        }

        (*shaders)[shadowMapDepth]->setMat4("model", transforms[i->first].getGlobalMatrix());
       
        Model* model = currentRenderer.model;

        //DRAW USING SHADOW MAP FOR CURRENT TRANSFORM
        for (auto mesh = model->meshes.begin(); mesh != model->meshes.end(); mesh++)
        {
            (*mesh)->Draw();
        }
    }

    DrawAnimation(animators, transforms, shadowCasters, (*shaders)[shadowMapDepth]);
    
    // Render scene with shadow map, to the screen framebuffer
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDrawBuffers(2, forwardAttachments);
    DrawAnimation(animators, transforms, renders, (*shaders)[forward]);
    DrawRenderers(renders, transforms, (*shaders)[forward]);

    RenderSSAO();
    glDisable(GL_DEPTH_TEST);
    screenFrameBuffer->Bind();

    // TODO: move viewport changing stuff into FrameBuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);


    (*shaders)[ShaderIndex::super]->Use();

    // set light uniforms
    // TODO: Shouldn't need to set light uniforms here, use the shader flags and make one for shadowed
    (*shaders)[ShaderIndex::super]->setVec3("viewPos", camera->transform.getPosition());
    (*shaders)[ShaderIndex::super]->setVec3("lightPos", shadowCaster->getPos());
    (*shaders)[ShaderIndex::super]->setMat4("directionalLightSpaceMatrix", lightSpaceMatrix);

    (*shaders)[ShaderIndex::super]->setSampler("shadowMap", 17);
    //TODO:
    shadowCaster->depthMap->Bind(17);
    // RENDER SCENE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    
    glDrawBuffers(2, attachments);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawRenderers(renders, transforms);

    (*shaders)[ShaderIndex::lines]->Use();
    lines.Draw();

    glDepthFunc(GL_LEQUAL); // Change depth function
    Texture::UseCubeMap(skyboxTexture, (*shaders)[ShaderIndex::skyBoxShader]);
    
    RenderQuad();
    glDepthFunc(GL_LESS);

    // Draw animated stuff
    (*shaders)[ShaderIndex::super]->Use();
    glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, camera->nearPlane, camera->farPlane);
    (*shaders)[ShaderIndex::super]->setMat4("vp", projection * camera->GetViewMatrix());
    
    DrawAnimation(animators, transforms, renders, (*shaders)[ShaderIndex::super]);

    RenderBloom(bloomBuffer);



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Unbind framebuffer
    //FrameBuffer::Unbind();
    glDisable(GL_DEPTH_TEST); // Disable depth test for fullscreen quad


    //screenFrameBuffer->Bind();

    //FrameBuffer Rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //HDR

    (*shaders)[screen]->Use();

    (*shaders)[screen]->setInt("scene", 1);
    (*shaders)[screen]->setInt("bloomBlur", 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, bloomMips[0].texture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    (*shaders)[screen]->setFloat("exposure", exposure);

    RenderQuad();

    if (showShadowDebug) {
        // Debug render the light depth map
        (*shaders)[ShaderIndex::shadowDebug]->Use();
        (*shaders)[ShaderIndex::shadowDebug]->setFloat("near_plane", shadowCaster->shadowNearPlane);
        (*shaders)[ShaderIndex::shadowDebug]->setFloat("far_plane", shadowCaster->shadowFarPlane);
        shadowCaster->depthMap->Bind(1);

        //TODO: Make Shadow Debug Quad
        shadowDebugQuad->Draw();
    }


    screenFrameBuffer->Unbind();


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

    screenColourBuffer->setWidthHeight((int)width, (int)height);
    screenFrameBuffer->setWidthHeight(width, height);

    ForwardUpdate();
    SSAOUpdate();
    HDRBufferUpdate();
    OutputBufferUpdate();
    BloomUpdate();
}

void RenderSystem::DrawAnimation(
    std::unordered_map<unsigned long long, Animator>& animators,
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, ModelRenderer>& renderers,
    Shader* shader
)
{
    shader->Use();
    for (auto iter = animators.begin(); iter != animators.end(); iter++)
    {
        std::vector<glm::mat4> animations = iter->second.getFinalBoneMatrices();
        for (int i = 0; i < animations.size(); i++)
        {
            shader->setMat4("boneMatrices[" + std::to_string(i) + "]", animations[i]);
        }
        shader->setMat4("model", transforms[iter->first].getGlobalMatrix());

        ModelRenderer animationRenderer = renderers[iter->first];
        if (!shader)
        {
            // TODO: This is only using the first material found on the model, each mesh could potentially have a different material?
            animationRenderer.materials[0]->Use();
            animationRenderer.materials[0]->getShader()->setMat4("model", transforms[iter->first].getGlobalMatrix());
        }
        else 
        {
        	shader->setMat4("model", transforms[iter->first].getGlobalMatrix());
        }

        //Model* model = animationRenderer.model;
        //for (auto mesh = model->meshes.begin(); mesh != model->meshes.end(); mesh++)
        //{
        //    (*mesh)->Draw();
        //}
    }
}

void RenderSystem::DrawRenderers(
    std::unordered_map<unsigned long long, ModelRenderer>& renderers, 
    std::unordered_map<unsigned long long, Transform>& transforms,
    Shader* _shader
)
{
    for (auto i = renderers.begin(); i != renderers.end(); i++)
    {
        //// TODO: This is only using the first material found on the model, each mesh could potentially have a different material?
        //i->second.materials[0]->Use();
        //Shader* curShader = i->second.materials[0]->getShader();
        //curShader->setMat4("model", transforms[i->first].getGlobalMatrix());

        //// TODO: Is this supposed to be used for something?
        //// ASK:
        //int samplerCount = i->second.materials[0]->texturePointers.size();

        //ActivateFlaggedVariables(curShader, i->second.materials[0]);

        //// TODO: use shader function
        //curShader->setVec3("materialColour", i->second.materials[0]->colour);

        Model* model = i->second.model;
        //for (auto mesh = model->meshes.begin(); mesh != model->meshes.end(); mesh++)
        //{
        //    (*mesh)->Draw();
        //}

        Shader* prevShader = nullptr;
        int prevMaterialID = -1;
        if (model == nullptr) { continue; }
        for (auto mesh = model->meshes.begin(); mesh != model->meshes.end(); mesh++)
        {
            int materialID = (*mesh)->materialID;

            // Ensure that the materialID is valid
            if (materialID >= model->materialIDs || i->second.materials[materialID] == nullptr) {
                materialID = 0;
            }
            // Only bind textures if using a different material
            if (materialID != prevMaterialID) {
                if (i->second.materials[materialID] == nullptr) { continue; }
                i->second.materials[materialID]->Use(_shader ? _shader : nullptr);
            }
            // Only need to set shader variables if using a different shader
           
            Shader* shader;
            if (_shader == nullptr)
                shader = i->second.materials[materialID]->getShader();
            else
                shader = _shader;


            if (prevShader != shader) {
                shader->Use();
                prevShader = shader;
                shader->setMat4("view", viewMatrix);
                shader->setMat4("model", transforms[i->first].getGlobalMatrix());
                ActivateFlaggedVariables(shader, i->second.materials[materialID]);
                shader->setVec3("materialColour", i->second.materials[materialID]->colour);
            }
            (*mesh)->Draw();
        }
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

        glActiveTexture(GL_TEXTURE0 + 8);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

        glActiveTexture(GL_TEXTURE0 + 9);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

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

    SetPrefilteredMap(skybox);

    glGenTextures(1, &brdfLUTTexture);
    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);

    (*shaders)[brdf]->Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    (*shaders)[ShaderIndex::super]->Use();

    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
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

void RenderSystem::ForwardSetup()
{
    glGenTextures(1, &positionBuffer);
    glGenTextures(1, &normalBuffer);
    glGenFramebuffers(1, &forwardFBO);
    ForwardUpdate();
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
    ssaoShader->setInt("positionColour", 1);
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

    ssaoShader->setInt("kernelSize", kernelSize);
    ssaoShader->setFloat("radius", ssaoRadius);
    ssaoShader->setFloat("bias", ssaoBias);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, positionBuffer);
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
