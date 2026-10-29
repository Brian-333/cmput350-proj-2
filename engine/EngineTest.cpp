#include <cassert>

#include "EntityManager.h"
#include "Component.h"

void TestEntityManager() {
    ECSEngine::EntityManager<int, float, char> entityManager;
    ECSEngine::EntityID entity = entityManager.CreateEntity("entity1");
    ECSEngine::ComponentStorage<float>& floatComponentStorage = entityManager.GetComponentStorage<float>();

    // Check that the float component storage is empty
    assert(floatComponentStorage.size() == 0);

    // Add a component to the entity
    entityManager.AddComponent(entity, 1);

    // Check that the component was added
    assert(entityManager.HasComponent<int>(entity));
    assert(entityManager.GetComponent<int>(entity) == 1);

    // Add a float component to the entity
    entityManager.AddComponent(entity, 2.0f);

    // Check that the components were added
    assert(entityManager.HasComponent<int>(entity));
    assert(entityManager.HasComponent<float>(entity));
    assert(!entityManager.HasComponent<char>(entity));

    // Remove the int component
    entityManager.RemoveComponent<int>(entity);

    // Check that the component was removed
    assert(!entityManager.HasComponent<int>(entity));

    // Check that the float component was not removed
    assert(entityManager.HasComponent<float>(entity));
    assert(entityManager.GetComponent<float>(entity) == 2.0f);
    assert(floatComponentStorage.size() == 1);
    assert(floatComponentStorage[0] == 2.0f);

    // Remove the float component
    entityManager.RemoveComponent<float>(entity);

    // Check that the component was removed
    assert(!entityManager.HasComponent<float>(entity));
    assert(floatComponentStorage.size() == 0);

    // Remove the entity
    entityManager.AddComponent(entity, 3);
    entityManager.AddComponent(entity, 4.0f);
    entityManager.AddComponent(entity, 'c');
    entityManager.RemoveEntity(entity);

    // Check that the entity was removed
    assert(entityManager.GetComponentStorage<int>().size() == 0);
    assert(entityManager.GetComponentStorage<float>().size() == 0);
    assert(entityManager.GetComponentStorage<char>().size() == 0);

    std::cout << "EntityManager test passed" << std::endl;
}


int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    TestEntityManager();

    return 0;
}