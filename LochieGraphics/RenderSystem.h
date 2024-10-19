#pragma once

#include "LineRenderer.h"
#include "Graphics.h"

#include <unordered_map>
#include <unordered_set>

class Animator;
class FrameBuffer;
class Camera;
class Light;
class Mesh;
class Material;
class ModelRenderer;
class Shader;
class Transform;
class Texture;

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

    static LineRenderer lines;
    static LineRenderer debugLines;

    void Start(
        unsigned int _skyboxTexture,
        std::vector<Shader*>* _shaders,
        Light* shadowCaster,
        std::string paintStrokeTexturePath
    );

    void SetIrradianceMap(unsigned int skybox);
    void SetPrefilteredMap(unsigned int skybox);

    float exposure = 1.0f;

    void ForwardUpdate();
    void HDRBufferUpdate();
    void OutputBufferUpdate();
    void BloomUpdate();
    void SSAOUpdate();

    void Update(
        std::unordered_map<unsigned long long, ModelRenderer>& renders,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& shadowCasters,
        std::unordered_map<unsigned long long, Animator>& animators,
        Camera* camera
    );

    void ScreenResize(int width, int height);

    void IBLBufferSetup(unsigned int skybox);

    int SCREEN_WIDTH, SCREEN_HEIGHT = 0;

    bool showShadowDebug = false;
    unsigned int skyboxTexture = 0;
    glm::mat4 projection = glm::zero<glm::mat4>();
    int kernelSize = 64;
    float ssaoRadius = 0.5f;
    float ssaoBias = 0.025f;

    /// <summary>
    /// Pointer to current window
    /// </summary>
    GLFWwindow* window = nullptr;


    void GUI();

private:

    Mesh* shadowDebugQuad = nullptr;
    Mesh* screenQuad = nullptr;
    Mesh* buttonQuad = nullptr;

    Texture* screenColourBuffer = nullptr;

    FrameBuffer* screenFrameBuffer = nullptr;

    Light* shadowCaster = nullptr;

    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture = 0;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation = 0;

    glm::mat4 viewMatrix = glm::zero<glm::mat4>();

    /// <summary>
    /// List of entities in order of the shader they use.
    /// </summary>
    std::vector<std::vector<unsigned int>> entityShaderOrder;

    /// <summary>
    /// shader programs
    /// </summary>
    std::vector<Shader*>* shaders = {};


    void DrawAnimation(
        std::unordered_map<unsigned long long, Animator>& animators,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& renderers,
        Shader* shader = nullptr
    );

    void DrawRenderers(
        std::unordered_map<unsigned long long, ModelRenderer>& renderers,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_set<unsigned long long> animatedRenderered,
        Shader* shader = nullptr
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

    unsigned int hdrFBO = 0;
    unsigned int bloomBuffer = 0;
    unsigned int colorBuffer = 0;
    unsigned int rboDepth = 0;

    unsigned int outputFBO = 0;
    unsigned int outputTexture = 0;

    unsigned int captureFBO = 0;
    unsigned int captureRBO = 0;

    unsigned int irradianceMap = 0;
    unsigned int brdfLUTTexture = 0;
    unsigned int prefilterMap = 0;
    Texture* paintStrokeTexture = nullptr;

    void BloomSetup();
    unsigned int mFBO = 0;
    std::vector<bloomMip> bloomMips = {};

    void RenderBloom(unsigned int srcTexture);

    void RenderDownSamples(unsigned int srcTexture);
    void RenderUpSamples(float aspectRatio);

    unsigned int positionBuffer = 0;
    unsigned int normalBuffer = 0;
    unsigned int forwardFBO = 0;
    void ForwardSetup();

    unsigned int ssaoFBO = 0;
    unsigned int ssaoColorBuffer = 0;
    unsigned int ssaoBlurFBO = 0;
    unsigned int ssaoBluredBuffer = 0;
    std::vector<glm::vec3> ssaoKernel = {};
    std::vector<glm::vec3> ssaoNoise = {};
    unsigned int noiseTexture = 0;
    void SSAOSetup();

    void RenderSSAO();

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
