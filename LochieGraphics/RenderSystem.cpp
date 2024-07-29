#include "RenderSystem.h"
#include "ResourceManager.h"
#include "Maths.h"

RenderSystem::RenderSystem(GLFWwindow* _window)
{
    window = _window;
}

void RenderSystem::Start(
    unsigned int _skyboxTexture,
    std::vector<Shader*>* _shaders,
    Light* _shadowCaster
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

    //cube.InitialiseCube(2.0f)

    IBLBufferSetup(skyboxTexture);
    HDRBufferSetUp();
    OutputBufferSetUp();
    BloomSetup();
    shadowCaster = _shadowCaster;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    shadowDebugQuad.InitialiseQuad(0.5f, 0.5f);
    screenQuad.InitialiseQuad(1.f, 0.0f);
    //cube.InitialiseCube(2.0f);

    // Create colour attachment texture for fullscreen framebuffer
    screenColourBuffer = ResourceManager::LoadTexture(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_LINEAR, GL_LINEAR);

    // Make fullscreen framebuffer
    screenFrameBuffer = new FrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, screenColourBuffer, nullptr, true);

    // TODO: Should be done for each light
    // Create shadow depth texture for the light
    depthMap = ResourceManager::LoadTexture(shadowCaster->shadowTexWidth, shadowCaster->shadowTexHeight, GL_DEPTH_COMPONENT, nullptr, GL_CLAMP_TO_BORDER, GL_FLOAT, false, GL_NEAREST, GL_NEAREST);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // TODO: Move to be apart of texture
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    shadowFrameBuffer = new FrameBuffer(shadowCaster->shadowTexWidth, shadowCaster->shadowTexHeight, nullptr, depthMap, false);
    (*shaders)[ShaderIndex::shadowDebug]->Use();
    (*shaders)[ShaderIndex::shadowDebug]->setInt("depthMap", 1);
    (*shaders)[ShaderIndex::super]->Use();
    (*shaders)[ShaderIndex::super]->setInt("irradianceMap", 7);
    (*shaders)[ShaderIndex::super]->setInt("prefilterMap", 8);
    (*shaders)[ShaderIndex::super]->setInt("brdfLUT", 9);

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

        RenderCube();
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

    unsigned int currShader = (*shaders)[ShaderIndex::prefilter]->GLID;
    glUseProgram(currShader);

    glUniform1i(glGetUniformLocation(currShader, "environmentMap"), 1);
    glUniformMatrix4fv(glGetUniformLocation(currShader, "projection"), 1, GL_FALSE, &captureProjection[0][0]);

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
        glUniform1f(glGetUniformLocation(currShader, "roughness"), roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            //ColouredOutput("view in prefilter is: ", Colour::green);
            //ColouredOutput(glGetUniformLocation(currShader, "view") == -1, Colour::red, false);
            glUniformMatrix4fv(glGetUniformLocation(currShader, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCube();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
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
    shadowFrameBuffer->Load();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //TODO: TO make more flexible?
	// Render depth of scene to texture (from light's perspective)
	glm::mat4 lightSpaceMatrix;
	lightSpaceMatrix = shadowCaster->getShadowViewProjection();
	
	(*shaders)[shadowMapDepth]->Use();
    (*shaders)[shadowMapDepth]->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, shadowCaster->shadowTexWidth, shadowCaster->shadowTexHeight);
	shadowFrameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //RENDER SCENE

    for(auto i = shadowCasters.begin(); i != shadowCasters.end(); i++)
    {
        ModelRenderer currentRenderer = i->second;
        Texture* alphaMap = currentRenderer.material->getFirstTextureOfType(Texture::Type::diffuse);
        if (!alphaMap) {
            alphaMap = currentRenderer.material->getFirstTextureOfType(Texture::Type::albedo);
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
            mesh->Draw();
        }
    }

    DrawAnimation(animators, transforms, shadowCasters, (*shaders)[shadowMapDepth]);
    
    //TODO: LEARN SKYBOX DRAW
    //skyBox->Draw();
    //glCullFace(GL_BACK);

    // Render scene with shadow map, to the screen framebuffer
    screenFrameBuffer->Bind();

    // TODO: move viewport changing stuff into FrameBuffer
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    (*shaders)[ShaderIndex::shadowMapping]->Use();

    // set light uniforms
    (*shaders)[ShaderIndex::shadowMapping]->setVec3("viewPos", camera->position);
    (*shaders)[ShaderIndex::shadowMapping]->setVec3("lightPos", shadowCaster->getPos());
    (*shaders)[ShaderIndex::shadowMapping]->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    //TODO:
    depthMap->Bind(17);
    (*shaders)[ShaderIndex::shadowMapping]->setSampler("shadowMap", 17);

    (*shaders)[ShaderIndex::super]->Use();

    // set light uniforms
    // TODO: Shouldn't need to set light uniforms here, use the shader flags and make one for shadowed
    (*shaders)[ShaderIndex::super]->setVec3("viewPos", camera->position);
    (*shaders)[ShaderIndex::super]->setVec3("lightPos", shadowCaster->getPos());
    (*shaders)[ShaderIndex::super]->setMat4("directionalLightSpaceMatrix", lightSpaceMatrix);

    (*shaders)[ShaderIndex::super]->setSampler("shadowMap", 17);
    //TODO:
    depthMap->Bind(17);
    // RENDER SCENE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawRenderers(renders, transforms);

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

    if (showShadowDebug) {
        // Debug render the light depth map
        (*shaders)[ShaderIndex::shadowDebug]->Use();
        (*shaders)[ShaderIndex::shadowDebug]->setFloat("near_plane", shadowCaster->shadowNearPlane);
        (*shaders)[ShaderIndex::shadowDebug]->setFloat("far_plane", shadowCaster->shadowFarPlane);
        depthMap->Bind(1);

        //TODO: Make Shadow Debug Quad
        DrawMesh(shadowDebugQuad);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Unbind framebuffer
    //FrameBuffer::Unbind();
    glDisable(GL_DEPTH_TEST); // Disable depth test for fullscreen quad


    //screenFrameBuffer->Bind();

    //FrameBuffer Rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //HDR
    glUseProgram((*shaders)[ShaderIndex::screen]->GLID);

    glUniform1i(glGetUniformLocation((*shaders)[ShaderIndex::screen]->GLID, "scene"), 1);
    glUniform1i(glGetUniformLocation((*shaders)[ShaderIndex::screen]->GLID, "bloomBlur"), 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, bloomMips[0].texture);

    glUniform1f(glGetUniformLocation((*shaders)[ShaderIndex::screen]->GLID, "exposure"), exposure);

    RenderQuad();

    screenFrameBuffer->Unbind();
    // Re enable the depth test
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::ScreenResize(int width, int height)
{
    SCREEN_HEIGHT = height;
    SCREEN_WIDTH = width;

    screenColourBuffer->setWidthHeight((int)width, (int)height);
    screenFrameBuffer->setWidthHeight(width, height);

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
            animationRenderer.material->Use();
            animationRenderer.material->getShader()->setMat4("model", transforms[iter->first].getGlobalMatrix());
        }
        else 
        {
        	shader->setMat4("model", transforms[iter->first].getGlobalMatrix());
        }

        Model* model = animationRenderer.model;
        for (auto mesh = model->meshes.begin(); mesh != model->meshes.end(); mesh++)
        {
            mesh->Draw();
        }
    }
}

void RenderSystem::DrawRenderers(
    std::unordered_map<unsigned long long, ModelRenderer>& renderers, 
    std::unordered_map<unsigned long long, Transform>& transforms
)
{
    for (auto i = renderers.begin(); i != renderers.end(); i++)
    {
        i->second.material->Use();
        Shader* curShader = i->second.material->getShader();
        curShader->setMat4("model", transforms[i->first].getGlobalMatrix());
        int samplerCount = i->second.material->texturePointers.size();
        if (curShader->getFlag(Shader::Flags::Spec))
        {
            glActiveTexture(GL_TEXTURE0 + 7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

            glActiveTexture(GL_TEXTURE0 + 8);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

            glActiveTexture(GL_TEXTURE0 + 9);
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        }

        glUniform3fv(glGetUniformLocation(curShader->GLID, "materialColour"), 1, &i->second.material->colour[0]);


        Model* model = i->second.model;
        for (auto mesh = model->meshes.begin(); mesh != model->meshes.end(); mesh++)
        {
            mesh->Draw();
        }
    }
}

void RenderSystem::DrawMesh(Mesh& mesh)
{
    glBindVertexArray(mesh.getVAO());
    if (mesh.getIBO() != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBO());
        glDrawElements(GL_TRIANGLES, 3 * mesh.getTriCount(), GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, 3 * mesh.getTriCount());
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    unsigned int currShader = (*shaders)[ShaderIndex::brdf]->GLID;
    glUseProgram(currShader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glUseProgram((*shaders)[ShaderIndex::super]->GLID);


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
    glUseProgram((*shaders)[ShaderIndex::downSample]->GLID);

    // Bind srcTexture (HDR color buffer) as initial texture input
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    glm::vec2 inverseRes = { 1.0f / (float)SCREEN_WIDTH, 1.0f / (float)SCREEN_HEIGHT };

    glUniform1i(glGetUniformLocation((*shaders)[ShaderIndex::downSample]->GLID, "mipLevel"), 0);

    glUniform2fv(glGetUniformLocation((*shaders)[ShaderIndex::downSample]->GLID, "srcResolution"), 1, &inverseRes[0]);

    glDisable(GL_BLEND);

    // Progressively downsample through the mip chain
    for (int i = 0; i < (int)bloomMips.size(); i++)
    {
        const bloomMip& mip = bloomMips[i];
        glViewport(0, 0, mip.size.x, mip.size.y);
        inverseRes = 1.0f / mip.size;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        RenderQuad();

        // Set current mip resolution as srcResolution for next iteration
        glUniform2fv(glGetUniformLocation((*shaders)[ShaderIndex::downSample]->GLID, "srcResolution"), 1, &inverseRes[0]);

        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Disable Karis average for consequent downsamples
        if (i == 0) glUniform1i(glGetUniformLocation((*shaders)[ShaderIndex::downSample]->GLID, "mipLevel"), 1);
    }

    glUseProgram(0);
}

void RenderSystem::RenderUpSamples(float aspectRatio)
{
    glUseProgram((*shaders)[ShaderIndex::upSample]->GLID);
    glUniform1f(glGetUniformLocation((*shaders)[ShaderIndex::upSample]->GLID, "aspectRatio"), aspectRatio);

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
    glUseProgram(0);
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

void RenderSystem::RenderCube()
{
    // initialize (if necessary)
    if (cube.getVAO() == 0) {
        cube.InitialiseCube();
    }
    // render Cube
    cube.Draw();
    return;
}
