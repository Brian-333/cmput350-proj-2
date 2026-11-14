#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "CameraSystem.h"
#include "CollisionComponent.h"
#include "CollisionLateSystem.h"
#include "CollisionSystem.h"
#include "ECSEngine.h"
#include "EntityManager.h"
#include "GravitySystem.h"
#include "InputComponent.h"
#include "InputSystem.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "MovementSystem.h"
#include "ScoreComponent.h"
#include "ScoreSystem.h"
#include "SpawnComponent.h"
#include "SpawnSystem.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"

std::string gResourcePath = "../assets/";
#define COMPONENTS                                                                             \
    ECSEngine::LocationComponent, ECSEngine::MovementComponent, ECSEngine::CollisionComponent, \
        ECSEngine::SpriteComponent, ECSEngine::SpawnComponent, ECSEngine::CameraComponent,     \
        ECSEngine::CameraFollower, ECSEngine::InputComponent, ECSEngine::GravityComponent,     \
        ECSEngine::CameraShake, ECSEngine::ScoreComponent, ECSEngine::RespawnComponent
using Engine = ECSEngine::ECSEngine<COMPONENTS>;

struct mapTile {
    std::string symbol;
    std::string path;
    ECSEngine::Rect loc;
    ECSEngine::Rect box;
};

