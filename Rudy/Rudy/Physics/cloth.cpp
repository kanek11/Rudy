
#include "RudyPCH.h"

#include "cloth.h"
#include "Rudy/Renderer/Renderer.h"

namespace Rudy
{

	Cloth::Cloth()
	{

	}


	void Cloth::Update()
	{

		if (m_num_particles < 0)
		{
			RD_CORE_ERROR("cloth : m_num_particles < 0");
			return;
		}

		m_num_subSteps = 16;

		auto m_solver_dt = 1.0f / m_FPS; 
		m_time_accumulator += m_system_dt;
		if (m_time_accumulator < m_solver_dt)
		{
			return;
		}
		else
		while (m_time_accumulator >= m_solver_dt)
		{
			m_time_accumulator -= m_solver_dt;

			m_deltaTime = m_solver_dt / m_num_subSteps;
			for (int i = 0; i < m_num_subSteps; i++)
			{
				  this->PBD_step();
				 //this->CPU_Update();
			}

		}
		 
		//m_deltaTime = 0.1f;
		//this->PBD_step();
		// this->CPU_Update();

		this-> ComputeNormals();
	}




	//prepare buffers based on mesh data
	void Cloth::Init()
	{
		
		
		if (m_mesh == nullptr) {
			RD_CORE_ERROR("Cloth: mesh is null");
            return;
		}

		//compatability issue: the buffer layout assume 16 bytes alignment
		m_positions.reserve(m_mesh->positions.size());
		m_normals.reserve(m_mesh->positions.size());
		m_correction.reserve(m_mesh->positions.size());

		for (auto& pos : m_mesh->positions)
		{
			m_positions.emplace_back(pos.x, pos.y, pos.z, 0.0f);
		}
		for (auto& normal : m_mesh->normals)
		{
			m_normals.emplace_back(normal.x, normal.y, normal.z, 0.0f);
		}


		auto density = 1.0f / m_mesh->positions.size();

		m_invMass = std::vector<float>(m_mesh->positions.size(), density);
	    m_invMass[0] = 0.0f;
		//m_invMass[2] = 0.0f;
		m_invMass[20] = 0.0f;
	  

		m_velocities = std::vector<glm::vec4>(m_mesh->positions.size(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
 

		auto indices = m_mesh->indices; 

		auto numTriangles = indices.size() / 3;
		m_num_triangles = numTriangles;
		auto numEdges = numTriangles * 3;
		m_num_edges = numEdges;

		m_num_particles = m_mesh->positions.size();

		RD_CORE_INFO("cloth:numTriangles: {0}", numTriangles);
		RD_CORE_INFO("cloth:numEdges: {0}", numEdges);


		struct Edge
		{
			uint32_t vertexIndex[2]; 
			uint32_t edgeIndex;
		};

		std::vector<Edge> edgeList(numEdges);

		std::vector<int> neighborEdgeIndices(numEdges ,-1 );
		//std::vector<uint32_t> neighborTriangleIndices(-1, numEdges);)
		 

		//fill edge list
		for (int i = 0; i < numTriangles; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				Edge edge;
				auto id0 = indices[i * 3 + j];
                auto id1 = indices[i * 3 + (j + 1) % 3];
				edge.vertexIndex[0] = std::min(id0, id1);
				edge.vertexIndex[1] = std::max(id0, id1);
				edge.edgeIndex = i * 3 + j;

				edgeList[i * 3 + j] = edge;
			}
		}

		//sort edge list
		std::sort(edgeList.begin(), edgeList.end(), [](const Edge& a, const Edge& b)
			{
				if (a.vertexIndex[0] == b.vertexIndex[0])
					return a.vertexIndex[1] < b.vertexIndex[1];
				else
					return a.vertexIndex[0] < b.vertexIndex[0];
			});


		//fill neighbor edge indices
        for (int i = 0; i < numEdges; i++)
		{
			auto edge0 = edgeList[i];
			auto vertex0 = edge0.vertexIndex[0];
			auto vertex1 = edge0.vertexIndex[1];

			//find neighbor edge
			if( i < numEdges - 1)
			{
				auto nextEdge = edgeList[i+1];
				auto nextVertex0 = nextEdge.vertexIndex[0];
				auto nextVertex1 = nextEdge.vertexIndex[1];

				if (nextVertex0 == vertex0 && nextVertex1 == vertex1)
				{
					neighborEdgeIndices[edge0.edgeIndex] = nextEdge.edgeIndex;
					neighborEdgeIndices[nextEdge.edgeIndex] = edge0.edgeIndex;
				}
			}
		}



		//add the distance constraints
		for (int i = 0; i < numTriangles; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				Edge edge;
				auto id0 = indices[i * 3 + j];
				auto id1 = indices[i * 3 + (j + 1) % 3];

				auto neighborEdgeIndex = neighborEdgeIndices[i * 3 + j];
				 
				//std::cout << "id0: "<< id0 << " id1: " << id1 << " neighborEdgeIndex:" << neighborEdgeIndex << std::endl;
				//if no neighbor edge;  if has, only add once for id0 < id1 ones
				if ((neighborEdgeIndex < 0) || (id0 < id1))
{ 
					//std::cout << "pushed id0: " << id0 << " id1: " << id1 << " neighborEdgeIndex: " << neighborEdgeIndex << std::endl;
				    m_edgePairIDs.push_back(id0);
				    m_edgePairIDs.push_back(id1); 
				} 

				//triangle pairs , if has, only add once for id0 < id1 ones
				//id2 - id0,
				// |  /  |
				//id1 - id3
				if (neighborEdgeIndex > 0 && id0 < id1)
				{
					auto tri_index = std::floor(neighborEdgeIndex / 3);
					auto edge_num = neighborEdgeIndex % 3;

					//the opposite vertex
					auto id2 = indices[i * 3 + (j + 2) % 3];
					auto id3 = indices[tri_index * 3 + (edge_num + 2) % 3];

					m_triPairIDs.push_back(id0);
					m_triPairIDs.push_back(id1);
					 
                    m_triPairIDs.push_back(id2);
					m_triPairIDs.push_back(id3);


				}  
			}

		} 

		m_num_edgePair = m_edgePairIDs.size() / 2;
		m_num_triPair =  m_triPairIDs.size() / 4;

		m_num_stretchConst = m_num_edgePair;
		m_num_bendConst = m_num_triPair;
		 

		//now that we use atomic, this data structure is legacy
		if (useLegacyShader)
		{
		//extract references to all influence particles
		m_stretchConstIDs = std::vector<stretchConstIDs>(m_num_particles);

		//the temporary stack;
		std::vector < std::vector<uint32_t>> stretchConstIDs(m_num_particles);
	 
		//for edge pair
		for (int i = 0; i < m_num_edgePair; i++)
		{
			auto id0 = m_edgePairIDs[i * 2];
			auto id1 = m_edgePairIDs[i * 2 + 1];

			stretchConstIDs[id0].push_back(id1);
			stretchConstIDs[id1].push_back(id0); 
		}

		 ////for triangle pair,  
		 for (int i = 0; i < m_num_triPair; i++)
		 {
		 	auto id0 = m_triPairIDs[i * 4];
		 	auto id1 = m_triPairIDs[i * 4 + 1];
		 	auto id2 = m_triPairIDs[i * 4 + 2];
		 	auto id3 = m_triPairIDs[i * 4 + 3];
		 
		 	stretchConstIDs[id2].push_back(id3);
		 	stretchConstIDs[id3].push_back(id2);  

			//std::cout << "tri pair push: "  << id2 << " " << id3 << std::endl;
		 }


	    //fill the stretchConstIDs
		for (int i = 0; i < m_num_particles; i++)
		{
			auto& ids = stretchConstIDs[i];
			auto count = ids.size();
			if(count > MAX_CONSTRAINTS)
			{
				RD_CORE_ERROR("Cloth: particle{0} has constriant{1}, the mesh is in bad topo",i, count); 
			}
			//also fill the rest with -1
			for (int j = 0; j < count; j++)
			{ 
				//std::cout << "stretchConstIDs: " << ids[j] << std::endl;
				m_stretchConstIDs[i].IDs[j] = ids[j];
			}

			for (int j = count; j < MAX_CONSTRAINTS; j++)
			{
				m_stretchConstIDs[i].IDs[j] = UINT32_MAX;
			}  

		}   


		}



		//debug:  
		if (false)
		{//sorted edges

            RD_CORE_INFO("positions: ");
			for (auto pos : m_mesh->positions)
			{
				std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
			}

			RD_CORE_INFO("indices: ");
			for (auto index : indices)
			{
				std::cout << index << " ";
			}
			std::cout << std::endl;

			for (int i = 0; i < numEdges; i++)
			{
				auto edge = edgeList[i];
				RD_CORE_INFO("sorted vid: {0},{1},edgeNum{2}", edge.vertexIndex[0], edge.vertexIndex[1], edge.edgeIndex);
			}

			//for (int i = 0; i < neighborEdgeIndices.size(); i++)
			//{
			//	RD_CORE_INFO("edge{0} neighbor: {1}", i, neighborEdgeIndices[i]);
			//}

			for (int i = 0; i < m_num_edgePair; i ++)
			{
				RD_CORE_INFO("edge pair IDs: {0}, {1}", m_edgePairIDs[i * 2], m_edgePairIDs[i * 2 + 1]);
			}

			for (int i = 0; i < m_num_triPair; i++)
			{
				RD_CORE_INFO("tri pair opposite IDs: {0}, {1}", m_triPairIDs[i * 4 + 2], m_triPairIDs[i * 4 +3]);
			}


			for (int i = 0; i < m_num_particles; i++)
			{
				auto& IDs = m_stretchConstIDs[i].IDs; 
				RD_CORE_INFO("stretchConstIDs of particle{0} :", i);
                for (int i = 0; i < MAX_CONSTRAINTS; i++)
				{
					std::cout << IDs[i] << " ";
				}
				std::cout << std::endl;
				 
			}  

		}
	 
		//===================================================================================================
		//init buffers 


		m_particle_position_buffer = StorageBuffer::Create();
		m_particle_position_buffer->SetData(m_positions.data(), m_positions.size() * sizeof(glm::vec4));

		m_particle_previous_position_buffer = StorageBuffer::Create();
		m_particle_previous_position_buffer->SetData(m_positions.data(), m_positions.size() * sizeof(glm::vec4));
		
		m_particle_rest_position_buffer = StorageBuffer::Create();
        m_particle_rest_position_buffer->SetData(m_positions.data(), m_positions.size() * sizeof(glm::vec4));

		 
		m_particle_velocity_buffer = StorageBuffer::Create();
		m_particle_velocity_buffer->SetData(nullptr, m_positions.size() * sizeof(glm::vec4));
		
		m_particle_inv_mass_buffer = StorageBuffer::Create();
		m_particle_inv_mass_buffer->SetData(m_invMass.data(), m_invMass.size() * sizeof(float));

		m_particle_correction_buffer = StorageBuffer::Create();
		m_particle_correction_buffer->SetData(nullptr, m_positions.size() * sizeof(glm::vec4));
 
	 
		m_stretchConstPair_IDs_buffer = StorageBuffer::Create();
		m_stretchConstPair_IDs_buffer->SetData(m_edgePairIDs.data(), m_edgePairIDs.size() * sizeof(uint32_t));

		m_bendConstPair_IDs_buffer = StorageBuffer::Create();
		m_bendConstPair_IDs_buffer->SetData(m_triPairIDs.data(), m_triPairIDs.size() * sizeof(uint32_t));


		m_stretchConst_rest_length_buffer = StorageBuffer::Create();
		m_stretchConst_rest_length_buffer->SetData(nullptr, m_num_stretchConst * sizeof(float));

		m_bendConst_rest_length_buffer = StorageBuffer::Create();
		m_bendConst_rest_length_buffer->SetData(nullptr, m_num_bendConst * sizeof(float)); 

		m_particle_normal_buffer = StorageBuffer::Create();
		m_particle_normal_buffer->SetData(m_normals.data(),m_normals.size() * sizeof(glm::vec4));

		m_indices_WorldToView_buffer =  StorageBuffer::Create();
		m_indices_WorldToView_buffer->SetData(indices.data(), indices.size() * sizeof(uint32_t));
		 
		//legacy
		if(useLegacyShader)
		{
			m_stretchConst_IDs_buffer = StorageBuffer::Create();
			m_stretchConst_IDs_buffer->SetData(m_stretchConstIDs.data(), m_stretchConstIDs.size() * sizeof(stretchConstIDs));
		} 


		//shaders===========================================================================================


		 m_prePBD_integrate_shader = Shader::CreateComputeShader("integrate", "Shaders/Cloth/pre_integrate_CS.glsl");
		
		 m_update_solve_stretchConst_shader =  Shader::CreateComputeShader("solve_strechConst",
			 "Shaders/Cloth/solve_stretchConst_CS.glsl");
		 m_update_solve_bendConst_shader = Shader::CreateComputeShader("solve_bendConst",
			 "Shaders/Cloth/solve_bendConst_CS.glsl"); 
	 

		 m_update_solve_stretchConst_legacy_shader = Shader::CreateComputeShader("solve_distConst",
			 "Shaders/Cloth/solve_distConst_legacy_CS.glsl");

		 
		 m_postPBD_correction_shader =  Shader::CreateComputeShader("correction", "Shaders/Cloth/post_correction_CS.glsl");
		 m_postPBD_update_vel_shader = Shader::CreateComputeShader("update_vel", "Shaders/Cloth/post_update_vel_CS.glsl");


		 m_compute_normals_shader = Shader::CreateComputeShader("compute_normals", 
			 "Shaders/Cloth/compute_normals_CS.glsl");
		 m_normalize_normals_shader = Shader::CreateComputeShader("normalize_normals", 
			 "Shaders/Cloth/normalize_normals_CS.glsl"); 
		 
		 
		 m_compute_rest_length_shader = Shader::CreateComputeShader("compute_rest_length", "Shaders/Cloth/compute_rest_length_CS.glsl");

		  

		 //shader bindings
		 { 
			 m_indexBuffer = IndexBuffer::Create();
			 m_indexBuffer->SetData(indices.data(), indices.size() * sizeof(uint32_t));
			 std::cout << "indices num: " << m_mesh->indices.size()<< std::endl;

			 m_vertexArray = VertexArray::Create();
			 m_vertexArray->AttachIndexBuffer(m_indexBuffer->GetBufferID());

			 m_vertexArray->AttachBuffer(m_particle_position_buffer->GetBufferID(), 0, sizeof(glm::vec4));
			 m_vertexArray ->AddAttribute( 0, 0 , 4, BufferDataType::FLOAT32,0);  
			  
             m_vertexArray->AttachBuffer(m_particle_normal_buffer->GetBufferID(), 2, sizeof(glm::vec4));
             m_vertexArray->AddAttribute( 2, 2, 4, BufferDataType::FLOAT32, 0); 
 

		 }
		   


		 //init setup
		 this->ComputeStretchRestLengths();
		 this->ComputeBendRestLengths();

		 //debug
		 m_restLength = std::vector<float>(m_num_stretchConst, 0.0f);
		 if (false)
		 {

			 ////map the data
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_stretchConst_rest_length_buffer->GetBufferID());
			 float* restLength = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY); 
			 
