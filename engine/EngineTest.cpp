#include <cassert>

#include "EntityManager.h"
#include "Component.h"
#include "SoundManager.h"
#include "SpriteManager.h"

/**
 * Test adding, getting, and removing components and entities in the entity manager.
 */
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

/**
 * Test registering and playing a sound, and re-registering it with a new volume.
 */
void TestSoundManager() {
    ECSEngine::SoundManager soundManager;
    soundManager.RegisterSound("../assets/sfx_gem.ogg", "gem");
    sf::Sound& currentSound = soundManager.GetSound("gem");
    currentSound.play();
    while (currentSound.getStatus() == sf::Sound::Status::Playing) {
        sf::sleep(sf::milliseconds(10));
    }
    assert(currentSound.getVolume() == 100);
    
    soundManager.RegisterSound("../assets/sfx_gem.ogg", "gem", 50);
    currentSound = soundManager.GetSound("gem");
    currentSound.play();
    while (currentSound.getStatus() == sf::Sound::Status::Playing) {
        sf::sleep(sf::milliseconds(10));
    }
    assert(currentSound.getVolume() == 50);
    std::cout << "SoundManager test passed" << std::endl;
}

// This test is written with help of Cursor AI.
/**
 * Test that registered sprites get separate IDs and keep their source rectangles.
 */
void TestSpriteManager() {
    ECSEngine::SpriteManager spriteManager;
    ECSEngine::Rect rect{{0, 0}, 16, 16};
    ECSEngine::SpriteID id0 = spriteManager.RegisterTexture(
        "../assets/spritesheet-tiles-default.png", rect);
    ECSEngine::SpriteID id1 = spriteManager.RegisterTexture(
        "../assets/spritesheet-tiles-default.png", {{16, 0}, 16, 16});
    assert(id0 == 0);
    assert(id1 == 1);
    assert(&spriteManager.GetSprite(id0) != &spriteManager.GetSprite(id1));
    assert(spriteManager.GetSprite(id0).getTextureRect().size.x == 16);
    std::cout << "SpriteManager test passed" << std::endl;
}


/**
 * Run all engine tests.
 * @param argc The number of command line arguments (unused).
 * @param argv The command line arguments (unused).
 * @return 0 if every test passes.
 */
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    TestEntityManager();
    TestSoundManager();
    TestSpriteManager();
    return 0;
}