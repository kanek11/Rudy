#pragma once

#include "EtherPCH.h"


//textures has 2 sources:
//1. load from file,  use lib like stb_image,
//the info is given as a argument,  eg: width, height, format, etc
// 
//2. generate from code,  eg: as a render target 
//we will specify the info in a struct  to generate the texture

  
namespace Hazel {

	//me: relating high level enums;
	enum class TextureFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F,

		//framebuffer related
		DEPTH_COMPONENT,
	};



	enum class WrapMode
	{
		None = 0,
		Repeat, 
		Clamp,
		Mirror,
	};

	enum class FilterMode
	{
		None = 0,
		Nearest,
		Linear, 
	};


	//better have default values;  at least width and height must be specified £º
	//eg:  TextureSpec spec{ 1024, 1024 };  will use default values for other members
	struct TextureSpec
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		TextureFormat TextureFormat = TextureFormat::RGBA8;
		bool GenerateMips = true;
		WrapMode wrapMode = WrapMode::Repeat;
		FilterMode filterMode = FilterMode::Linear;
	};


	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureSpec& GetTextureSpec() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetTextureID() const = 0;

		virtual const std::string& GetPath() const = 0;


		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;
		
		static void SetFlipYOnLoad(bool flip) ;
		//virtual bool operator==(const Texture& other) const = 0;


	public:
		static bool s_FlipYOnLoad; 
	};


	class Texture2D : public Texture
	{
	public: 
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(const TextureSpec& specfication = TextureSpec());
	};


}



