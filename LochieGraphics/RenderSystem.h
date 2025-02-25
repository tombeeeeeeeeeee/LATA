#pragma once

#include "LineRenderer.h"
#include "Graphics.h"

#include <unordered_map>
#include <unordered_set>

class FrameBuffer;
class Camera;
class Light;
class Material;
class Shader;
class Transform;
class Texture;
class PointLight;
class Spotlight;

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

    Texture* postFrameTexture;
    FrameBuffer* postFrameBuffer;
    Texture* baseColourKey = nullptr;
    Texture* colourKey1 = nullptr;
    Texture* colourKey2 = nullptr;
    Texture* roomAmbience = nullptr;
    float postEffectPercent = 1.0f;
    bool postEffectOn = true;

    int superSampling = 1;

    static LineRenderer lines;
    static LineRenderer debugLines;

    bool particleFacingCamera;

    float lightTimeToOn = 2.5f;
    float lightTimeToOff = 2.5f;
    float lightTimeToExplode = 0.5f; 
    float lightTimeToFlicker = 5.5f;
    float ambientIntensity = 1.0f;
    bool inOrtho = false;
    glm::vec2 mapMin = { 0.0f, 0.0f };
    glm::vec2 mapDelta = { 0.0f, 0.0f };
    int frameCountInSixteen = 0;
    int sssSteps = 32;
    float sssMaxRayDistance = 0.5f;
    float sssThickness = 0.05f;
    float sssMaxDepthDelta = 0.005f;


    void Start();

    void LevelLoad();

    void PlayStart(
        std::unordered_map<unsigned long long, PointLight>& pointLights,
        std::unordered_map<unsigned long long, Spotlight>& spotlights
        );

    void SetIrradianceMap(unsigned int skybox);

    float exposure = 1.0f;
    int bufferIndex = 0;

    void DeferredUpdate();
    void CompositeBufferUpdate();
    void OutputBufferUpdate();
    void BloomUpdate();
    void SSAOUpdate();
    void LightPassUpdate();
    void LinesUpdate();

    void Update(
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, PointLight>& pointLights,
        std::unordered_map<unsigned long long, Spotlight>& spotlights,
        Camera* camera,
        float delta
    );

    void ScreenResize(int width, int height);

    void IBLBufferSetup(unsigned int skybox);


    int SCREEN_WIDTH, SCREEN_HEIGHT = 0;

    unsigned int skyboxTexture = 0;
    glm::mat4 projection = glm::zero<glm::mat4>();
    int kernelSize = 64;
    float ssaoRadius = 32.0f;
    float ssaoBias = 7.5f;

    /// <summary>
    /// Pointer to current window
    /// </summary>
    GLFWwindow* window = nullptr;

    void GUI();

    glm::mat4 viewMatrix = glm::zero<glm::mat4>();

    void ActivateFlaggedVariables(
        Shader* shader,
        Material* mat
    );


private:

    Light* shadowCaster = nullptr;

    Shader* postProcess = nullptr;

    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture = 0;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation = 0;


    /// <summary>
    /// List of entities in order of the shader they use.
    /// </summary>
    std::vector<std::vector<unsigned int>> entityShaderOrder;

    void DrawAllRenderers(
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_set<unsigned long long> animatedRenderered,
        int staticPass = 2,
        Shader* shader = nullptr
    );

    void RenderPointLights(
        std::unordered_map<unsigned long long, PointLight>& pointLights,
        std::unordered_map<unsigned long long, Transform>& transforms,
        float delta
    );

    void RenderSpotlights(
        std::unordered_map<unsigned long long, Spotlight>& spotlights,
        std::unordered_map<unsigned long long, Transform>& transforms,
        float delta
    );

    void CompositeBufferSetUp();
    void RenderComposite();
    void OutputBufferSetUp();

    unsigned int compositeFBO = 0;
    unsigned int bloomBuffer = 0;
    unsigned int colorBuffer = 0;
    Shader* compositeShader = nullptr;

    unsigned int outputFBO = 0;
    unsigned int outputTexture = 0;

    unsigned int captureFBO = 0;
    unsigned int captureRBO = 0;

    unsigned int irradianceMap = 0;

    void BloomSetup();
    unsigned int mFBO = 0;
    std::vector<bloomMip> bloomMips = {};

    void RenderBloom(unsigned int srcTexture);

    void RenderDownSamples(unsigned int srcTexture);
    void RenderUpSamples(float aspectRatio);

    unsigned int normalBuffer = 0;
    unsigned int albedoBuffer = 0;
    unsigned int emissionBuffer = 0;
    unsigned int pbrBuffer = 0;
    unsigned int depthBuffer = 0;
    unsigned int deferredFBO = 0;
    void DeferredSetup();

    unsigned int lightPassBuffer = 0;
    unsigned int lightPassFBO = 0;
    Shader* ambientPassShader = nullptr;
    Shader* pointLightPassShader = nullptr;
    Shader* spotlightPassShader = nullptr;
    Shader* spotlightShadowPassShader = nullptr;
    void LightPassSetup();
    void RenderAmbientPass();

    unsigned int linesBuffer = 0;
    unsigned int linesFBO = 0;
    void LinesSetup();
    void RenderLinePass();

    Shader* beamShader;
    Shader* syncAimShader;
    float tileLength = 50.0f;
    void RenderBeams(float delta);
    void RenderSyncAim(float delta);

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
