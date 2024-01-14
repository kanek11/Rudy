
#include "RudyPCH.h"
#include "ParticleSystem.h"

namespace Rudy
{

	Emitter::Emitter(): RenderableObject()
	{

		RD_CORE_INFO("Emitter {0} created:", m_name);
		
	}

    Emitter::~Emitter()
	{
		RD_CORE_INFO("Emitter {0} destroyed", m_name);
	}

	void Emitter::Draw(Ref<Camera> cam)
	{ 
		this->GetRendererComponent()->DrawInstanced(cam, m_currentAliveCount);
	}



	void Emitter::Spawn() {

		//the initiaization of draw indirect buffer is now handled by the renderer
		//but we keep the structure consistent and simple by simply assigning the draw indirect buffer
		//auto DrawArraysIndirectCommand = new DrawArraysIndirectCommand();
  
		auto dispatchComputeIndirectCommand = new DispatchComputeIndirectCommand();
 
		//handles rendering
		//we get it from component, to keep the code more consistent
		//the intialization, data copy is handled by the renderer, also some infos are needed for the shader.
		{
			if(!this->hasRendererComponent())
			{
				RD_CORE_ERROR("Emitter {0} has no renderer component ", m_name); 
			} 

		} 


		//----------------------------------
		//initialize buffers 
		{
			m_particle_position_buffer = StorageBuffer::Create();
			m_particle_velocity_buffer = StorageBuffer::Create();
			m_particle_age_buffer =      StorageBuffer::Create();
			m_particle_lifeTime_buffer = StorageBuffer::Create();
		 
			m_particle_position_buffer->SetData(nullptr, m_maxParticleCount * sizeof(glm::vec4),  BufferUsage::DYNAMIC_COPY);
			m_particle_velocity_buffer->SetData(nullptr, m_maxParticleCount * sizeof(glm::vec4),  BufferUsage::DYNAMIC_COPY);
			m_particle_age_buffer     ->SetData(nullptr, m_maxParticleCount * sizeof(float),      BufferUsage::DYNAMIC_COPY);
			m_particle_lifeTime_buffer->SetData(nullptr, m_maxParticleCount * sizeof(float),      BufferUsage::DYNAMIC_COPY);
			//std::cout <<  "size of glm vec3 is: " << sizeof(glm::vec3) << std::endl;

			m_aliveList_buffer[0] = StorageBuffer::Create();
			m_aliveList_buffer[1] = StorageBuffer::Create();
			m_deadList_buffer     = StorageBuffer::Create();
			m_counter_buffer      = StorageBuffer::Create();

			m_aliveList_buffer[0]->SetData(nullptr, m_maxParticleCount * sizeof(uint32_t), BufferUsage::DYNAMIC_COPY);
			m_aliveList_buffer[1]->SetData(nullptr, m_maxParticleCount * sizeof(uint32_t), BufferUsage::DYNAMIC_COPY);
			m_deadList_buffer    ->SetData(nullptr, m_maxParticleCount * sizeof(uint32_t), BufferUsage::DYNAMIC_COPY);
			m_counter_buffer     ->SetData(nullptr, sizeof(Counter),                       BufferUsage::DYNAMIC_COPY);

	 
			m_indirect_dispatch_update_buffer = StorageBuffer::Create(); 
			m_indirect_dispatch_update_buffer->SetData(dispatchComputeIndirectCommand, sizeof(DispatchComputeIndirectCommand), BufferUsage::DYNAMIC_COPY);

			m_indirect_dispatch_emission_buffer = StorageBuffer::Create();
			m_indirect_dispatch_emission_buffer->SetData(dispatchComputeIndirectCommand, sizeof(DispatchComputeIndirectCommand), BufferUsage::DYNAMIC_COPY);
		 
		 


		}

		//----------------------------------

		//----------------------------------
		//initialize shaders

		{
			m_particle_reset_compute_shader    = Shader::CreateComputeShader("particle reset",  "Shaders/Particles/particle_reset_CS.glsl");
			m_particle_emission_compute_shader = Shader::CreateComputeShader("particle emit",   "Shaders/Particles/particle_emission_CS.glsl");
			m_particle_update_compute_shader   = Shader::CreateComputeShader("particle update", "Shaders/Particles/particle_update_CS.glsl");
			
			m_particle_dispatch_compute_shader = Shader::CreateComputeShader("particle dispatch", "Shaders/Particles/particle_dispatch_CS.glsl");

			////----------------------------------
			//m_particle_reset_compute_shader->Bind(); 
            //m_particle_reset_compute_shader->Unbind();
			//
			//
		 	//
			//// ------------------------------
			//m_particle_emission_compute_shader->Bind();  
			//m_particle_emission_compute_shader->Unbind();
			//
			//
            ////----------------------------------
			//
			//m_particle_update_compute_shader->Bind(); 
			//m_particle_update_compute_shader->Unbind();
 
			  
		} 
		//reset once
		this->Reset(); 

	}

