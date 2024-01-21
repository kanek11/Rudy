#pragma once

#include "RudyPCH.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"

#include "Rudy/Renderer/Renderer.h"



namespace Rudy
{

     
	class WorldGrid {

	public: 
        ~WorldGrid() = default;
        WorldGrid(int size);

		void CreateGeometry(int size);
		void Draw(); 

		uint32_t gridVAO, gridVBO;

		//address geometry differently
		std::vector<float> gridVertices; // 10 vertical and 10 horizontal lines, each requiring 4 vertices

		//Transform  Transform;

		Ref<Material>  material;

	};



    

     


 


    //x red , y green, z blue ; 
    class Navigation {

    public: 
        ~Navigation() = default;
        Navigation();

        void CreateGeometry();
        void Draw();

        uint32_t vao, vbo; 
        std::vector<float> vertices; // 10 vertical and 10 horizontal lines, each requiring 4 vertices
         
        Ref<Material>  material;

    };




}

 