void LoadMap(Engine& engine, const std::string& path, const std::string& file, const ECSEngine::SpriteID starId) {
    auto &em = engine.GetEntityManager();
    auto &sm = engine.GetSpriteManager();

    std::ifstream loadedMap(path + file);

    if (!loadedMap.is_open()) {
        std::cout << "Error loading map" << std::endl;
        return;
    }

    int mapType;
    std::unordered_map<char, mapTile> tiles;
    std::unordered_map<char, ECSEngine::SpriteID> spriteIds;

    // Getting first line (the mapType)
    std::string line; 
    std::getline(loadedMap, line);
    std::istringstream iss(line);
    std::string skip;
    iss >> skip >> mapType;

    // Getting the tiles
    while (std::getline(loadedMap, line)) {
        iss = std::istringstream(line);

        std::string symbol, filePath;
        mapTile tile;

        iss >> symbol;

        if (symbol == "map") { break ;} // Have all the tiles collected

        iss >> filePath;

        float x, y, w, h;

        iss >> x >> y >> w >> h;

        tile.loc.topLeft = {x, y};
        tile.loc.width = w;
        tile.loc.height = h;

        if (mapType == 2) {
            float x2, y2, w2, h2;
            iss >> x2 >> y2 >> w2 >> h2;

            tile.box.topLeft = {x2, y2};
            tile.box.width = w2;
            tile.box.height = h2;
        }

        tile.path = path + filePath;
        tile.symbol = symbol;

        tiles[symbol[0]] = tile;
        spriteIds[symbol[0]] = sm.RegisterTexture(tile.path, tile.loc);
    }

    // Set up map origin
    float oLeft, oTop, tWidth, tHeight;
    int rows, cols;
    iss >> skip >> oLeft >> oTop >> skip >> tWidth >> tHeight >> skip >> cols >> rows;

    // Register entities
    for (int row = 0; row < rows; ++row) {
        std::getline(loadedMap, line);
        for (int col = 0; col < cols; ++col) {
            char symbol = line[col];

            if (symbol == '.' || spriteIds.count(symbol) == 0) { continue; }    // nothing here

            ECSEngine::Point2D pos = {oLeft + col * tWidth, oTop + row * tHeight};

            ECSEngine::EntityID id = em.CreateEntity("tile");
            em.AddComponent(id, ECSEngine::LocationComponent{pos}); 
            em.AddComponent(id, ECSEngine::SpriteComponent{
                spriteIds[symbol], ECSEngine::Rect{{0, 0}, tWidth, tHeight}, true
            });

            if (mapType == 2) {
                ECSEngine::CollisionComponent c{};
                c.staticEntity = 1;
                c.collisionBox = tiles[symbol].box;
                c.previousCollisionBox = c.collisionBox;
                c.previousCollisionBox.topLeft += pos;

                if (symbol == 'x') {
                    c.doesDamage = 1;
                }

                em.AddComponent(id, c);
            }

            if (symbol == 'S') {    // spawner
                em.AddComponent(id, ECSEngine::SpawnComponent{
                    id, "star", starId, 60, 120, 20 
                });
            }   
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc >= 3 && argv[1] == std::string("-path")) {
        gResourcePath = argv[2];
    }
    std::cout << "Usage: " << argv[0] << " [-path resource_path]\n";
    std::cout << "Using resource path: " << gResourcePath << "\n";

    // Engine test code
    Engine engine(1024, 768, "Test Engine");

    // Load sounds
    auto& soundM = engine.GetSoundManager();
    soundM.RegisterSound(gResourcePath + "sfx_jump.ogg", "playerJump", 50.f);
    soundM.RegisterSound(gResourcePath + "sfx_gem.ogg", "starCollect", 50.f);
    soundM.RegisterSound(gResourcePath + "footstep_grass_003.ogg", "playerLand", 50.f);
    soundM.RegisterSound(gResourcePath + "footstep_snow_001.ogg", "wallSlide", 50.f);

    auto& em = engine.GetEntityManager();
    auto& spriteM = engine.GetSpriteManager();
    auto& sm = engine.GetSystemManager();
    auto starSpriteId = spriteM.RegisterTexture(gResourcePath + "spritesheet-tiles-default.png", {{640, 320}, 64, 64});

    // Load maps
    LoadMap(engine, gResourcePath, "sky.map", starSpriteId);
    LoadMap(engine, gResourcePath, "world.map", starSpriteId);

    // Place player
    ECSEngine::Point2D playerSpawn({832, 512});

    auto playerSprite = spriteM.RegisterTexture(
        gResourcePath + "spritesheet-characters-default.png", {{0, 0}, 128, 128}
    ); 

    auto playerId = em.CreateEntity("player");
    em.AddComponent(playerId, ECSEngine::LocationComponent{playerSpawn});
    em.AddComponent(playerId, ECSEngine::MovementComponent{{0, 0}});
    em.AddComponent(playerId, ECSEngine::GravityComponent{{0.f, 0.5f}, 0});

    ECSEngine::CollisionComponent playerC{};
    playerC.staticEntity = 0;
    playerC.collisionBox = {{18, 20}, 61, 76};
    playerC.previousCollisionBox = playerC.collisionBox;
    playerC.previousCollisionBox.topLeft += playerSpawn;
    em.AddComponent(playerId, playerC);

    em.AddComponent(playerId, ECSEngine::SpriteComponent{
        playerSprite, {{0, 0}, 96, 96}, true
    });
    em.AddComponent(playerId, ECSEngine::InputComponent(playerId));
    em.AddComponent(playerId, ECSEngine::RespawnComponent{playerSpawn});

    // Place a star
    ECSEngine::Point2D starSpawn({512, 192});
    auto starId = em.CreateEntity("star");
    em.AddComponent(starId, ECSEngine::LocationComponent{starSpawn});
    em.AddComponent(starId, ECSEngine::MovementComponent{{0, 0}});

    ECSEngine::CollisionComponent starC{};
    starC.staticEntity = 0;
    starC.collisionBox = {{12, 13}, 40, 38};
    starC.previousCollisionBox = starC.collisionBox;
    starC.previousCollisionBox.topLeft += starSpawn;
    starC.collectable = 1;
    em.AddComponent(starId, starC);

    em.AddComponent(starId, ECSEngine::SpriteComponent{
        starSpriteId, {{0, 0}, 64, 64}, true
    });
    


    // Setup score component
    ECSEngine::ScoreComponent sc{};
    sc.score = 0;
    for (int i = 0; i < 10; ++i) {   // register score sprites
        auto score = spriteM.RegisterTexture(
            gResourcePath + "spritesheet-tiles-default.png", {{832, 832 - i * 64}, 64, 64}
        );
        sc.digitTextureID[i] = score;
    }
    sc.digitID[0] = em.CreateEntity("scoreOnes");
    sc.digitID[1] = em.CreateEntity("scoreTens");
    sc.digitID[2] = em.CreateEntity("scoreHundreds");

    for (int i = 0; i < 3; ++i) {   // register components with score entities
        em.AddComponent(sc.digitID[i], ECSEngine::LocationComponent{{90 - i * 40, 10}});
        em.AddComponent(sc.digitID[i], ECSEngine::SpriteComponent{
            sc.digitTextureID[0], {{0, 0}, 64, 64}, false
        });
    }
    em.AddComponent(playerId, sc);
    
    // Setup Camera
    auto camera = em.CreateEntity("camera");
    em.AddComponent(camera, ECSEngine::CameraFollower{playerId});
    // TODO: change the exclusion zones to not be hardcoded
    em.AddComponent(camera, ECSEngine::CameraComponent{
        {playerSpawn.x, 448.f}, 1.f, {{102.4f, 0.f}, 819.2f, 768}, {{307.2f, 0.f}, 409.6f, 768.f}, 0.05f
    });

    // Add all systems
    sm.AddSystem(std::make_unique<ECSEngine::CollisionLateSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::InputSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::GravitySystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::MovementSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CollisionSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::ScoreSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CameraSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpriteSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpawnSystem<COMPONENTS>>());

    engine.Run();
    return 0;
}
