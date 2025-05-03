// LevelC.h
#include "Scene.h"

class LevelC : public Scene {
public:
    int ENEMY_COUNT = 7;

    bool level_win = false;

    ~LevelC();

    //getter
    bool      const get_level_win() const { return level_win; }

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;


};
