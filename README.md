# a GPU-based renderer
This is a rendering engine in development, primarily for educational purposes.
Source codes of the following demos can be found under the 'Apps' folder.

## Prerequisite
This is a visual studio project runs on Windows, and relying on NVIDIA graphics cards for atomic float operations.
Resources and dependencies are included in the project.

## Major implementations.　
 実装した項目
* Deferred rendering
* Physically based rendering
* Image-based lighting  
* Skeletal animation
* Post-processings(Bloom,Outline,SSAO,SSR)
* NPR pipeline
* GPU particles
* GPU cloth
* Eulerian fluid


## Snapshots
### PBR
Physically based Material with gbuffer visualization.
The buffers in the following are relatively： <br>
* Metallic , Roughness, ViewDepth, ShadowMap <br>
* Position , WorldNormal, gWorldTangent, Albedo <br>
<img src="images/PBR.png" width="800">

### NPR
A basic non-photorealistic rendering pipeline.
* cel shading
* face shadow by lightmap
* Postprocessings(Bloom,Outline)
<img src="images/NPR.png" width="800">

* flow chart showing the pipeline data flow. (visualization using Blender shader graph)
<img src="images/NPR_pipeline.png" width="800">

### GPU particles
* lifetime management
* camera-facing sprites
<img src="images/particles_gif.gif" width="800">

### GPU cloth
* by PBD(position-based dynamics) 
<img src="images/cloth.gif" width="800">

### Fluid simulation
* by Eulerian method
<img src="images/fluid_gif.gif" width="800">

## Roadmap
これからやりたいこと
* Build methods & Documentation (after the framework is stablized)
* Temporal techniques (TAA, temporal denoising)
* MMD animations with physics
* Particle sorting/Collision handling
* fluid simulation by FLIP
* SoftBody simulation 
* Volume rendering
* Subsurface scattering

## Dependencies
* [spdLog](https://github.com/gabime/spdlog)
* [Glad](https://glad.dav1d.de/) 
* [GLFW](https://www.glfw.org/) 
* [GLM](https://github.com/g-truc/glm)
* [stb](https://github.com/nothings/stb)
* [assimp](https://github.com/assimp/assimp)
* [Dear ImGui](https://github.com/ocornut/imgui)
