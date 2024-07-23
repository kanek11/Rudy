#pragma once
#include "RudyPCH.h"

#include <Rudy/Animation/Animation.h>
#include <Rudy/Renderer/Buffer.h>
#include <Rudy/Renderer/Material.h>
#include <Rudy/Renderer/Mesh.h>
#include <Rudy/Renderer/RendererComponent.h>
#include <Rudy/Renderer/Transform.h>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace Rudy
{

// rule: use a factory method Create() to create an object;
// create a transform component for the object;

// object is the base class for all objects in the scene,
// we will have reflection system in the future,  but for now,  we just use this
// simple inheritance;

// virtual destructor is needed to correctly release derived class;
//<<abstract>>
class Object
{
public:
    virtual ~Object() = default;
    Object()          = default;

    static Ref<Object> Create()
    {
        auto object = CreateRef<Object>();
        object->InitComponent(object);
        return object;
    }

    void InitComponent(Ref<Object> object)
    {
        object->transform = Transform::Create(object);
    }

public:
    Ref<Transform> transform = nullptr;
    std::string    name      = "Unnamed gameObject";

    bool isRenderable = false;
};

// a renderable object is an object that can be rendered;
// now, it contains a renderer component;
// a generic renderer component defines draw command.
// there's no default one,  user must define the drived classes and set the
// renderer component;

// we hardcode the transform component in the renderer component;
// todo: maybe implement a fancy ECS system in the future;
//<<abstract>>
class RenderableObject : public Object
{
public:
    virtual ~RenderableObject() = default;
    RenderableObject() :
        Object() { }

    void SetRendererComponent(Ref<Renderer> rendererComponent)
    {
        rendererComponent->SetTransform(this->transform);
        this->m_rendererComponent = rendererComponent;
    }

    Ref<Renderer> GetRendererComponent() { return m_rendererComponent; }

    bool hasRendererComponent() { return m_rendererComponent != nullptr; }

public:
    virtual void Draw(Ref<Camera> cam) = 0;

    Ref<Renderer> m_rendererComponent = nullptr;
};

//<<terminal>>
// simply assume mesh renderer component; come with a mesh component;
class StaticMeshObject : public Object
{
public:
    ~StaticMeshObject() = default;
    StaticMeshObject() :
        Object() { }

    static Ref<StaticMeshObject> Create()
    {
        auto object = CreateRef<StaticMeshObject>();
        object->InitComponent(object);
        return object;
    }

    void InitComponent(Ref<StaticMeshObject> object)
    {
        object->Object::InitComponent(object);
        object->SetRenderer(StaticMeshRenderer::Create());
    }

    void SetRenderer(Ref<StaticMeshRenderer> rendererComponent)
    {
        rendererComponent->SetTransform(this->transform);
        this->m_renderer = rendererComponent;
    }
    Ref<StaticMeshRenderer> GetRenderer() { return this->m_renderer; }

    void SetMaterial(Ref<Material> mat) { this->m_renderer->SetMaterial(mat); }
    void SetShader(Ref<Shader> shader) { this->m_renderer->SetShader(shader); }

public:
    // override draw command if needed;
    virtual void Draw(Ref<Camera> cam) { this->m_renderer->Draw(cam); }

    // asset and renderer component;
    Ref<Mesh>               m_mesh   = nullptr;
    Ref<StaticMeshRenderer> m_renderer = nullptr;
};

// a model contains... whatever the model contains;
// expect multiple mesh objects;
class Model : public Object
{
public:
    // global settings
    static float       s_scaleFactor;
    static Ref<Shader> s_defaultShader;

public:
    ~Model() = default;
    Model()  = default;
    Model(std::string const& path);

    static Ref<Model> LoadModel(std::string const& path);

public:
    Ref<Shader> shader = nullptr;
    void        SetShader(Ref<Shader> shader);

    Ref<StorageBuffer> boneTransformBuffer = StorageBuffer::Create();

    // animator component;
    Ref<Animator> animator = nullptr;

    void SetAnimator(Ref<Animator> animator)
    {
        this->animator = animator;
        if (this->hasAnimation())
            animator->animationClip = this->animationClip;
        else
            RD_CORE_WARN("Model: model has no animations, invalid animator setting");
    }

    //============================================
    // load from model files:
    std::vector<Ref<StaticMeshObject>> meshObjects;

    // animation and bound bones are could-be-separate;
    // might need retarget;
    Ref<AnimationClip> animationClip = nullptr;
    bool               hasAnimation() { return animationClip != nullptr; }

    // retreive the bound bones defined in the mesh;     //aiBone->mName;
    // aiBone->mOffsetMatrix ordered map,  key = name to facilate searching by
    // name in animation keyframe;
    std::map<std::string, int> boneMap;
    std::vector<glm::mat4>     bindPose = std::vector<glm::mat4>(100, glm::mat4(1.0f));

public:
    void Draw(Ref<Camera> cam, uint32_t count = 1, Ref<Material> mat = nullptr);

    //========system
    // retrieved in the hierarchy of the animation file;
    Ref<Object> rootNode = nullptr;
    Ref<Object> findNode(std::string const& name);
    Ref<Object> findNodeRecursive(Ref<Transform> node, std::string const& name);

public:
    std::vector<Ref<Texture2D>> m_Loaded_Textures;
    // opt to avoid duplicate textures,  loading is extremely slow

    void processNode(const aiScene* scene, aiNode* ai_node, Ref<Object> scene_node);

    Ref<Mesh>     processMesh(aiMesh* mesh, const aiScene* scene);
    Ref<Material> processMaterial(aiMesh* mesh, const aiScene* scene);

    void processBones(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& vertices);
    void processAnimation(const aiScene* scene);

public:
    std::string directory;
};

} // namespace Rudy
