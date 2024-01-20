#version 460 core 
//====MACRO==== 
#define PI 3.14159265359f   
//===output===
layout(location = 0)  out vec4 out_FragColor;
layout(location = 1)  out vec4 out_ViewNormal; 


//===input===
in VS_OUT
{
	vec3 WorldPos;
	vec2 TexCoords;
	vec3 WorldNormal;
	vec3 WorldTangent;
	mat3 WorldTBN;

} fs_in;



//Common:
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;

uniform vec3 u_Albedo;
uniform float u_Specular;
uniform float u_Metallic;
uniform float u_Roughness;

uniform bool Use_u_AlbedoMap;
uniform bool Use_u_SpecularMap;
uniform bool Use_u_NormalMap;
uniform bool Use_u_MetallicMap;
uniform bool Use_u_RoughnessMap;





//NPR:
uniform sampler2D u_ToonTexture;
uniform sampler2D u_FaceSDFTexture;

uniform vec3 u_litColor;
uniform vec3 u_shadowColor;


uniform bool u_face;
uniform vec3 u_faceForward ;
uniform vec3 u_faceRight ;

uniform vec3 u_testColor;


struct DirLight {
	vec3 color;
	vec3 direction;
	float intensity;
};

uniform DirLight u_DirLight;

uniform vec3 u_CameraPos; 

uniform mat4 u_view;


