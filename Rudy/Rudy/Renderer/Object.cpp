#include "RudyPCH.h"  

#include "Object.h"  
 

#include <Rudy/Renderer/Renderer.h>





namespace Rudy 

{   
    //static declaration
    float Model::s_scaleFactor  = 1.0f; 

    namespace AssimpGLMHelpers
    { 

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }

        static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
        {
            return glm::vec3(vec.x, vec.y, vec.z);
        }

        static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
        {
            return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }

    };

     

    //-------------------------
    //---model----------------- 

    Ref<Model> Model::LoadModel(std::string const& path)

    {
        return CreateRef<Model>(path);
    }
     

    Model::Model(std::string const& path) 
     { 
        //start an instance of assimp importer
        Assimp::Importer importer;

        //optional: scale factor; eg: dae file is usually in cm, <unit meter="0.010000"/>
        //but i found that's dealt in the root node transform;

        //it seems imported .dae include the scale factor in the rootNode transform; 0.01f;
        //it's kinda weird logic that bone and mesh can be in different scale initially;
        //so i will override this in the root node transform;

        importer.SetPropertyFloat("GLOBAL_SCALE_FACTOR",  this->s_scaleFactor ); 

        //read data ,  optionally with some postprocessing
        const aiScene* scene = importer.ReadFile
        (path, aiProcess_GlobalScale | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
 
        // check for errors, complete scene ,  with root node
        //assert(scene && scene->mRootNode);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        // retrieve the directory path of the filepath, used to load the textures under the same directory
        this->directory = path.substr(0, path.find_last_of('/'));
         

        //new: animation logic
        //retrieve the hierarchy of the nodes as gameobjects 

        // process ASSIMP's root node recursively 
        RD_CORE_INFO("go into root node");

        aiMatrix4x4 newIdentity ;  //identity by default
        scene->mRootNode->mTransformation = newIdentity;  //override the scale factor;
        processNode(scene, scene->mRootNode, rootNode);
        RD_CORE_INFO("scene traverse finished");

        //new: if the model has animation data, after the node hierarchy is processed
        if (scene->HasAnimations())
        {
            processAnimation(scene);
        }


    }



    //each member of aiNodeAnim* ,is keys of different type;

    //struct aiVectorKey {
    //    double mTime;       // The time of this key
    //    aiVector3D mValue;  // The value of this key
    //};

 
     
    //the aiScene organize things an whole array, instead of the actual nodes,
    //what the nodes/mesh refer to data in the scene by index

    //each node contains meshes[],  and pointers to its children nodes ; 
     
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void Model::processNode (const aiScene* scene, aiNode* ai_node, Ref<Object> scene_node )
    {
        //RD_PROFILE_FUNCTION(); 

        //new: keep the node hierarchy, the model now keep a local scene graph
        //this gives the local transform for static rest pose;
        //used to update the FK; 

        auto name = ai_node->mName.data;
        scene_node->name = name;

        //important: local , not global
        scene_node->transform->localTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(ai_node->mTransformation);
        scene_node->transform->gameObject = scene_node;  //important

         
        //process meshes in the node if any
        //they are separated by one-material-per-mesh when imported
        RD_CORE_INFO("modelloader: find {0} meshes in the node: {1}", ai_node->mNumMeshes, name);
        for (uint32_t i = 0; i < ai_node->mNumMeshes; i++)
        {
            //the mMeshes are indices to the actual meshes in the scene
            aiMesh* ai_mesh = scene->mMeshes[ai_node->mMeshes[i]];

            //engine-specific mesh object
            auto meshObject = StaticMeshObject::Create();
            auto meshRenderer = meshObject->GetRenderer();
        
            RD_CORE_INFO("model: process mesh");
            meshRenderer->SetMesh( processMesh(ai_mesh, scene) );

            RD_CORE_INFO("model: process material");
            meshRenderer->SetMaterial( processMaterial(ai_mesh, scene) );


            this->meshObjects.push_back(meshObject);  
        
        
        }


        //recursively process each of the children nodes, it stops if no children
        for (uint32_t i = 0; i < ai_node->mNumChildren; i++)
        { 

            //new: keep the node hierarchy 
            //new children node
            Ref<Object> newNode = CreateRef<Object>();
            scene_node->transform->children.push_back(newNode->transform);

            processNode(scene, ai_node->mChildren[i], newNode);

          
        }
 

    }



    Ref<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill,  model loader fits better for per-vertex workflow
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // walk through each of the mesh's vertices
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
           
           Vertex vertex;
       
           vertex.Position = glm::vec3(
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z 
            );

            // if any normals
            if (mesh->HasNormals())
            {
                vertex.Normal = glm::vec3(
                   mesh->mNormals[i].x,
                   mesh->mNormals[i].y,
                   mesh->mNormals[i].z
                );
            } 

            //if any uv 
            //and a vertex can contain UV0, UV1, UV2, etc. in 2d array  we only consider the first set
            if (mesh->HasTextureCoords(0))
            {
               vertex.UV = glm::vec2(
                 mesh->mTextureCoords[0][i].x,
                 mesh->mTextureCoords[0][i].y
                );
            }
            else
                vertex.UV = glm::vec2(0.0f, 0.0f);



            if(mesh->HasTangentsAndBitangents())
            vertex.Tangent = glm::vec3(
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z);
  

            vertices.push_back(vertex);


        }

        //the indices are called faces  
        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (uint32_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }


         
        // new: process the bone data to get a static rest-pose skeleton

        processBones(scene, mesh, vertices);

    
        // return a mesh object created from the extracted mesh data
        //return Mesh(vertices, indices, textures);
        auto _mesh = Mesh::Create();
        _mesh->vertices = vertices;
        _mesh->indices = indices;

        _mesh->name = mesh->mName.C_Str();
        _mesh->globalIndex = mesh->mMaterialIndex;  //the index in the scene 
          
        RD_CORE_INFO("Modelloading: mesh name:{0}, index:{1}", _mesh->name, _mesh->globalIndex);
         
        return _mesh; 


    }



    void Model::processBones(const aiScene* scene, aiMesh* mesh,  std::vector<Vertex>& vertices)
    {

        //auto& boneInfoMap = m_BoneInfoMap;
        
        int boneCounter = bindPoseBoneMap.size();  //the current number of bones in the map;

        RD_CORE_INFO("Modelloading: {0} bound bones detected for the mesh", mesh->mNumBones);
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            //RD_CORE_INFO("bone name detected {0}", boneName);

            if (this->bindPoseBoneMap.find(boneName) == bindPoseBoneMap.end())  //not registered yet
            {
                
                auto bindPoseTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                
                bindPoseBoneMap[boneName] =
                Bone{
                    boneCounter,
					bindPoseTransform
                };
                 
                boneID = boneCounter;
                boneCounter++;

                RD_CORE_INFO("bound bone registered, name:{0}, id:{1}", boneName, boneID);

            }
            else   //already registered
            {
                boneID = bindPoseBoneMap[boneName].boneIndex;
                RD_CORE_INFO("bone:{0} already registered", boneName);
            }

            assert(boneID != -1); 
            auto weights = mesh->mBones[boneIndex]->mWeights;  //the bound vertices info;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;  //the num;
            //RD_CORE_INFO("Modelloading: {0} bounded vertices detected for the bone", numWeights);
 
            //foreach bound vertex;
            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());

                for (int i = 0; i < 4; ++i)  //max 4 bones per vertex
                { 
                    //glm::vec4 BoneWeights;
                       if (vertices[vertexId].BoneWeights[i] == 0) //the first empty slot
                    {
                        vertices[vertexId].BoneWeights[i] = weight;
                        vertices[vertexId].BoneIndices[i] =  boneID;
                       // RD_CORE_INFO("modelloader weight:{0},id:{1}, vertex {2}", weight,boneID, vertexId);
                        break;
                    }
                } 
            }
        }



    }






    Ref<Material>  Model::processMaterial(aiMesh* mesh, const aiScene* scene)
    {
        // process materials
        aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex]; 

        //engine-specific material object
        auto material = PBRMaterial::Create();  
        material->m_Name = ai_material->GetName().C_Str();
        material->m_GlobalIndex = mesh->mMaterialIndex;  //the index in the scene

        RD_CORE_INFO("Modelloading: material name:{0}, index:{1}", material->m_Name, material->m_GlobalIndex);
         

        auto loadTexture = [&](aiTextureType type, TexType TexType) {
        aiString str;  //filenname

            if (ai_material->GetTexture(type, 0, &str) == AI_SUCCESS)
            {
                std::string filename = str.C_Str();
                std::string fileDir = this->directory + '/' + filename;
                 
              
                //optimize:  check if the texture is already loaded
                //it's well possible that the same texture is used for different meshes
                bool skip = false;
                for (auto const& texture : m_Loaded_Textures)
                {
                    if (texture->GetPath() == fileDir)
                    {
                        material->SetTexture(TexType, texture); // set reference directly.
                        skip = true;

                        RD_CORE_INFO("Modelloading: Texture at: {0} is already loaded ", fileDir);
                        break;
                    }
                }

                if (!skip)
                {
                    // Load your texture here using directory, and set it to your material
                    Ref<Texture2D> texture = Texture2D::LoadFile(fileDir);
                    
                    material->SetTexture(TexType, texture); // Assuming you have a setter for textures in your Material class
                    m_Loaded_Textures.push_back(texture);  //opt

                    
                    RD_CORE_WARN("Modelloading: Texture type: {0} is added to material ", (int)TexType);
                }

            }




        };


   
        //refer to the used version source code of assimp;

       //RD_CORE_INFO("Modelloading: BlinnPhong is used");
        loadTexture(aiTextureType_DIFFUSE, TexType::AlbedoMap);
        loadTexture(aiTextureType_BASE_COLOR, TexType::AlbedoMap);

        loadTexture(aiTextureType_SPECULAR, TexType::SpecularMap);

        //tangent space
        loadTexture(aiTextureType_NORMALS, TexType::NormalMap);

        loadTexture(aiTextureType_DIFFUSE_ROUGHNESS, TexType::RoughnessMap);

        loadTexture(aiTextureType_METALNESS, TexType::MetallicMap);
        loadTexture(aiTextureType_AMBIENT_OCCLUSION, TexType::AOMap);


        // Load other properties here if needed, like colors, roughness, etc.

        return material;

    }



    void Model::processAnimation(const aiScene* scene)
    {

        //for (uint32_t i = 0; i < scene->mNumAnimations; i++) 
        RD_CORE_INFO("Modelloading: Animation num:{0}",scene-> mNumAnimations);

        aiAnimation* ai_animation = scene->mAnimations[0];  //test only one animation for now

        //engine-specific animation object
        animationClip = AnimationClip::Create();

        animationClip->rootNode = rootNode->transform;

        //meta info
        auto FPS = ai_animation->mTicksPerSecond; 
        auto duration = ai_animation->mDuration ;  ///FPS = tick to second
        animationClip->duration = duration;
        animationClip->FPS = FPS;

        RD_CORE_INFO("animationclip created, duration:{0}, fps:{1}", duration, FPS);



        //the keyframes of each node transforms, in global space
        //correspond to the hierarchy of the nodes;

        //a sequence is called a channel in assimp
        //a channel is aiNodeAnim,  which contains the transforms of a node as a sequence
        int size = ai_animation->mNumChannels;
        RD_CORE_INFO("""animationclip:{0} channels ", ai_animation->mNumChannels);


        //auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
        //int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

        //for each channel; 
        for (int i = 0; i < size; i++)
        {
            aiNodeAnim* channel = ai_animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;



            //RD_CORE_INFO("Modelloading: animated bone name found:{0}", boneName);
            Ref<KeyBone> newKeyBone = CreateRef<KeyBone>();

            //me: refer to the hierarchy of the nodes,
            auto node = this->findNode(boneName);
            if (node)
                newKeyBone->m_Node = node->transform;
            else
                RD_CORE_ERROR("Modelloading: animted bone name not found in the scene:{0}", boneName);


            //retreive properties in the channel
            //RD_CORE_WARN("Modelloading: mNumPositionKeysfound:{0}", channel->mNumPositionKeys);
            for (unsigned int j = 0; j < channel->mNumPositionKeys; j++)
            {
                aiVectorKey key = channel->mPositionKeys[j];
                KeyPosition newKeyPosition;
                newKeyPosition.frameIndex = key.mTime;         //static_cast<int>(key.mTime * FPS / duration);
                //std::cout << "frameIndex:" << newKeyPosition.frameIndex << std::endl;
                newKeyPosition.position = AssimpGLMHelpers::GetGLMVec(key.mValue);

                newKeyBone->m_KeyPositions.push_back(newKeyPosition); 
            }

            for (unsigned int j = 0; j < channel->mNumRotationKeys; j++)
            {
                auto key = channel->mRotationKeys[j];
                KeyRotation newKeyRotation;
                newKeyRotation.frameIndex = key.mTime;  //static_cast<int>(key.mTime * FPS / duration);
                newKeyRotation.rotation = AssimpGLMHelpers::GetGLMQuat(key.mValue);
                newKeyBone->m_KeyRotations.push_back(newKeyRotation);
            }

            for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
            {
                auto key = channel->mScalingKeys[j];
                KeyScale newKeyScale;
                newKeyScale.frameIndex = key.mTime;  //static_cast<int>(key.mTime * FPS / duration);
                newKeyScale.scale = AssimpGLMHelpers::GetGLMVec(key.mValue);
                newKeyBone->m_KeyScales.push_back(newKeyScale);
            }


            animationClip->AddKeyBone(boneName, newKeyBone);

            RD_CORE_INFO("animclip: add animated keybone:{0} ", boneName);
        }


    }




    Ref<Object> Model::findNode(std::string const& name)
    {
		return findNodeRecursive(rootNode->transform, name);
    }



    Ref<Object> Model::findNodeRecursive(Ref<Transform> node, std::string const& name) {
        if (node == nullptr) {
            return nullptr;
        }
         
        if (node->gameObject->name == name) {
            return node->gameObject;
        }

 
        for (auto& child : node->children) {
            Ref<Object> found = findNodeRecursive(child, name);
            if (found != nullptr) {
                return found;
            }
        }
         
      
        return nullptr;
    }



    //assume model use same shader for all meshes,  this works for now
    //otherwise the buffer should be handled for each mesh ;
    void Model::Draw(Ref<Camera> cam, uint32_t count, Ref<Material> mat)
    {
         //legacy way:
        //for (int i = 0; i < 100; ++i)
        //    gBufferPassSkinnedShader->SetMat4("u_BoneTransforms[" + std::to_string(i) + "]", transforms[i]);
 
        if (this->animator != nullptr)
        {
           if(this->shader != nullptr)
           this->shader->Bind();
           boneTransformBuffer->BindBase(0); 

           if(mat !=nullptr)
           mat->GetShader()->Bind();
           boneTransformBuffer->BindBase(0); 

		}   

        for (auto meshObj : this->meshObjects)
            meshObj->GetRenderer()->Draw(cam, count, mat);

    }

 

    void Model::SetShader(Ref<Shader> shader)
    {
		for (auto meshObj : this->meshObjects)
            meshObj->GetRenderer()->SetShader(shader);
	}



}