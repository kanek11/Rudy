#pragma once
#include "RudyPCH.h"  

#include "Rudy/Renderer/Texture.h"
#include "Rudy/Renderer/Material.h"

 

namespace Rudy {

	//temp disregard. 
 

	enum class RenderBufferFormat
	{
		None = 0, 
		// Depth/stencil, default support. 
		STENCIL_INDEX8 ,
		DEPTH_COMPONENT24,
		DEPTH24STENCIL8, 
	};



	//keep it simple. 
	class RenderBuffer
	{
    public:
		virtual ~RenderBuffer() = default; 

		virtual uint32_t GetRenderBufferID() = 0;

		static Scope<RenderBuffer> Create(uint32_t width, uint32_t height, RenderBufferFormat format = RenderBufferFormat::DEPTH24STENCIL8);
	
	private:
		uint32_t m_Width = 0, m_Height = 0;
		RenderBufferFormat m_Format ;

		uint32_t m_RenderBufferID = 0;
	
	};
	 



	//my idea, might be wrong.
	enum class FrameBufferType
	{
		None = 0, 
		
		Regular,  //colorbuffer as texture2D, depth24+stencil8 as renderbuffer; 

		DepthTexture,   //depth as texture, no colorbuffer 

		GBuffer, //basically default but with multiple colorbuffers
 	};


	 //enum class AttachmentType
     //{
     //	None = 0, 
	//	ColorAttachment = 1,
	//	DepthStencilAttachment = 2,
	//	DepthAttachment = 3,
     //	 
     //};


	//<<interface>>
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default; 

		virtual void Bind() = 0;
		virtual void Unbind() = 0; 

		virtual uint32_t GetFrameBufferID() = 0;  

		static Ref<FrameBuffer> Create(
			std::string name,
			uint32_t width, uint32_t height, 
			FrameBufferType type = FrameBufferType::Regular); 

		virtual void SetColorTexture(TexType type, Ref<Texture> texture, uint32_t slot) = 0;
		virtual void SetDepthTexture(Ref<Texture> texture) = 0;

		virtual Ref<Texture> GetTextureByType(TexType type) = 0; 
		
		virtual void SetDrawBuffers() = 0;
		virtual void CheckCompleteness() = 0; 
		virtual void FinishSetup() = 0;

	};


}