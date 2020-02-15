#include "game.h"
#include "fps.h"
#include "machine.h"
#include <memory>


int main() {
    // TODO: SHOULD LOAD PGR ROM AND TRY AGAIN
    Game game(800, 480, "FCEmu");
    auto p_fps = std::make_shared<Fps>();
    auto machine_entity = std::make_shared<MachineEntity>();
    game.addEntity(p_fps);
    game.addEntity(machine_entity);
    game.run();
    return 0;
}