	void Emitter::Reset()
	{ 
		m_particle_reset_compute_shader->Bind(); 

		m_counter_buffer->BindBase(0);
		m_deadList_buffer->BindBase(1);
		m_indirect_dispatch_update_buffer->BindBase(2);
		m_indirect_dispatch_emission_buffer->BindBase(3);

		//set uniforms
		m_particle_reset_compute_shader->SetUInt("u_maxParticleCount", m_maxParticleCount);

		uint32_t dispatchX = static_cast<uint32_t>(
			std::ceil((float)m_maxParticleCount / (float)m_local_size_x));
		glDispatchCompute(dispatchX, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		m_particle_reset_compute_shader->Unbind();
 
	}


	void Emitter::Update() { 


		//utility computation 
	    // calculate emission count
		//auto emissionCount = static_cast<uint32_t>(
		//	ceil(m_deltaTime * m_emissionRate));

		m_emissionAccumulator += m_deltaTime * m_emissionRate;
		 
		 //integer part,  eg: 1.5 -> 1
		uint32_t emissionCount = static_cast<uint32_t>(
			floor(m_emissionAccumulator));
		
		totalEmissionCount += emissionCount;
		 totalEmissionTime += m_deltaTime ;
		//fractional part is left in the accumulator
		if(emissionCount > 0)
	    m_emissionAccumulator -= static_cast<float>(emissionCount);
		
		//std::cout << "emission this frame: " << m_deltaTime * m_emissionRate << std::endl;
		//std::cout << "emission accumulator: " << m_emissionAccumulator << std::endl;
		//std::cout << "emission count: " << emissionCount << std::endl;

		if(false)
		if (totalEmissionCount > m_emissionRate)
		{
			RD_CORE_INFO( "total emission count:{0}", totalEmissionCount );
			RD_CORE_INFO( "total emission time: {0} " ,totalEmissionTime  );
			totalEmissionCount = 0;
			totalEmissionTime = 0.0f;


			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counter_buffer->GetBufferID());
			Counter* counter = (Counter*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
				 
			std::cout << "pre sim alive count: " << std::endl;
			std::cout << counter->aliveCount[m_preSimIndex] << std::endl; 
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

			 std::cout << std::endl;
		}

		 
		//loop control
		if(false)
		{
			 
			 this->Reset(); 
 
		}


		//dispatch 
		{ 

			m_particle_dispatch_compute_shader->Bind();


			m_particle_dispatch_compute_shader->SetUInt("u_local_size_x", m_local_size_x);
			m_particle_dispatch_compute_shader->SetUInt("u_emission_count", emissionCount);
			m_particle_dispatch_compute_shader->SetUInt("u_preSimIndex", m_preSimIndex);
			m_particle_dispatch_compute_shader->SetUInt("u_postSimIndex", m_postSimIndex);

			m_counter_buffer->BindBase(0);
			m_indirect_dispatch_update_buffer->BindBase(1);
			m_indirect_dispatch_emission_buffer->BindBase(2);


			glDispatchCompute(1, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			 

			m_particle_dispatch_compute_shader->Unbind();
		}



		if (false)
		{
			std::cout << "after dispatch: " << std::endl;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counter_buffer->GetBufferID());
			Counter* counter = (Counter*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			 
			std::cout << "dead count: " << std::endl;
			std::cout << counter->deadCount << std::endl;

			std::cout << "pre sim alive count: " << std::endl;
			std::cout << counter->aliveCount[m_preSimIndex] << std::endl;
			
			std::cout << "emission count: " << std::endl;
			std::cout << counter->emissionCount << std::endl;

			std::cout << "update count: " << std::endl;
		    std::cout << counter->updateCount << std::endl;


			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indirect_dispatch_emission_buffer->GetBufferID());
			DispatchComputeIndirectCommand* emission= (DispatchComputeIndirectCommand*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << "dispatch emission size X: " << std::endl;
			std::cout << emission->numGroupsX << std::endl; 
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indirect_dispatch_update_buffer->GetBufferID());
			DispatchComputeIndirectCommand* update = (DispatchComputeIndirectCommand*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << "dispatch update size X: " << std::endl;
			std::cout << update->numGroupsX << std::endl; 
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

			std::cout << std::endl;

		}

		 

		//emission
		{
			//random generator
		
			std::uniform_real_distribution<float> distribution(0.1f, 1000.0f);
			std::random_device m_random;
			std::mt19937 generator(m_random());

			auto seeds = glm::vec3(distribution(generator),
				distribution(generator), distribution(generator));




			m_particle_emission_compute_shader->Bind();

			//set uniforms  
			// 
			//emitter spawn
			m_particle_emission_compute_shader->SetVec3("u_seeds", seeds);
			 

			m_particle_emission_compute_shader->SetVec3("u_emitter_position", m_emitter_position);
			m_particle_emission_compute_shader->SetFloat("u_sphereRadius", m_sphereRadius);


			//====emitter update
            m_particle_emission_compute_shader->SetUInt("u_preSimIndex", m_preSimIndex);

			//====particle spawn
			m_particle_emission_compute_shader->SetFloat("u_particle_minLifetime", m_particle_minLifetime);
			m_particle_emission_compute_shader->SetFloat("u_particle_maxLifetime", m_particle_maxLifetime);
			m_particle_emission_compute_shader->SetFloat("u_particle_minInitialSpeed", m_particle_minInitialSpeed);
			m_particle_emission_compute_shader->SetFloat("u_particle_maxInitialSpeed", m_particle_maxInitialSpeed);



			//buffer
			m_counter_buffer->BindBase(0);
			m_deadList_buffer->BindBase(1);
			m_aliveList_buffer[m_preSimIndex]->BindBase(2);
			 

			m_particle_position_buffer->BindBase(5);
			m_particle_velocity_buffer->BindBase(6);
			m_particle_lifeTime_buffer->BindBase(7);
			m_particle_age_buffer->BindBase(8);



			//uint32_t dispatchX = static_cast<uint32_t>(
			//	std::ceil( (float)emissionCount / (float)m_local_size_x ) );
			//glDispatchCompute(dispatchX, 1, 1);
			//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			//RD_CORE_INFO("emission dispatch: {0}", dispatchX);
			glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, m_indirect_dispatch_emission_buffer->GetBufferID());
			glDispatchComputeIndirect(0);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
			 
			m_particle_emission_compute_shader->Unbind();

		}


		//debug here
		if(false)
		{// 
		    std::cout << "after emission: " << std::endl;
			//std::cout << "pre sim index: " << m_preSimIndex << std::endl;
			//std::cout << "post sim index: " << m_postSimIndex << std::endl; 
			//
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_deadList_buffer->GetBufferID());
			//uint32_t* deadList = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			//
			//std::cout << "deadlist : " << std::endl;
			//for(int i = 0; i < 10; i++)
			//{
			//	std::cout << deadList[i] << " ";
			//}
			//std::cout << std::endl;
			//
			//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			//
			////
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_aliveList_buffer[0]->GetBufferID());
			//uint32_t* aliveList0 = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			//
			//std::cout << "aliveList0: " << std::endl;
			//for (int i = 0; i < 10; i++)
			//{
			//	std::cout << aliveList0[i] << " ";
			//}
			//std::cout << std::endl;
			//
			//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			//
			////
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_aliveList_buffer[1]->GetBufferID());
			//uint32_t* aliveList1 = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			//
			//std::cout << "aliveList1: " << std::endl;
			//for (int i = 0; i < 10; i++)
			//{
			//	std::cout << aliveList1[i] << " ";
			//}
			//std::cout << std::endl;
			//
			//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

