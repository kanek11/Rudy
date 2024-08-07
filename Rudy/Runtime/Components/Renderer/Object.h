#pragma once
#include "RudyPCH.h"

#include "Runtime/Components/Animation/Animation.h"

#include "Buffer.h"
#include "Material.h"
#include "Mesh.h"
#include "RendererComponent.h"
#include "Transform.h"

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

    static SharedPtr<Object> Create()
    {
        auto object = CreateShared<Object>();
        object->InitComponent(object);
        return object;
    }

    void InitComponent(SharedPtr<Object> object)
    {
        object->transform = Transform::Create(object);
    }

public:
    SharedPtr<Transform> transform = nullptr;
    std::string          name      = "Unnamed gameObject";

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

    void SetRendererComponent(SharedPtr<Renderer> rendererComponent)
    {
        rendererComponent->SetTransform(this->transform);
        this->m_rendererComponent = rendererComponent;
    }

    SharedPtr<Renderer> GetRendererComponent() { return m_rendererComponent; }

    bool hasRendererComponent() { return m_rendererComponent != nullptr; }

public:
    virtual void Draw(SharedPtr<Camera> cam) = 0;

    SharedPtr<Renderer> m_rendererComponent = nullptr;
};

//<<terminal>>
// simply assume mesh renderer component; come with a mesh component;
class StaticMeshObject : public Object
{
public:
    ~StaticMeshObject() = default;
    StaticMeshObject() :
        Object() { }

    static SharedPtr<StaticMeshObject> Create()
    {
        auto object = CreateShared<StaticMeshObject>();
        object->InitComponent(object);
        return object;
    }

    void InitComponent(SharedPtr<StaticMeshObject> object)
    {
        object->Object::InitComponent(object);
        object->SetRenderer(StaticMeshRenderer::Create());
    }

    void SetRenderer(SharedPtr<StaticMeshRenderer> rendererComponent)
    {
        rendererComponent->SetTransform(this->transform);
        this->m_renderer = rendererComponent;
    }
    SharedPtr<StaticMeshRenderer> GetRenderer() { return this->m_renderer; }

    void SetMaterial(SharedPtr<Material> mat) { this->m_renderer->SetMaterial(mat); }
    void SetShader(SharedPtr<Shader> shader) { this->m_renderer->SetShader(shader); }

public:
    // override draw command if needed;
    virtual void Draw(SharedPtr<Camera> cam) { this->m_renderer->Draw(cam); }

    // asset and renderer component;
    SharedPtr<Mesh>               m_mesh     = nullptr;
    SharedPtr<StaticMeshRenderer> m_renderer = nullptr;
};

// a model contains... whatever the model contains;
// expect multiple mesh objects;
class Model : public Object
{
public:
    // global settings
    static float             s_scaleFactor;
    static SharedPtr<Shader> s_defaultShader;

public:
    ~Model() = default;
    Model()  = default;
    Model(std::string const& path);

    static SharedPtr<Model> LoadModel(std::string const& path);

public:
    SharedPtr<Shader> shader = nullptr;
    void              SetShader(SharedPtr<Shader> shader);

    SharedPtr<StorageBuffer> boneTransformBuffer = StorageBuffer::Create();

    // animator component;
    SharedPtr<Animator> animator = nullptr;

    void SetAnimator(SharedPtr<Animator> animator)
    {
        this->animator = animator;
        if (this->hasAnimation())
            animator->animationClip = this->animationClip;
        else
            RD_CORE_WARN("Model: model has no animations, invalid animator setting");
    }

    //============================================
    // load from model files:
    std::vector<SharedPtr<StaticMeshObject>> meshObjects;

    // animation and bound bones are could-be-separate;
    // might need retarget;
    SharedPtr<AnimationClip> animationClip = nullptr;
    bool                     hasAnimation() { return animationClip != nullptr; }

    // retreive the bound bones defined in the mesh;     //aiBone->mName;
    // aiBone->mOffsetMatrix ordered map,  key = name to facilate searching by
    // name in animation keyframe;
    std::map<std::string, int> boneMap;
    std::vector<glm::mat4>     bindPose = std::vector<glm::mat4>(100, glm::mat4(1.0f));

public:
    void Draw(SharedPtr<Camera> cam, uint32_t count = 1, SharedPtr<Material> mat = nullptr);

    //========system
    // retrieved in the hierarchy of the animation file;
    SharedPtr<Object> rootNode = nullptr;
    SharedPtr<Object> findNode(std::string const& name);
    SharedPtr<Object> findNodeRecursive(SharedPtr<Transform> node, std::string const& name);

public:
    std::vector<SharedPtr<Texture2D>> m_Loaded_Textures;
    // opt to avoid duplicate textures,  loading is extremely slow

    void processNode(const aiScene* scene, aiNode* ai_node, SharedPtr<Object> scene_node);

    SharedPtr<Mesh>     processMesh(aiMesh* mesh, const aiScene* scene);
    SharedPtr<Material> processMaterial(aiMesh* mesh, const aiScene* scene);

    void processBones(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& vertices);
    void processAnimation(const aiScene* scene);

public:
    std::string directory;
};

} // namespace Rudy
