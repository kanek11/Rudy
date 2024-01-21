#include "RudyPCH.h"   

#include "Bloom.h"

namespace  Rudy
{

	Bloom::Bloom(
		uint32_t m_width, uint32_t m_height,
		std::unordered_map< TexType, Ref<Texture>>& m_BloomInputs,
		std::unordered_map< TexType, Ref<Texture>>& m_BloomOutputs)
		:m_width(m_width), m_height(m_height),
		m_BloomInputs(m_BloomInputs), m_BloomOutputs(m_BloomOutputs)
	{
		Init();
	}


	//progressive upscale formula
	//E' = blur(E)
	//D' = blur(D)+ blur(E')  
	//blur and downsample at the same time, so it' like
	//D' += blur(E')  without conflict 
	void Bloom::Init()
	{
		 
		//implementation using two pass gaussian blur
		//horizontal be intermediate ,  vertical be final
		//horizontal start with thresholded image;
		//I tried to use mipmaps , but not able to visualize it, just use texture array instead


		//simulate mipchain by texture array
		//spec: 6 mipmaps , level 0 with original size.
		//1-5 levels is blurred;
		glm::uvec2 mip_res = glm::uvec2(m_width, m_height); //accept unsigned int
		for (int i = 0; i < nMips; i++)
		{
			RT_Horizonal.push_back(Texture2D::CreateEmpty(TextureSpec{
					mip_res.x, mip_res.y, TextureInternalFormat::RGBA32F,
					false, WrapMode::ClampToEdge, 
					FilterMode::Linear ,
					FilterMode::Linear
			}));

			RT_Vertical.push_back(Texture2D::CreateEmpty(TextureSpec{
					mip_res.x, mip_res.y, TextureInternalFormat::RGBA32F,
					false, WrapMode::ClampToEdge, 
					FilterMode::Linear ,
					FilterMode::Linear
			}));

			mip_res /= 2u;
		}
		 

		//glTextureStorage2D(this->RT_Vertical->GetID(), 5, GL_RGBA32F, m_width, m_height);
	 

		//input texture has no mipmaps, so just reference it is not enough,
		//either  create a new texture and copy it
		//or generate mipmaps for input texture 
		this->inputTexture = m_BloomInputs[TexType::ScreenTexture];
		//glGenerateTextureMipmap( this->inputTexture->GetID() );

		//this->m_BloomOutputs[TexType::ScreenTexture] = RT_Vertical; 
		this->m_BloomOutputs[TexType::ScreenTexture] = RT_Vertical[0];
		//this->m_BloomOutputs[TexType::ScreenTexture] = RT_Horizonal[1];



		//shaders
		this->thresholdShader = Shader::CreateComputeShader(
			"threshold shader", "Shaders/PostProcess/threshold_CS.glsl");
		

		this->downsampleShader = Shader::CreateComputeShader(
			 "downsample shader", "Shaders/PostProcess/downSample_CS.glsl"); 


		this->upsampleShader = Shader::CreateComputeShader(
			"upsample shader", "Shaders/PostProcess/upSample_CS.glsl");

	}


	void Bloom::Render(Ref<Camera>) 
	{

		//threshold
		{
			this->thresholdShader->Bind();

			this->thresholdShader->SetFloat("u_threshold", this->threshold); 
			
			glBindImageTexture(0, inputTexture->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		 
			glBindImageTexture(1, RT_Vertical[0]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			//dispatch
			glDispatchCompute(glm::ceil(float(m_width) / 8), glm::ceil(float(m_height) / 8), 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			this->thresholdShader->Unbind(); 

		}
 

		 
		if(true)
		{ 
		//downsample
		//input: readonly sampler2D ; original size binary texture
		//output:  writeonly image2D £» vertical blur [mip]
	    //the two works like pingpong.

		if(downsampleShader == nullptr)
		{
			RD_CORE_ERROR("Shader is null");
			return;  
		}
		
		downsampleShader->Bind();
		 
		glm::uvec2 mip_res = glm::uvec2(m_width, m_height) ; //accept unsigned int

		//write to mip+1 ; 0-4 write 1-5
		for (int mip = 0; mip < nMips-1; mip++)
		{ 
			//for robust, use power of 2  instead of scale 
			mip_res.x = glm::max(1.0, glm::floor(float(m_width)  / glm::pow(2.0, mip+1)));
			mip_res.y = glm::max(1.0, glm::floor(float(m_height) / glm::pow(2.0, mip+1)));
			 
			//std::cout << mip+1 << std::endl;
			//set uniforms
			this->downsampleShader->SetInt("u_kernel_radius", kernelRadiusArray[mip+1]); 

			//horizontal=====	 
		    this->downsampleShader->SetVec2("u_direction", glm::vec2(1,0));
		
			//input  
			RT_Vertical[mip]->Bind(0);
		 
			//output			   
			glBindImageTexture(0,  RT_Horizonal[mip+1]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		
			//dispatch
			glDispatchCompute(glm::ceil(float(mip_res.x) / 8), glm::ceil(float(mip_res.y) / 8), 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); 


			//vertical=======
			//set uniforms
			this->downsampleShader->SetVec2("u_direction", glm::vec2(0,1));
			 
			//input
			RT_Horizonal[mip+1]->Bind(0);
			//output			  
			glBindImageTexture(0, RT_Vertical[mip+1]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			//dispatch
			glDispatchCompute(glm::ceil(float(mip_res.x) / 8), glm::ceil(float(mip_res.y) / 8), 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		}


		downsampleShader->Unbind();


		upsampleShader->Bind();
		//upsample 
		//from high to low, until level 0 of vertical blur
	    ///mip res remains its state at level 5;  
		//5-1 write 4-0
		for (int mip = nMips - 1; mip >=1 ; mip--)
		{
			mip_res.x = glm::max(1.0, glm::floor(float(m_width) /  glm::pow(2.0, mip - 1)));
			mip_res.y = glm::max(1.0, glm::floor(float(m_height) / glm::pow(2.0, mip - 1)));

			//set uniforms
			// std::cout << mip-1 << std::endl;
			this->upsampleShader->SetInt("u_kernel_radius", kernelRadiusArray[mip]);


			//horizontal=====	 
			this->upsampleShader->SetVec2("u_direction", glm::vec2(1, 0));
			//set output, write to mip+1
			//input 
			RT_Vertical[mip]->Bind(0);
			//output			   
			glBindImageTexture(0, RT_Horizonal[mip-1]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

			//dispatch
			glDispatchCompute(glm::ceil(float(mip_res.x) / 8), glm::ceil(float(mip_res.y) / 8), 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  

			//vertical=======
			//set uniforms
			this->upsampleShader->SetVec2("u_direction", glm::vec2(0, 1));

			 this->upsampleShader->SetFloat("u_bloom_strength", bloom_strength);
			 this->upsampleShader->SetFloat("u_bloom_radius", bloom_radius);
			 this->upsampleShader->SetFloat("u_factor", bloomFactors[mip]);

			//input
			RT_Horizonal[mip-1]->Bind(0);
			//output			  
			glBindImageTexture(0, RT_Vertical[mip-1]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

			//dispatch
			glDispatchCompute(glm::ceil(float(mip_res.x) / 8), glm::ceil(float(mip_res.y) / 8), 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		 

		}
		 


		upsampleShader->Unbind();




		}






	}







}