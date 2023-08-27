
#include "EtherPCH.h"  
#include "Object.h"  




namespace Hazel
{ 




    void Object::loadModel(std::string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;

        //important, calculate tangent space for normal mapping
     const aiScene* scene = importer.ReadFile
        (path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);


        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        // retrieve the directory path of the filepath, used to load the textures
        m_Directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void Object::processNode(aiNode* node, const aiScene* scene)
    {
        //HZ_PROFILE_FUNCTION();
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(processMesh(mesh, scene));
            m_Materials.push_back(processMaterial(mesh, scene)); 
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        { 
            processNode(node->mChildren[i], scene);
        }
 

    }
     

    Ref<Material> Object::processMaterial(aiMesh* mesh, const aiScene* scene)
    {
        // process materials
        aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];


        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // learnopengl use texture_diffuseN , but we hardly use more than one of same type, so we use texture_diffuse

        auto material = Material::Create(m_Shader, WorkflowMode::BlinnPhong);

        //loop through all the textures of the mesh,   add as dictionary

       
        // Assume that you have functions to load textures somewhere in your code
        auto loadTexture = [&](aiTextureType type, TextureType textureType) {
            aiString str;  //filenname
 
            if (ai_material->GetTexture(type, 0, &str) == AI_SUCCESS)
            { 
                std::string filename = str.C_Str(); 
                std::string fileDir = m_Directory + '/' + filename;
                 
                //opt
                bool skip = false;
                for (auto const& texture : m_Loaded_Textures)
                {
                    if (texture->GetPath() == fileDir)
                    {
						material->SetTexture(textureType, texture); // set reference directly.
						skip = true;


                        HZ_CORE_INFO("Modelloading: Texture at: {0} is already loaded ", fileDir);
						break;

					}
				}

                if (!skip)
                {
                    // Load your texture here using directory, and set it to your material
					Ref<Texture2D> texture = Texture2D::Create(fileDir);
					material->SetTexture(textureType, texture); // Assuming you have a setter for textures in your Material class
					m_Loaded_Textures.push_back(texture);  //opt

                    HZ_CORE_INFO("Modelloading: Texture at: {0} is being loaded ", fileDir);
				}
         
             
             }
         };

    
        // Load textures for different types depending on your workflow mode
        if (material->GetWorkflowMode() == WorkflowMode::BlinnPhong)
        {
            //HZ_CORE_INFO("Modelloading: BlinnPhong is used");
            loadTexture(aiTextureType_DIFFUSE, TextureType::DiffuseMap);
            loadTexture(aiTextureType_SPECULAR, TextureType::SpecularMap);
            loadTexture(aiTextureType_HEIGHT, TextureType::NormalMap);  //in assimp, normal map is height map for historic reason.
             
           
        }
        else if (material->GetWorkflowMode() == WorkflowMode::Metallic)
        {
            //HZ_CORE_INFO("Modelloading: Metallic is used");
            loadTexture(aiTextureType_DIFFUSE, TextureType::AlbedoMap);
            loadTexture(aiTextureType_SPECULAR, TextureType::MetallicMap);
            loadTexture(aiTextureType_SHININESS, TextureType::RoughnessMap);
            loadTexture(aiTextureType_HEIGHT, TextureType::NormalMap);
            loadTexture(aiTextureType_AMBIENT, TextureType::AOMap); 
        }
		else if (material->GetWorkflowMode() == WorkflowMode::FlatColor)
		{
            HZ_CORE_INFO("Modelloading: FlatColor is used");
		}
	    else 
		{
            HZ_CORE_INFO("Modelloading: unknown workflow");
		}
 

        // Load other properties here if needed, like colors, roughness, etc.
      
        return material;
    }

    Ref<Mesh> Object::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        std::unordered_map<TextureType, Ref<Texture2D>> m_Textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector; 
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);


           // if (vertex.Position.length()) HZ_CORE_INFO("vertex Position is null");
           // if (vertex.Normal.length()) HZ_CORE_INFO("vertex normal is null");
           // if (vertex.TexCoords.length()) HZ_CORE_INFO("vertex UV is null");
           // if (vertex.Tangent.length()) HZ_CORE_INFO("vertextangent is null");
           // if (vertex.Bitangent.length()) HZ_CORE_INFO("vertex Bitangent is null");


            vertices.push_back(vertex);

        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }



        // return a mesh object created from the extracted mesh data
        //return Mesh(vertices, indices, textures);
        auto _mesh = Mesh::Create(vertices, indices);
        return _mesh; 

    }
}