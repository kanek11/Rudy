#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/Object.h"
#include "Rudy/Renderer/Buffer.h"
#include "Rudy/Renderer/RendererComponent.h"


namespace Rudy
{

	//structs are mere data containers to specify the layout for the buffers
	//they are structed the same way as in the shader structs

	//important warning: somehow the std140 and std430 layout  always round up to the next multiple of 16 bytes
	//so vec3 is really 16 bytes and access by index will cause a misalignment ,so just use vec4 instead
	struct Particle
	{
		glm::vec4 position;
		glm::vec4 velocity;
		  
		float lifeTime; 
		float age;
	};

	struct Counter
	{
		uint32_t deadCount;
		uint32_t aliveCount[2];  //pre-sim and post-sim
		uint32_t emissionCount;  //depend on dynamic , avaliable #dead particles
		uint32_t updateCount;    //depend on dynamic , avaliable #alive particles
	};
	 

	struct DispatchComputeIndirectCommand{
		uint32_t  numGroupsX = 1;
		uint32_t  numGroupsY = 1;
		uint32_t  numGroupsZ = 1;
	};




	class ParticleSystem
	{
	public: 
		ParticleSystem();
		~ParticleSystem();

		
		void Render(); 
	
	};

	//void OnUpdate(float deltaTime);
	//void OnRender();
	//void OnImGuiRender(); 


	class Emitter : public RenderableObject
	{
public: 
		Emitter();
		~Emitter(); 

	
		void Spawn();
		void Update();
		void Reset(); 
		
		   
	//inherit;
		void Draw(Ref<Camera> cam) override;
	
	public: 

		//system handles 
		float m_deltaTime = 0.0f;

		uint32_t m_local_size_x = 32;
		uint32_t m_local_size_y = 1;
		uint32_t m_local_size_z = 1; 
		 
		uint32_t m_maxParticleCount = 1024;

		uint32_t m_currentAliveCount = 0;

		float m_emissionAccumulator = 0.0f;

		//debug
		uint32_t totalEmissionCount = 0;
		float totalEmissionTime = 0.0f;


		//emitter spawn parameters

		glm::vec3 m_emitter_position = { 0.0f, 0.0f, 0.0f };

		float m_emissionRate = 64;
		float m_sphereRadius = 1.0f;
		 

		//emitter update parameters
		uint32_t m_preSimIndex = 0;
		uint32_t m_postSimIndex = 1;


		//particle spawn parameters  
		float m_particle_minLifetime = 3.0f;
		float m_particle_maxLifetime = 5.0f;
		float m_particle_minInitialSpeed = 5.0f;
		float m_particle_maxInitialSpeed = 10.0f;
 
		 

		//particle simulation/update parameters
		 

	public:
		//system handles 
		//======shader programs======//
	//each shader is a stage in the stack;
		Ref<Shader> m_particle_reset_compute_shader;
		Ref<Shader> m_particle_dispatch_compute_shader;
		Ref<Shader> m_particle_emission_compute_shader;
		Ref<Shader> m_particle_update_compute_shader; 


		//======buffers======//

		//particles attributes
		Ref<StorageBuffer> m_particle_position_buffer;
		Ref<StorageBuffer> m_particle_velocity_buffer;
		Ref<StorageBuffer> m_particle_age_buffer;
		Ref<StorageBuffer> m_particle_lifeTime_buffer;

		//management of alive/dead particles
		Ref<StorageBuffer> m_aliveList_buffer[2];
		Ref<StorageBuffer> m_deadList_buffer;
		Ref<StorageBuffer> m_counter_buffer;

		//indirect for dispatch of each stage ; for dynamic length updated by compute shader 
		Ref<StorageBuffer> m_indirect_dispatch_update_buffer; 
		Ref<StorageBuffer> m_indirect_dispatch_emission_buffer;

		
		
		 

	public:
		//meta data
		std::string m_name = "Unnamed";


	};

 
	 

	 
	//float rotation = 0.0f;
	//float lifeRemaining = 0.0f;
	//bool active = false; 
	//glm::vec4 colorBegin, colorEnd; 
	//float sizeBegin, sizeEnd;

	 


}