			  std::cout << "stretchConst_rest_length_buffer: " << std::endl;
			  for (int i = 0; i < m_num_stretchConst; i++)
	         {
		        m_restLength[i] = restLength[i];
		        std::cout << "const"<< i <<" " << restLength[i] << " ";
			  }
			  std::cout << std::endl;

			 glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); 
			 glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); 

		 }


	}




	void Cloth::ComputeStretchRestLengths()
	{

		m_compute_rest_length_shader->Bind(); 
		m_particle_rest_position_buffer->BindBase(0);
		m_stretchConstPair_IDs_buffer   ->BindBase(1);
		m_stretchConst_rest_length_buffer ->BindBase(2);
		 
		if (m_num_stretchConst < 0)
		{
			RD_CORE_ERROR("cloth : m_num_stretchConst < 0");
			return; 
		}
		 
		uint32_t dispatchX = static_cast<uint32_t>(
			std::ceil((float)m_num_stretchConst / (float)m_local_size_x)); 

		glDispatchCompute(dispatchX, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		m_compute_rest_length_shader->Unbind();

	}


	void Cloth::ComputeBendRestLengths()
	{

		m_compute_rest_length_shader->Bind(); 
		m_particle_rest_position_buffer->BindBase(0);
		m_bendConstPair_IDs_buffer->BindBase(1);
		m_bendConst_rest_length_buffer->BindBase(2);

		if (m_num_bendConst < 0)
		{
			RD_CORE_ERROR("cloth : m_num_stretchConst < 0");
			return;
		}

		uint32_t dispatchX = static_cast<uint32_t>(
			std::ceil((float)m_num_bendConst / (float)m_local_size_x));

		glDispatchCompute(dispatchX, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		m_compute_rest_length_shader->Unbind();

	}

	 


	void Cloth::PBD_step()
	{
		//integrate
		{
			m_prePBD_integrate_shader->Bind(); 

			m_prePBD_integrate_shader->SetFloat("u_deltaTime", m_deltaTime);
			m_prePBD_integrate_shader->SetFloat("u_gravity", m_gravity);
			m_prePBD_integrate_shader->SetUInt("u_num_particles", m_num_particles);


			m_particle_position_buffer->BindBase(0);
			m_particle_previous_position_buffer->BindBase(1);
			m_particle_correction_buffer->BindBase(2);
			m_particle_velocity_buffer->BindBase(3);
			m_particle_inv_mass_buffer->BindBase(4);
			m_particle_normal_buffer->BindBase(5);

			
			uint32_t dispatchX = static_cast<uint32_t>(
				std::ceil((float)m_num_particles / (float)m_local_size_x));

			glDispatchCompute(dispatchX, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_prePBD_integrate_shader->Unbind();

		}

 

		//solve stretch constraints 
		// atomic issue solved
		if (true)
		{

			m_update_solve_stretchConst_shader->Bind(); 
			m_update_solve_stretchConst_shader->SetFloat("u_deltaTime", m_deltaTime);
			m_update_solve_stretchConst_shader->SetFloat("u_compliance", m_stretch_compliance);
			m_update_solve_stretchConst_shader->SetUInt("u_num_distConst",  m_num_stretchConst);
	 
			m_particle_position_buffer->BindBase(0); 
			m_particle_correction_buffer->BindBase(2);
			m_particle_inv_mass_buffer->BindBase(3);

			m_stretchConstPair_IDs_buffer->BindBase(4); 
			m_stretchConst_rest_length_buffer->BindBase(5); 


			uint32_t dispatchX = static_cast<uint32_t>( 
			    std::ceil((float)m_num_stretchConst / (float)m_local_size_x));
			
			glDispatchCompute(dispatchX, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_update_solve_stretchConst_shader->Unbind();

		}



		if (true)
		{
			//bend
			m_update_solve_bendConst_shader->Bind();
			m_update_solve_bendConst_shader->SetFloat("u_deltaTime", m_deltaTime);
			m_update_solve_bendConst_shader->SetFloat("u_compliance", m_bend_compliance	);

			m_update_solve_bendConst_shader->SetUInt("u_num_distConst", m_num_bendConst);
 
			m_particle_position_buffer->BindBase(0);
			m_particle_correction_buffer->BindBase(2);
			m_particle_inv_mass_buffer->BindBase(3);

			//bend
			m_bendConstPair_IDs_buffer->BindBase(4);
			m_bendConst_rest_length_buffer->BindBase(5);


			uint32_t dispatchX = static_cast<uint32_t>( 
				std::ceil((float)m_num_bendConst / (float)m_local_size_x));

			glDispatchCompute(dispatchX, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_update_solve_bendConst_shader->Unbind();

		}




		 
		 
		if (useLegacyShader)
		{
			RD_PROFILE_SCOPE("solve constriants");

			m_update_solve_stretchConst_legacy_shader->Bind();
			m_update_solve_stretchConst_legacy_shader->SetFloat("u_deltaTime", m_deltaTime);
			m_update_solve_stretchConst_legacy_shader->SetFloat("u_compliance", 0.0f);
			m_update_solve_stretchConst_legacy_shader->SetUInt("u_max_num_distConst", MAX_CONSTRAINTS);
			m_update_solve_stretchConst_legacy_shader->SetUInt("u_num_particles", m_num_particles);

			m_particle_position_buffer->BindBase(0);
			m_particle_rest_position_buffer->BindBase(1);
			m_particle_correction_buffer->BindBase(2);
			m_particle_inv_mass_buffer->BindBase(3);

			m_stretchConst_IDs_buffer->BindBase(4);

			//m_stretchConst_rest_length_buffer->BindBase(5); 

			uint32_t dispatchX = static_cast<uint32_t>(
				std::ceil((float)m_num_particles / (float)m_local_size_x)); 
			glDispatchCompute(dispatchX, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_update_solve_stretchConst_legacy_shader->Unbind();

		}
		 

		 
		//debug
		if(false)
		{ 
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_stretchConstPair_IDs_buffer->GetBufferID());
			uint32_t* _stretchConstIDs = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << " buffer map: stretchConst pairs: " << std::endl;

			for (int i = 0; i < m_num_stretchConst; i++)
			{
				std::cout << _stretchConstIDs[i * 2] << " " << _stretchConstIDs[i * 2 + 1] << std::endl;
			} 
			 
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



		}
	 
		 //debug
		if (false)
		{
			RD_CORE_INFO("Correction:");
			//map the data
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particle_correction_buffer->GetBufferID());
			glm::vec4* correction= (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << "particle_correction_buffer: " << std::endl;
			for (int i = 0; i < m_num_particles; i++)
			{
				 std::cout << correction[i].x << " " << correction[i].y << " " << correction[i].z << " " << correction[i].w << std::endl;
			} 
			std::cout << std::endl; 
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); 

		}


		//post PBD correction
		if (true)
		{
			m_postPBD_correction_shader->Bind();

			m_postPBD_correction_shader->SetFloat("u_deltaTime", m_deltaTime);
			m_postPBD_correction_shader->SetFloat("u_jacobiScale", 0.2f);

			m_postPBD_correction_shader->SetUInt("u_num_particles", m_num_particles);

			m_particle_position_buffer         ->BindBase(0);
			m_particle_previous_position_buffer->BindBase(1);
			m_particle_correction_buffer       ->BindBase(2);
			m_particle_velocity_buffer         ->BindBase(3);
			m_particle_inv_mass_buffer         ->BindBase(4);

 
			uint32_t dispatchX = static_cast<uint32_t>(
				std::ceil((float)m_num_particles / (float)m_local_size_x));
			glDispatchCompute(m_num_particles, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_postPBD_correction_shader->Unbind();

			 
		}
  


		if (false)
		{
			RD_CORE_INFO("after correction:");
			//map the data
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particle_position_buffer->GetBufferID());
			glm::vec4* restLength = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << "particle_position_buffer: " << std::endl;
			for (int i = 0; i < m_num_particles; i++)
			{
				std::cout << restLength[i].x << " " << restLength[i].y << " " << restLength[i].z << " " << restLength[i].w << std::endl;
			}

			std::cout << std::endl;

			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		}


	} 



	void Cloth::ComputeNormals()
	{


		//compute normals
		if (true)
		{

			m_compute_normals_shader->Bind();
			m_compute_normals_shader->SetUInt("u_num_triangles", m_num_triangles);

			m_particle_position_buffer->BindBase(0);
			m_particle_normal_buffer->BindBase(1);
			m_indices_WorldToView_buffer->BindBase(2);


			uint32_t dispatchX = static_cast<uint32_t>(
				std::ceil((float)m_num_triangles / (float)m_local_size_x));
			glDispatchCompute(dispatchX, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_compute_normals_shader->Unbind();


		}

		if (true)
		{
			m_normalize_normals_shader->Bind();
			m_normalize_normals_shader->SetUInt("u_num_vertices", m_num_particles);

			m_particle_normal_buffer->BindBase(0);


			uint32_t dispatchX = static_cast<uint32_t>(
				std::ceil((float)m_num_particles / (float)m_local_size_x));
			glDispatchCompute(dispatchX, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			m_normalize_normals_shader->Unbind();
		}

	}
	 

		 







	void Cloth::CPU_Update()
	{

		//CPU integrate
		std::vector<glm::vec4> prev_position(m_num_particles);
		{
			prev_position = m_positions;
			for (int i = 0; i < m_num_particles; i++)
			{
				auto invMass = m_invMass[i];
				if (invMass == 0.0f)
					continue;

				m_velocities[i].y += m_gravity * m_deltaTime;
				m_velocities[i] *= 0.999f;
				m_positions[i] += m_velocities[i] * m_deltaTime;

			}

		}

		//CPU solve constraints
		//std::fill(m_correction.begin(), m_correction.end(), glm::vec4(0.0));
		m_correction = std::vector<glm::vec4>(m_num_particles, glm::vec4(0.0f));
		if (true)
		{
			//RD_CORE_INFO("substep start: ");
			for (int i = 0; i < m_num_stretchConst; i++)
			{
				auto id0 = m_edgePairIDs[i * 2];
				auto id1 = m_edgePairIDs[i * 2 + 1];

			 
				auto invMass0 = m_invMass[id0];
				auto invMass1 = m_invMass[id1];
				if (invMass0 == 0.0f && invMass1 == 0.0f)
					continue;

				auto restLen = m_restLength[i];  

				auto pos0 = m_positions[id0];
				auto pos1 = m_positions[id1];


				auto grad = pos0 - pos1 ;
				auto Len = glm::length(grad); 

				auto error = (Len - restLen);

				auto lambda = -error / 2;
				 
				
			//if (id1 == id0 + 4)
			//{
			//	std::cout << "CPU troll: id0: " << id0 << " id1: " << id1 << std::endl;
			//
			//	lambda = 100.0f;
			//	continue;
			//}

				auto correction0 = lambda* invMass0 * grad / Len  ;
				auto correction1 = lambda* invMass1 * grad / Len  ; 
		
				m_correction[id0] += correction0;
				m_correction[id1] -= correction1; 

			}


		}

		//debug
		if (false)
		{

			std::cout << "cpu Correction:" << std::endl;
			for (int i = 0; i < m_num_particles; i++)
			{
				std::cout << m_correction[i].x << " " << m_correction[i].y << " " << m_correction[i].z << " " << m_correction[i].w << std::endl;
			}
			std::cout << std::endl;

		}


		//post PBD correction  
		{

			for (int i = 0; i < m_num_particles; i++)
			{
				auto invMass = m_invMass[i];
				if (invMass == 0.0f)
					continue;

				m_positions[i] += m_correction[i] * 0.3f;

				m_velocities[i] = (m_positions[i] - prev_position[i]) / m_deltaTime;
  
			}

		}
		 

		{
			m_particle_position_buffer->SetData(m_positions.data(), m_positions.size() * sizeof(glm::vec4));
		}



	}





	void Cloth::Draw(Ref<Camera> cam)
	{

		if (!hasMesh())
		{
			RD_CORE_ERROR("MeshObject::Draw: no mesh attached");
			return;
		}
		if (m_material == nullptr)
		{
			RD_CORE_ERROR("MeshObject::Draw: no material attached");
			return;
		}

		m_vertexArray->Bind();
		m_material->Bind();

		if (cam != nullptr)
		{
			glm::mat4 model = glm::mat4(1.0f);
			m_material->GetShader()->SetMat4("u_model", model);
			m_material->GetShader()->SetMat4("u_projection", cam->GetProjectionMatrix());
			m_material->GetShader()->SetMat4("u_view", cam->GetViewMatrix());

		}

		 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//
		//std::cout << "draw command: " << (int)m_mesh->drawCommand << std::endl;  
		//std::cout << "indices num: " << m_mesh->GetIndexCount() << std::endl;
		//std::cout << "topo: " << (int)m_mesh->topology << std::endl;
		switch (m_mesh->drawCommand)
		{
		case MeshDrawCommand::INDEXED:
			Renderer::GetRendererAPI()->DrawIndexed(m_mesh->topology, m_mesh->GetIndexCount());
			break;
		case MeshDrawCommand::ARRAYS:
		{
			Renderer::GetRendererAPI()->DrawArrays(m_mesh->topology, m_mesh->GetVertexCount());
			break;
		}

		}


		m_material->Unbind();
		m_vertexArray->Unbind();

	}


}



 