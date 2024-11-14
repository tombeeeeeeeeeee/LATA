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
class Model;
class Shader;
class Transform;
class Texture;
class Particle;
class PointLight;
class Spotlight;
struct Frustum;

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
    float postEffectPercent = 0;
    bool postEffectOn = false;

    int superSampling = 1;

    static LineRenderer lines;
    static LineRenderer debugLines;

    bool particleFacingCamera;


    float lightTimeToOn = 2.5f;
    float lightTimeToOff = 2.5f;
    float lightTimeToExplode = 0.1f; 
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


    void Start(unsigned int _skyboxTexture);

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
        std::unordered_map<unsigned long long, ModelRenderer>& renders,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& shadowCasters,
        std::unordered_map<unsigned long long, Animator*>& animators,
        std::unordered_map<unsigned long long, PointLight>& pointLights,
        std::unordered_map<unsigned long long, Spotlight>& spotlights,
        Camera* camera,
        float delta,
        std::vector<Particle*> particles = {}
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

    Mesh* screenQuad = nullptr;

    Light* shadowCaster = nullptr;

    Shader* postProcess = nullptr;

    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture = 0;

    Texture* onLightTexture = nullptr;
    Texture* offLightTexture = nullptr;
    Texture* flickeringLightTexture = nullptr;
    Texture* explodingLightTexture = nullptr;
    Texture* syncLightTexture = nullptr;
    Model* lightSphere = nullptr;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation = 0;


    /// <summary>
    /// List of entities in order of the shader they use.
    /// </summary>
    std::vector<std::vector<unsigned int>> entityShaderOrder;

    void DrawAllRenderers(
        std::unordered_map<unsigned long long, Animator*>& animators,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& renderers,
        std::unordered_set<unsigned long long> animatedRenderered,
        Frustum frustum,
        Shader* shader = nullptr
    );

    void RenderSpotLightShadowMaps(
        std::unordered_map<unsigned long long, Spotlight>& spotlights,
        std::unordered_map<unsigned long long, Animator*>& animators,
        std::unordered_map<unsigned long long, Transform>& transforms,
        std::unordered_map<unsigned long long, ModelRenderer>& renderers,
        std::unordered_set<unsigned long long> animatedRenderered
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
    Texture* paintStrokeTexture = nullptr;

    void BloomSetup();
    unsigned int mFBO = 0;
    std::vector<bloomMip> bloomMips = {};

    void RenderBloom(unsigned int srcTexture);

    void RenderDownSamples(unsigned int srcTexture);
    void RenderUpSamples(float aspectRatio);

    unsigned int normalBuffer = 0;
    unsigned int albedoBuffer = 0;
    unsigned int emissionBuffer = 0;
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

    void RenderParticles(
        std::vector<Particle*> particles
    );

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
