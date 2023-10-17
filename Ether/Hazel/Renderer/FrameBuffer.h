#pragma once
#include "EtherPCH.h"  

#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/Material.h"

//,  it's a texture,  can be sampled in subsequent passes.


//a framebuffer is essentially a collection of buffers;
//it's the concept of "render target" ;
// the default framebuffer is the final result of the window output;
// 
//only usually many buffers works together;
//they are called "attachments"  to the framebuffer.
 
//built-in buffers are  
// 
//1. colorbuffer, depthbuffer,  and stencilbuffer 
//colorbuffer is the output of the fragment shader ; 
// 
//2. depth is the z-buffer tech;  also product of the pipeline,  used for depth test.
//3. stencil is is usually packed with depthbuffer.




//the workflow includes£º
//1. create and bind framebuffer
//2. specify the attachments, 
//3. check for completeness;



//specify attachments i.e. how to store the outputs of the pipeline ;

//there are two options:
//1. texture,  can be sampled in subsequent passes.
//2. renderbuffer,  can't be sampled,  for efficient copy. 
 
//eg: 
//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  TextureID, 0);
//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferID);


//to specify which colorbuffer(s) to draw to;
// we can specify multiple render targets MRT:
// glDrawBuffer(int size , unsigned int[] buffer_array);  
//
// eg:
// unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };//
// glDrawBuffers(3, attachments);
// 
// 
// for depth map,  we don't need colorbuffer,  we can disable it by.
// glDrawBuffer(GL_NONE);  
// 




//the purpose of custom framebuffer is the output textures; 
//the use is basically routined,  so we can just specify as "built-in" framebuffer.
//without detailed specifications every time.


//default:  we need the colorbuffer as a texture to read,  depth_stencil is not to be accessed.
//colorbuffer as a texture,   
//depth24+stencil8 as a renderbuffer ;   


 
//usecase: purely by me:
//shadowmap is not "normal", we only need depthbuffer,  no colorbuffer/disable drawbuffer. 
//the depthbuffer then is used as a texture in subsequent passes.
// 
//say we want to a post-processing effect, we will sample: not particularly color or depth, but the result of previous passes.
//so as the data, we instead store of one arrayOftextures,  and one renderbuffer, 
//that is regardless color or depth; 
//how to interpret the data is particular to the construction method/shader.
// 
// 
// 
// 
//to initialize with specfiic would be verbose ,
//the config should be only a few, why not just use different constructors?
// 
//FrameBufferSpec spec{ 1024, 1024, 
// 
// ColorBuffer & DepthBuffer spec};
// 
//FrameBuffer::Create(spec);





namespace Hazel {

	//temp disregard. 


    //temp disregard
	//struct FrameBufferSpec {
	//
	//	uint32_t Width = 0, Height = 0;  
	//	//TODO:
	//	//colorbuffer , type and spec
	//	//depthbuffer , type and spec  
	//};


	enum class RenderBufferFormat
	{
		None = 0, 
		// Depth/stencil, default support. 
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
		
		Screen,  //colorbuffer as texture2D, depth24+stencil8 as renderbuffer; 

		DepthMap,   //depth as texture, no colorbuffer 

		GBuffer, //basically default but with multiple colorbuffers
		 

		//PostProcess, //colorbuffer as texture, no depthbuffer;  might not be necessary to create new one£¿
	};


	 //enum class AttachmentType
     //{
     //	None = 0, 
	//	ColorAttachment = 1,
	//	DepthStencilAttachment = 2,
	//	DepthAttachment = 3,
     //	 
     //};



	class FrameBuffer
	{
	public:
		~FrameBuffer() = default;


		virtual void Bind() = 0;
		virtual void Unbind() = 0;


		virtual uint32_t GetFrameBufferID() = 0;
		virtual uint32_t GetColorBufferNum() = 0;


		//virtual uint32_t GetTextureBufferID(uint32_t index) = 0;

		//virtual void SetTextureBuffers(std::unordered_map<TextureType, Ref<Texture>> TextureBuffers) = 0;
		//virtual Ref<Texture> GetTextureBufferByType(TextureType type) = 0;
		//virtual std::unordered_map<TextureType, Ref<Texture>> GetTextureBuffers() = 0;
		//virtual uint32_t GetRenderBufferID()   = 0;


		//static Ref<FrameBuffer> CreateWithSpec(FrameBufferSpec& spec);
		//static Ref<FrameBuffer> Create(uint32_t width, uint32_t height, FrameBufferType type = FrameBufferType::Default,
		//	std::unordered_map<TextureType, Ref<Texture>> TextureBuffers = std::unordered_map<TextureType, Ref<Texture>>());
	 

		static Ref<FrameBuffer> Create(uint32_t width, uint32_t height, FrameBufferType type = FrameBufferType::Screen,
			uint32_t colorBufferNum = 0 );


		virtual void SetColorAttachmentTexture(Ref<Texture> texture, uint32_t slot) = 0;
		virtual void SetDepthAttachmentTexture(Ref<Texture> texture) = 0;


		virtual void CheckCompleteness() = 0;



	private:

		uint32_t m_FrameBufferID = 0; 
		uint32_t m_ColorBufferNum = 0;

		//FrameBufferType type = FrameBufferType::Default;
		//FrameBufferSpec m_Spec;

	 
		//std::unordered_map<TextureType, Ref<Texture>>  m_TextureBuffers;
		Scope<RenderBuffer> m_RenderBuffer ;   //renderbuffer


	};


}