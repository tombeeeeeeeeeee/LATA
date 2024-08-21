#pragma once
#include "Light.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "Animator.h"
#include "Camera.h"
#include "Mesh.h"
#include "Skybox.h"
#include "FrameBuffer.h"
#include "ShaderEnum.h"

#include "LineRenderer.h"
#include "Graphics.h"

#include <iostream>
#include <unordered_map>

struct bloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    unsigned int texture;
};

const int bloomMipMapCount = 6;

class RenderSystem
{
public:

    LineRenderer lines;

    RenderSystem(GLFWwindow* window);

    void Start(
        unsigned int _skyboxTexture,
        std::vector<Shader*>* _shaders,
        Light* shadowCaster,
        std::string paintStrokeTexturePath
    );

    void SetShaders(std::vector<Shader*>* _shaders) { shaders = _shaders; };

    void SetIrradianceMap(unsigned int skybox);
    void SetPrefilteredMap(unsigned int skybox);

    float exposure = 1.0f;

    void HDRBufferUpdate();
    void OutputBufferUpdate();
    void BloomUpdate(); 

    void Update(
        std::unordered_map<unsigned long long, ModelRenderer>& renders,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& shadowCasters,
        std::unordered_map<unsigned long long, Animator>& animators,
        Camera* camera
    );

    void ScreenResize(int width, int height);

    void IBLBufferSetup(unsigned int skybox);

    int SCREEN_WIDTH, SCREEN_HEIGHT;

    bool showShadowDebug = false;
    unsigned int skyboxTexture;

private:

    Mesh* shadowDebugQuad;
    Mesh* screenQuad;
    Mesh* buttonQuad;

    Texture* screenColourBuffer = nullptr;

    FrameBuffer* screenFrameBuffer = nullptr;
    //Texture* screenTexture = nullptr;

    FrameBuffer* shadowFrameBuffer = nullptr;
    Texture* depthMap = nullptr;

    Light* shadowCaster;

    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation;

    glm::mat4 projectionMatrix, viewMatrix;

    /// <summary>
    /// Pointer to current window
    /// </summary>
    GLFWwindow* window;

    /// <summary>
    /// entity id for camera.
    /// </summary>
    unsigned int cameraID;

    /// <summary>
    /// List of entities in order of the shader they use.
    /// </summary>
    std::vector<std::vector<unsigned int>> entityShaderOrder;

    /// <summary>
    /// shader programs
    /// </summary>
    std::vector<Shader*>* shaders;


    void DrawAnimation(
        std::unordered_map<unsigned long long, Animator>& animators,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& renderers,
        Shader* shader = nullptr
        );

    void DrawRenderers(
        std::unordered_map<unsigned long long, ModelRenderer>& renderers,
        std::unordered_map<unsigned long long, Transform>& transforms
        );



    void ActivateFlaggedVariables(
        Shader* shader,
        Material* mat
        );


    //void BindLightUniform(unsigned int shaderProgram,
    //    std::unordered_map<unsigned int, Light>& lightComponents,
    //    std::unordered_map<unsigned int, Transform>& transComponents);

    void HDRBufferSetUp();
    void OutputBufferSetUp();

    unsigned int hdrFBO;
    unsigned int bloomBuffer;
    unsigned int colorBuffer;
    unsigned int rboDepth;

    unsigned int outputFBO = 0;
    unsigned int outputTexture = 0;

    unsigned int captureFBO;
    unsigned int captureRBO;

    unsigned int irradianceMap = 0;
    unsigned int brdfLUTTexture = 0;
    unsigned int prefilterMap = 0;
    Texture* paintStrokeTexture = nullptr;

    void BloomSetup();
    unsigned int mFBO;
    std::vector<bloomMip> bloomMips;

    void RenderBloom(unsigned int srcTexture);

    void RenderDownSamples(unsigned int srcTexture);
    void RenderUpSamples(float aspectRatio);

    void RenderQuad();
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    Mesh* cube;

    //Captures for CubeMap
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
};

