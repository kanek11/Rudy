#pragma once
#include "EtherPCH.h"  

#include "Hazel/Renderer/Texture.h"

//a framebuffer is specfified by:

//size,
//attachments:
//colorbuffer , could be multiple for deferred rendering. MRT etc.
//depthbuffer  
//stencilbuffer  

//buffer type:
//texture , can be sampled in shaders , used by subsequent passes

//renderbuffer ,    can't be sampled , for efficient copy.


//there is a bunch of specifications to create a custom framebuffer,
//but in most cases, the default framebuffer is enough.
//colorbuffer as a texture,  
//depth24+stencil8 as a renderbuffer ;  it's kinda confusing to me that they are usually packed together. 

//the stencil buffer is used for stencil test,  we config it as gl functions, hardly directly access it. 
//so usually not explicity specified.  maybe include with depthbuffer by default.

 

//the texture info of colorbuffer is useful,
//the renderbuffer  is hardly accessed.


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


	enum class AttachmentType
	{
		None = 0,

		Texture2D,
		RenderBuffer,
	};


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

		static Scope<RenderBuffer> Create(RenderBufferFormat format, uint32_t width, uint32_t height);
	
	private:
		uint32_t m_Width = 0, m_Height = 0;
		RenderBufferFormat m_Format = RenderBufferFormat::DEPTH24STENCIL8;

		uint32_t m_RenderBufferID = 0;
	
	};
	 

	struct FrameBufferSpec {

		uint32_t Width = 0, Height = 0; 
		
		//TODO:
		//colorbuffer , type and spec
		//depthbuffer , type and spec 

	};

	//my idea, might be wrong.
	enum class FrameBufferType
	{
		Default = 0, //colorbuffer as texture, depth24+stencil8 as renderbuffer

		DepthMap,   //depth as texture, no colorbuffer 
	  
		//PostProcess, //colorbuffer as texture, no depthbuffer;  might not be necessary to create new one£¿

		Deferred, //basically default but with multiple colorbuffers
	};



	class FrameBuffer
	{
	public:
		~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;


		virtual uint32_t GetFrameBufferID() = 0;
		virtual uint32_t GetTextureBufferID(uint32_t index) = 0;
		virtual uint32_t GetRenderBufferID()   = 0;


		static Ref<FrameBuffer> CreateWithSpec(FrameBufferSpec& spec);
		static Ref<FrameBuffer> CreateWithSize(uint32_t width, uint32_t height, FrameBufferType type = FrameBufferType::Default);
		//size is the very first thing to be specified, the rest if defualt.
 


	private:

		uint32_t m_FrameBufferID = 0;
		FrameBufferSpec m_Spec;

		std::vector<Ref<Texture2D>> m_TextureBuffers; //colorbuffer
		Scope<RenderBuffer> m_RenderBuffer ;   //renderbuffer


	};


}