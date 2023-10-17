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
		R32F,

		RG8,
		RG32F,

		RGB8, 
		RGB32F,

		RGBA8,
		RGBA32F,

		//system built-in implementation:
		DEPTH_COMPONENT,
	};
	 

	enum class WrapMode
	{
		None = 0,
		Repeat, 
		ClampToEdge,
		ClampToBorder,
		Mirror,
	};

	enum class FilterMode
	{
		None = 0,
		Nearest,
		Linear, 
		LinearMipmapLinear,
	};


	//better have default values;  at least width and height must be specified £º
	//eg:  TextureSpec spec{ 1024, 1024 };  will use default values for other members
	struct TextureSpec
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		TextureFormat TextureFormat = TextureFormat::RGB8;
		//data format in GPU is deduced from TextureFormat here;
		bool GenerateMips = true;
		WrapMode wrapMode = WrapMode::Repeat;
		FilterMode minfilterMode = FilterMode::Linear;
		FilterMode magfilterMode = FilterMode::Linear;
	};


	class Texture
	{
	public:
	    ~Texture() = default;


		//=====setters and getters
		//virtual void SetData(void* data, uint32_t size) = 0;
		virtual const TextureSpec& GetTextureSpec() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetTextureID() const = 0; 
		virtual const std::string& GetPath() const = 0;
		//=====setters and getters


		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0; 

		//virtual bool operator==(const Texture& other) const = 0;
	 
		//====static method for state.
		static void SetFlipYOnLoad(bool flip) ;  //for image loader.


	public:
		static bool s_FlipYOnLoad; 
	};


	class Texture2D : public Texture
	{
	public: 
		static Ref<Texture2D> LoadFile(const std::string& path, bool isHDRI = false);
		static Ref<Texture2D> CreateEmpty(const TextureSpec& specfication = TextureSpec());
	};



	enum class ConvolutionType
	{
		None = 0,
		Specular, 
		Diffuse,
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> LoadHDRI(const std::string& path); 
		static Ref<TextureCube> LoadImages(const std::vector<std::string>& paths); 


		static Ref<TextureCube> CreateEmpty(const TextureSpec& specfication = TextureSpec());


		virtual Ref<TextureCube> CreatePrefilteredEnvMap(Ref<TextureCube> envMap,
			ConvolutionType type = ConvolutionType::Specular,
			uint32_t mipLevels = 5) = 0;
	};

}



