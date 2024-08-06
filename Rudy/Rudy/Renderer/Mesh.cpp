#include "RudyPCH.h"


#include "Mesh.h" 

namespace Rudy
{
 
	SharedPtr<Mesh> Mesh::Create()
	{
		return CreateShared<Mesh>();
	}

	void Mesh::SetupVertices()
	{
		//common robustness check: vertex info are the same size as positions
		if (UVs.size()> 0 && UVs.size()!= positions.size())
		{
			RD_CORE_ERROR("Mesh:   vertex UVs size is not the same as positions size");
			return;
		}
		 
		if (normals.size() > 0 && normals.size() != positions.size())
		{
			RD_CORE_ERROR("Mesh:   vertex normals size is not the same as positions size");
			return;
		}

		if (boneWeights.size() > 0 && boneWeights.size() != positions.size())
		{
			RD_CORE_ERROR("Mesh:   vertex boneWeights size is not the same as positions size");
			return;
		}

  
		//orgainze into vertex struct 
	   //expect same size if defined;
		if (vertices.size() != 0)
		{
			return;
		}
		else
		{
			RD_CORE_INFO("Mesh: put data into vertex arrays");

			std::vector<Vertex> _vertices;
			for (size_t i = 0; i < positions.size(); ++i) {

				Vertex _vertex; 

				if (UVs.size() > 0)
					_vertex.UV = UVs[i];

				//must have position
				_vertex.Position = positions[i];

				if (normals.size() > 0)
					_vertex.Normal = normals[i];

				if (tangents.size() > 0)
					_vertex.Tangent = tangents[i];

				if (boneWeights.size() > 0)
				{
					auto _indices =
						glm::ivec4(boneWeights[i].BoneIndex0, boneWeights[i].BoneIndex1,
							boneWeights[i].BoneIndex2, boneWeights[i].BoneIndex3);

					auto _weights =
						glm::vec4(boneWeights[i].Weight0, boneWeights[i].Weight1,
							boneWeights[i].Weight2, boneWeights[i].Weight3);

					_vertex.BoneIndices = _indices;
					_vertex.BoneWeights = _weights;
				}

				_vertices.push_back(_vertex);

				this->vertices = _vertices;

			}
		}

	}



}