void main()
{


	//===========preprocess================
	//this is copied from gbuffer shader; we leave it the way it is.  may be optimized later
	vec3 gWorldPosition;
	vec3 gAlbedo;
	vec3 gWorldNormal;
	vec3 gWorldTangent;
	float gSpecular;
	float gMetallic;
	float gRoughness; 

	gWorldPosition = fs_in.WorldPos;   

	gAlbedo = u_Albedo;
	if (Use_u_AlbedoMap)
		gAlbedo *= pow(texture(u_AlbedoMap, fs_in.TexCoords).rgb, vec3(2.2)); //gamma correction

	gSpecular = u_Specular;
	if (Use_u_SpecularMap)
		gSpecular *= texture(u_SpecularMap, fs_in.TexCoords).r;

	gMetallic = u_Metallic;
	if (Use_u_MetallicMap)
		gMetallic *= texture(u_MetallicMap, fs_in.TexCoords).r;

	gRoughness = u_Roughness;
	if (Use_u_RoughnessMap)
		gRoughness *= texture(u_RoughnessMap, fs_in.TexCoords).r;

	gWorldNormal = normalize(fs_in.WorldNormal);
	if (Use_u_NormalMap)
	{
		vec3 localNormal = texture(u_NormalMap, fs_in.TexCoords).rgb * 2.0 - 1.0; //from [0,1] to [-1,1] 
		gWorldNormal = normalize(fs_in.WorldTBN * localNormal);   //convert from tangent space to world space
	}


	//gfs_in.WorldNormal = vec3( 1.0,0.0,0.0);  //test normal map
	gWorldNormal = gWorldNormal * 0.5 + 0.5; //from [-1,1] to [0,1] ;

	gWorldTangent = normalize(fs_in.WorldTangent) * 0.5 + 0.5; //from [-1,1] to [0,1] 


	//new:
	out_ViewNormal = u_view * vec4(gWorldNormal, 1.0f);

	//=====sample the gbuffer
	//vec3 worldPos =    texture(gWorldPosition, fs_in.TexCoords).xyz;
	//vec3 albedo =      texture(gAlbedo, fs_in.TexCoords).xyz; 
	//float specular =   texture(gSpecular, fs_in.TexCoords).r;  
	//vec3 worldNormal = texture(gWorldNormal, fs_in.TexCoords).xyz;
	//if (length(worldNormal) < 0.2) discard;  //discard background
	//vec3 normal = normalize(worldNormal * 2.0 - 1.0);

	//new:
	 vec3 worldPos  = gWorldPosition; 
	 vec3 albedo    = gAlbedo; 
	 float specular = gSpecular; 
 

	 vec3 normal = normalize(gWorldNormal * 2.0 - 1.0);


	//==============TOON SHADING================ 

 
	//common 
	 vec3 lightDir = -normalize(u_DirLight.direction);
	 vec3 viewDir = normalize(u_CameraPos - worldPos);
	 vec3 halfwayDir = normalize(lightDir + viewDir);


	 vec3 radiance = u_DirLight.color * u_DirLight.intensity;
	
	

	 //========diffuse component========
 
	 
	 //diffuse node [0,1]
	

	//shadowmap later
	 float lit_level = max( dot(lightDir, normal), 0.0f);


	//========face shadow handling========

	 float test_sdf = texture(u_FaceSDFTexture, fs_in.TexCoords).r;
	//if (u_face)
	 if(u_face)
	 {
		//if not face, use the normal
		normal = u_faceForward;
		float LdotN = max(dot(lightDir, normal), 0.0f);

		//override the trivial LdotN for face shadow

        //angle with right;
		float LdotR = dot(lightDir, u_faceRight) ; 

		//flip the uv if the light is on the left side 
		vec2 uv_face = vec2(0.0f, 0.0f);
		uv_face.x = LdotR < 0.0f ? fs_in.TexCoords.x : 1.0f - fs_in.TexCoords.x;
		uv_face.y = fs_in.TexCoords.y;

		//the threshold of the face shadow, based on angle with right
		//this map the angle to [1,-1] range
		// 0-pi/2, 1 to 0  to cap,   pi/2 - pi, 0 to -1 as no effect, flip it.
		float threshold = acos(LdotR) / PI * 2.0f - 1.0f;

		//flip it if the light is on the left side
		threshold = LdotR < 0.0f ? threshold: -threshold ;

		//step(threshold, sdf) returns 0 if threshold > sdf
		//uv_face = fs_in.TexCoords;
		float sdf = texture(u_FaceSDFTexture, uv_face).r;
	 
		// threshold = 0.2f;
		//same as LdotN,  0 means unlit, 1 means lit
		 lit_level = step(threshold, sdf) * LdotN;

		//lit_level = sdf;
	 }
	

	 

	//======diff coloring 


	//note: this depend on the toon texture given;
	//only y component
	vec3 toon_light = texture(u_ToonTexture, vec2(0.0f, 0.0f)).rgb;
	//toon_light = vec3(1.0f)  ;
	vec3 toon_shadow = texture(u_ToonTexture, vec2(0.0f, 1.0f)).rgb;
	//toon_shadow = vec3(0.0f)  ;

	//the gradient effect
	//[parameter]
	float diff_stop = 0.1f;
	float lit_ramp = smoothstep(0.0f,  diff_stop,  lit_level);
	vec3 diff_toon = mix(toon_shadow, toon_light, lit_ramp);

	vec3 diff_color = diff_toon * radiance * albedo;
	//diff_color = lit_level * radiance * albedo;


	//========specular component======== 
	//simple blinn phong will work
	//
		float NdotH = max(dot(normal, halfwayDir), 0.0f);
	
		//[parameter]  glossiness
		float spec_coeff = 32 * specular;
        float spec_strength = pow(NdotH, spec_coeff);

		//[parameter] hightlight threshold
		spec_strength = smoothstep(0.005f, 0.1f, spec_strength);

		vec3 spec_color = vec3(1.0f) * spec_strength; //white color

	//

	//ambient
	//
		//[parameter] ambient coeff
		float ambient_coeff = 0.2f;
		vec3 ambient_color = ambient_coeff * albedo; 
		
	//


	 vec3 output_HDR = diff_color + 0 *spec_color + ambient_color;

	 //tune gamma correction and tone mapping here;
	 vec3 output_LDR = output_HDR / (output_HDR + vec3(1.0f));
	 //gamma correction  

	  out_FragColor = vec4(output_LDR, 1.0f);
	  //out_FragColor = vec4(vec3(lit_level), 1.0f);
	  // if(u_face)  out_FragColor = vec4(vec3(test_sdf), 1.0f);
	//out_FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);


}