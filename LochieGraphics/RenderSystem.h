#pragma once
#include "glad.h"
#include "Light.h"
#include "Transform.h"
#include "ModelRenderer.h"


#include "unordered_map"

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
    void Start();

    void SetIrradianceMap(unsigned int skybox);
    void SetPrefilteredMap(unsigned int skybox);

    float exposure = 1.0f;

    void HDRBufferUpdate();
    void OutputBufferUpdate();


	void Update();

private:
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
    std::array<std::vector<unsigned int>, Shader::count> entityShaderOrder;

    /// <summary>
    /// shader programs
    /// </summary>
    std::vector<unsigned int>* shaders;

    void BindLightUniform(unsigned int shaderProgram,
        std::unordered_map<unsigned int, Light>& lightComponents,
        std::unordered_map<unsigned int, Transform>& transComponents);

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
    unsigned int skyboxTexture;
    unsigned int irradianceMap;
    unsigned int brdfLUTTexture;
    unsigned int prefilterMap;
    void IBLBufferSetup(unsigned int skybox);

    void BloomSetup();
    unsigned int mFBO;
    std::vector<bloomMip> bloomMips;

    void RenderBloom(unsigned int srcTexture);

    void RenderDownSamples(unsigned int srcTexture);
    void RenderUpSamples(float aspectRatio);

    void RenderQuad();
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    void RenderCube();
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;



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

