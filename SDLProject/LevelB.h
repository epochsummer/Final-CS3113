// LevelA.h
#include "Scene.h"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 3;

    bool level_win = false;

    ~LevelB();

    //getter
    bool      const get_level_win() const { return level_win; }

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;


};
