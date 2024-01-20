#pragma once 
#include "RudyPCH.h"


//todo: _anisotropy support

//a data copy command. glTextureSubImage2D, but we don't find use case yet.
//

//textures has 2 sources:
//1. load from file,  use lib like stb_image,
//the info is given as a argument,  eg: width, height, format, etc
// 
//2. generate from code,  eg: as a render target 
//we will specify the info in a struct  to generate the texture

  
namespace Rudy {

	//data format on GPU; standardize to be "sized";
	enum class TextureInternalFormat
	{
		None = 0,

		//colors
		R8, 
		R32F,

		RG8,
		RG32F,

		RGB8, 
		RGB16F,
		RGB32F,

		RGBA8,  
		RGBA16F, //HDR
		RGBA32F, 
		 
		//other framebuffers buffers
		//for now standardize the depth to be 24 bit, to be compatible with default depth24_stencil8 
		DEPTH24_STENCIL8,  
		STENCIL_INDEX8, 
		DEPTH_COMPONENT24,
		DEPTH_COMPONENT32F,
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
//might used later;
//LinearMipmapNearest,
//NearestMipmapLinear,
//NearestMipmapNearest,
	};


	//better have default values;  at least width and height must be specified £º
	//eg:  TextureSpec spec{ 1024, 1024 };  will use default values for other members
	struct TextureSpec
	{
		uint32_t width = 0;
		uint32_t height = 0;
		TextureInternalFormat textureInternalFormat = TextureInternalFormat::RGB8;
		//data format in GPU is deduced from TextureInternalFormat here;
		bool generateMips = true;
		WrapMode wrapMode = WrapMode::Repeat;
		FilterMode minfilterMode = FilterMode::Linear;
		FilterMode magfilterMode = FilterMode::Linear;
	};


	//<<interface>>
	class Texture
	{
	public: 
		//if ever need compare, copy, move.
		//virtual bool operator==(const Texture& other) const = 0;


		//=====setters and getters
		//virtual void SetData(void* data, uint32_t size) = 0;
		virtual const TextureSpec& GetTextureSpec() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetID() const = 0; 
		virtual const std::string& GetPath() const = 0;
		 


		//GL utils
		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		//format , type is deduced from internal format
		virtual void SubData(void* data,  
			uint32_t width, uint32_t height, uint32_t xOffset=0, uint32_t yOffset=0) = 0;


		//helpers
		virtual bool IsLoaded() const = 0; 

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
	    //static Ref<Texture2D> CreateUsingData(const TextureSpec& specfication = TextureSpec(), void* data = nullptr);
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
		static Ref<TextureCube> CreateEmpty(const TextureSpec& specfication = TextureSpec());

		static Ref<TextureCube> LoadHDRI(const std::string& path); 
		static Ref<TextureCube> LoadImages(const std::vector<std::string>& paths); 


		virtual Ref<TextureCube> CreatePrefilteredEnvMap(Ref<TextureCube> envMap,
			ConvolutionType type = ConvolutionType::Specular,
			uint32_t mipLevels = 5) = 0;
	};

}



