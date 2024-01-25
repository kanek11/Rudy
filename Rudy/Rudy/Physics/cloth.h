#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/Object.h"
#include "Rudy/Renderer/Buffer.h"
#include "Rudy/Renderer/RendererComponent.h"


namespace Rudy
{

	const uint32_t MAX_CONSTRAINTS = 16;

	struct stretchConstIDs
	{
		uint32_t IDs[MAX_CONSTRAINTS];
	};
	 

	class Cloth  
	{
	public:
		~Cloth() = default;
         Cloth();

		void Init();
		void Update();
		void CPU_Update();
		void PBD_step();
		void ComputeStretchRestLengths();
		void ComputeBendRestLengths();
		void  ComputeNormals();


		uint32_t m_num_subSteps = 16;
		float m_stretch_compliance = 0.0f;
		float m_bend_compliance = 3.0f;

		bool useLegacyShader = false;



		std::vector<glm::vec4> m_correction; 

		//constraints
		std::vector< uint32_t> m_edgePairIDs;
		std::vector< uint32_t> m_triPairIDs;   
		 
		Ref<StorageBuffer> m_stretchConstPair_IDs_buffer;
		Ref<StorageBuffer> m_bendConstPair_IDs_buffer; 

		std::vector<float>  m_restLength;
		std::vector<float> m_invMass;
		std::vector<glm::vec4> m_positions;
		std::vector<glm::vec4> m_velocities;
		std::vector<glm::vec4> m_normals;


		uint32_t m_num_particles = 0;
		uint32_t m_num_triangles = 0;
		uint32_t m_num_edges = 0;

		uint32_t m_num_edgePair = 0;
		uint32_t m_num_triPair = 0;

		uint32_t m_num_stretchConst = 0;
		uint32_t m_num_bendConst = 0;


		void Draw(Ref<Camera> cam) ;

		//fornow:
	public:
		void SetMesh(Ref<Mesh> m);
		Ref<Mesh> GetMesh() { return m_mesh; }
		bool hasMesh() { return m_mesh != nullptr; }

	public:
		Ref<Mesh> m_mesh = nullptr;
		Ref<Material> m_material = nullptr; 
		Ref<VertexArray> m_vertexArray = nullptr;
        Ref<IndexBuffer> m_indexBuffer = nullptr;



//cloth parameters
		
		float m_gravity = -9.8f;
		 

	public:
		float m_system_dt  = 0.0f;
		float m_time_accumulator = 0.0f;
		float m_deltaTime = 0.0f;

		float m_FPS = 24.0f;
		float m_solver_deltaTime = 0.0f;

		uint32_t m_local_size_x = 32;
		uint32_t m_local_size_y = 1;
		uint32_t m_local_size_z = 1; 

		



	public:
	   //buffers
	   //particles attributes
		Ref<StorageBuffer> m_particle_position_buffer;  
		Ref<StorageBuffer> m_particle_rest_position_buffer;
		Ref<StorageBuffer> m_particle_previous_position_buffer;
		
		Ref<StorageBuffer> m_particle_correction_buffer;//jacobi iteration

		Ref<StorageBuffer> m_particle_velocity_buffer; 
		Ref<StorageBuffer> m_particle_inv_mass_buffer;  
	
		//for rendering, vertex normals
		Ref<StorageBuffer> m_particle_normal_buffer; 
		Ref<StorageBuffer> m_indices_WorldToView_buffer;

		Ref<StorageBuffer> m_stretchConst_rest_length_buffer;
		Ref<StorageBuffer> m_bendConst_rest_length_buffer;
		 

		//kernels/compute shaders
		 Ref<Shader> m_prePBD_integrate_shader;
		 Ref<Shader> m_update_solve_stretchConst_shader;
		 Ref<Shader> m_update_solve_bendConst_shader;
		 Ref<Shader> m_postPBD_correction_shader;
		 Ref<Shader> m_postPBD_update_vel_shader;
		  
		 Ref<Shader> m_compute_normals_shader;
		 Ref<Shader> m_normalize_normals_shader;

		 Ref<Shader> m_compute_rest_length_shader;
		  

		 //legacy use accumulations 
		 std::vector<stretchConstIDs> m_stretchConstIDs;
		 std::vector<stretchConstIDs> m_bendConstIDs;

		 Ref<Shader> m_update_solve_stretchConst_legacy_shader;

		 Ref<StorageBuffer> m_stretchConst_IDs_buffer;
		 Ref<StorageBuffer> m_bendConst_IDs_buffer;


	};








}