			//
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counter_buffer->GetBufferID());
			Counter* counter = (Counter*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << "deadcount: " << std::endl;
			std::cout << counter->deadCount << std::endl;

			std::cout << "alivecount pre sim(should =update count): " << std::endl;
			 std::cout << counter->aliveCount[m_preSimIndex] << std::endl; 


			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

			//dispatch update: 
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indirect_dispatch_update_buffer->GetBufferID());
			DispatchComputeIndirectCommand* dispatchUpdate = (DispatchComputeIndirectCommand*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

			std::cout << "dispatch update: " << std::endl;
            std::cout << dispatchUpdate ->numGroupsX << std::endl;

			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particle_position_buffer ->GetBufferID());
			//float* pos = (float *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			//
			//std::cout << "first 2 position: " << std::endl;
			//for (int i = 0; i < 3 * 2; i++)
			//{
			//	std::cout << pos[i] << " ";
			//}
			//std::cout << std::endl;
			//
			//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		}




		//update
		if(true)
		{ 
			
			m_particle_update_compute_shader->Bind();
			//u_deltaTime; 
			m_particle_update_compute_shader->SetFloat("u_deltaTime", m_deltaTime);

			//u_preSimIndex
			m_particle_update_compute_shader->SetUInt("u_preSimIndex", m_preSimIndex);
			m_particle_update_compute_shader->SetUInt("u_postSimIndex", m_postSimIndex);


			//buffers 
			m_counter_buffer->BindBase(0);
			m_deadList_buffer->BindBase(1);
			m_aliveList_buffer[m_preSimIndex]->BindBase(2);
			m_aliveList_buffer[m_postSimIndex]->BindBase(3); 

			m_particle_position_buffer->BindBase(5);
			m_particle_velocity_buffer->BindBase(6);
			m_particle_lifeTime_buffer->BindBase(7);
			m_particle_age_buffer->BindBase(8);


			glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, m_indirect_dispatch_update_buffer->GetBufferID());
			glDispatchComputeIndirect(0);   
			glMemoryBarrier(GL_ALL_BARRIER_BITS); 
			glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);

			m_particle_update_compute_shader->Unbind();

		}


		if (false)
		{
		 RD_CORE_INFO("after update: ");

		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_aliveList_buffer[0]->GetBufferID());
		//uint32_t* aliveList0 = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		//
		//std::cout << "aliveList0: " << std::endl;
		//for (int i = 0; i < 10; i++)
		//{
		//	std::cout << aliveList0[i] << " ";
		//}
		//std::cout << std::endl;
		//
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//
		////
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_aliveList_buffer[1]->GetBufferID());
		//uint32_t* aliveList1 = (uint32_t*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		//
		//std::cout << "aliveList1: " << std::endl;
		//for (int i = 0; i < 10; i++)
		//{
		//	std::cout << aliveList1[i] << " ";
		//}
		//std::cout << std::endl;
		//
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counter_buffer->GetBufferID());
		Counter* counter = (Counter*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

		std::cout << "deadcount: " << std::endl;
		std::cout << counter->deadCount << std::endl;

		std::cout << "alivecount pre sim(should =update count): " << std::endl;
		std::cout << counter->aliveCount[m_preSimIndex] << std::endl; 

		std::cout << "alivecount post sim: " << std::endl; 
		std::cout << counter->aliveCount[m_postSimIndex] << std::endl;

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		 

	  
		}

		if (false)
		{glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particle_position_buffer->GetBufferID());
		float* positions = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

		std::cout << "position of first 9: " << std::endl;
		for (int i = 0; i < 4 * 10; i++)
		{
			std::cout << positions[i] << " "; 
		}
		std::cout << std::endl;

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		}


		//get the current active
		//get active particle count
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counter_buffer->GetBufferID());
		Counter* counter = (Counter*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		  
		m_currentAliveCount = counter->aliveCount[m_postSimIndex];
		//unmap
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


		//set bindings for material 
		auto _shader = this->rendererComponent->GetMaterial()->GetShader();
		_shader->Bind();

		m_aliveList_buffer[m_postSimIndex]->BindBase(0);
		m_particle_position_buffer->BindBase(1); 

		_shader->Unbind();

		  

		//post-sim process
	    //swap pre and post sim index, after sim;
		m_preSimIndex = (m_preSimIndex == 0 ? 1 : 0);
		m_postSimIndex = (m_postSimIndex == 0 ? 1 : 0);

	

	}


	 
}