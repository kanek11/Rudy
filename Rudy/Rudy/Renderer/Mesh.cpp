#include "RudyPCH.h"


#include "Mesh.h" 

namespace Rudy
{
 
	Ref<Mesh> Mesh::Create()
	{
		return CreateRef<Mesh>();
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



	/*
	void OpenGLMesh::LoadToGPU()
	{

		//fixed routine
		glCreateVertexArrays(1, &m_vertexArrayTarget);
		glBindVertexArray(m_vertexArrayTarget);

		glCreateBuffers(1, &m_vertexBufferTarget);
		glCreateBuffers(1, &m_indexBufferTarget);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferTarget);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferTarget);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

		// for (size_t i = 0; i < vertices.size(); ++i)
		// {
		// 	std::cout << "bone indices " << vertices[i].BoneIndices << std::endl;
		// 	std::cout << "bone weights " << vertices[i].BoneWeights << std::endl;
		// }


		 //the interpretation: depend on how we define the vertex data
		 //if any field is empty, the code simply ignore it
		 // 
		 // vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));

		//
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneIndices));

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneWeights));


		glBindVertexArray(0);


		//RD_CORE_INFO("OpenGLMesh: Mesh is setup, vertex data is send to GPU, vao:{0}", m_VertexArray);


	}
	*/

}