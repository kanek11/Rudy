
#include "EtherPCH.h"
#include "Scene.h"

namespace Hazel
{

    void Scene::Add(Ref<Object> object)
    {
        Objects.push_back(object);
        std::cout << "Scene: add object, current size: " << Objects.size() << std::endl;
    }


}
       
