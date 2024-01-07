
#include "RudyPCH.h"
#include "Scene.h"

namespace Rudy
{

    Ref<Scene> Scene::Create()
    {
		return CreateRef<Scene>();
	}

    void Scene::AddObject(Ref<Object> object)
    {
        objects.push_back(object);
        std::cout << "Scene: add object, current size: " << objects.size() << std::endl;
    }


    //until we determine a stable framework;
    void Scene::Render(Ref<Camera> camera)
    {
		 RD_CORE_ERROR("not implemented"); 

        //for (const auto& object : objects)
        //{ 
        //    if(object->isRenderable)
		//		object->Draw(camera); 
		//}
	}


}
       
