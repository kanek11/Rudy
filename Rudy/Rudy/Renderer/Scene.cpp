
#include "RudyPCH.h"
#include "Scene.h"

namespace Rudy
{

    Ref<Scene> Scene::Create()
    {
		return CreateRef<Scene>();
	}

    void Scene::AddRenderableObject(Ref<RenderableObject> object)
    {
        renderObjects.push_back(object);
        std::cout << "Scene: add object, current size: " << renderObjects.size() << std::endl;
    }


    void Scene::Render(Ref<Camera> camera)
    {
		//RD_CORE_INFO("Scene::Draw"); 

        for (const auto& object : renderObjects)
        { 
			object->Draw(camera);
		}
	}


}
       
