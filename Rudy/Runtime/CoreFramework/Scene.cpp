
#include "RudyPCH.h"
#include "Scene.h"

namespace Rudy
{

    SharedPtr<Scene> Scene::Create()
    {
		return CreateShared<Scene>();
	}

    void Scene::AddObject(SharedPtr<Object> object)
    {
        objects.push_back(object);
        std::cout << "Scene: add object, current size: " << objects.size() << std::endl;
    }


    //until we determine a stable framework;
    void Scene::Render(SharedPtr<Camera> camera)
    {
		 RD_CORE_ERROR("not implemented"); 

        //for (const auto& object : objects)
        //{ 
        //    if(object->isRenderable)
		//		object->Draw(camera); 
		//}
	}


}
       
