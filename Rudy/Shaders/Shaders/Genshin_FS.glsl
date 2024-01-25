#version 460 core 
//====MACRO==== 
#define PI 3.14159265359f   
//===output===
layout(location = 0)  out vec4 out_FragColor;
layout(location = 1)  out vec4 out_ViewPosition;
layout(location = 2)  out vec4 out_ViewNormal; 


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
uniform sampler2D u_NormalMap; 

uniform vec3 u_Albedo; 

uniform bool Use_u_AlbedoMap; 
uniform bool Use_u_NormalMap;  
 

//shadow map 
uniform sampler2D u_DepthTexture;  


//NPR:
uniform sampler2D u_ToonTexture;
uniform sampler2D u_FaceLightMap;

uniform vec3 u_litColor;
uniform vec3 u_shadowColor; 

uniform float u_diffuse_cutoff;

uniform bool u_face;
uniform vec3 u_faceForward ;
uniform vec3 u_faceRight ;

uniform float u_ambient_coeff;


uniform bool u_skipNormal;

uniform bool u_receive_shadow;


struct DirLight {
	vec3 color;
	vec3 direction;
	float intensity;
}; 

uniform DirLight u_DirLight;
uniform mat4 u_LightSpaceMatrix;  //for shadow map 

uniform float u_min_shadow_bias;
uniform float u_max_shadow_bias; 
 

uniform mat4 u_view;  

uniform vec3 u_CameraPos;


//---shadow map
//return 1 if in shadow, 0 otherwise;
float ShadowCalculation(vec4 WorldFragPos, float NdotL)
{
	// transform to NDC;

	vec4 lightSpaceFragPos = u_LightSpaceMatrix * WorldFragPos;
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;   //perspective division
	projCoords = projCoords * 0.5 + 0.5;
	//robust check eg: outof range, return 0 means not in shadow; 
	if (projCoords.z < 0.0 || projCoords.z > 1.0)
		return 0.0;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(u_DepthTexture, projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// check whether current frag pos is in shadow

	//bias = u_shadow_bias;
	//tolerate some bias 
	float bias = max(u_max_shadow_bias * (1.0 - NdotL), u_min_shadow_bias);   //todo: tune it better;  

	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	//simple 3x3 PCF for AA
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_DepthTexture, 0);
	for (int x = -1; x <= 1; ++x)
		for (int y = -1; y <= 1; ++y)
		{
			float sampleDepth = texture(u_DepthTexture, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > sampleDepth ? 1.0 : 0.0;
		}
	shadow /= 9.0;  //9 samples

	return shadow;
}




void main()
{
	 
	//===========preprocess================
	//this is copied from gbuffer shader; we leave it the way it is.  may be optimized later
	vec3 gWorldPosition;
	vec3 gWorldNormal;
	vec3 gWorldTangent; 

	vec3 gAlbedo;

	float specular = 1.0;   //gSpecular;


	gWorldPosition = fs_in.WorldPos;   

	gAlbedo = u_Albedo;
	if (Use_u_AlbedoMap)
		gAlbedo *= pow(texture(u_AlbedoMap, fs_in.TexCoords).rgb, vec3(2.1));  //gamma correction
 

	gWorldNormal = normalize(fs_in.WorldNormal);
	if (Use_u_NormalMap)
	{
		vec3 localNormal = texture(u_NormalMap, fs_in.TexCoords).rgb * 2.0 - 1.0; //from [0,1] to [-1,1] 
		gWorldNormal = normalize(fs_in.WorldTBN * localNormal);   //convert from tangent space to world space
	}
	 

	//new:   
	vec3 viewNormal = mat3(u_view) * gWorldNormal;
	if (!u_skipNormal)
	out_ViewNormal = vec4( viewNormal * 0.5f + 0.5f , 1.0f); //from [-1,1] to [0,1] ;
	else
	out_ViewNormal = vec4(vec3(0.0) * 0.5f + 0.5f, 1.0f);

	out_ViewPosition = u_view * vec4(gWorldPosition, 1.0f); 


	//gfs_in.WorldNormal = vec3( 1.0,0.0,0.0);  //test normal map
	gWorldNormal = gWorldNormal * 0.5 + 0.5; //from [-1,1] to [0,1] ;

	gWorldTangent = normalize(fs_in.WorldTangent) * 0.5 + 0.5; //from [-1,1] to [0,1] 




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

	 float test_sdf = texture(u_FaceLightMap, fs_in.TexCoords).r;
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
		float sdf = texture(u_FaceLightMap, uv_face).r;
	 
		// threshold = 0.2f;
		//same as LdotN,  0 means unlit, 1 means lit
		lit_level = smoothstep(threshold, threshold+0.05, sdf) * LdotN;
		// lit_level = step(threshold, sdf) * LdotN;

		//lit_level = sdf;
	 }
	

	 

	//======diff coloring 


	//note: this depend on the toon texture given;
	//only y component
	//vec3 toon_light = texture(u_ToonTexture, vec2(0.0f, 0.0f)).rgb;
	//toon_light = vec3(1.0f)  ;
	 vec3 toon_light = u_litColor;
	//vec3 toon_shadow = texture(u_ToonTexture, vec2(0.0f, 1.0f)).rgb;
	//toon_shadow = vec3(0.0f)  ;
	 vec3 toon_shadow = u_shadowColor;

	//the gradient effect
	//[parameter]
	float diffuse_cutoff = u_diffuse_cutoff;  //0.2f;
	float lit_ramp = smoothstep(0.0f, diffuse_cutoff,  lit_level);
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
        float ambient_coeff = u_ambient_coeff;   // 0.2f;
	    vec3 ambient_color = ambient_coeff * albedo;  
	 //



	 //shadow  

	 float shadow = 0.0;
	 if(u_receive_shadow)
	 shadow = ShadowCalculation(vec4(worldPos, 1.0), dot(normal, lightDir));
  

	 vec3 output_HDR = (1 - shadow) * (diff_color + 0.0000001* spec_color ) + ambient_color;

	 //tune gamma correction and tone mapping here;
	 vec3 output_LDR = output_HDR / (output_HDR + vec3(1.0f));
	 //gamma correction  

	 
	 //out_FragColor = vec4(output_LDR, 1.0f);
	 out_FragColor = vec4(output_HDR, 1.0f);
	 //out_FragColor = vec4(albedo, 1.0f);
	  //out_FragColor = vec4(vec3(lit_level), 1.0f);
	 //if(u_face)  out_FragColor = vec4(vec3(test_sdf), 1.0f);
	  //out_FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);


}