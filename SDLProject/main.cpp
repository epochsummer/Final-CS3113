/**
* Author: Seha Kim
* Assignment: Man in Black
* Date due: May 2nd, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Effects.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "MenuScreen.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"

constexpr int WINDOW_WIDTH = 640, WINDOW_HEIGHT = 480;

constexpr float BG_RED = 0.1922f, BG_BLUE = 0.549f, BG_GREEN = 0.9059f, BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0, VIEWPORT_Y = 0, VIEWPORT_WIDTH = WINDOW_WIDTH, VIEWPORT_HEIGHT = WINDOW_HEIGHT;

GLuint g_background_texture;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl", F_SHADER_PATH[] = "shaders/effects_textured.glsl", FONT_SPRITE_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/font1.png";

BulletDirection bullet_direction = BulletDirection::NONE;

constexpr char BGM_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/371516__mrthenoronha__space-game-theme-loop.wav";

constexpr int CD_QUAL_FREQ = 44100, AUDIO_CHAN_AMT = 2, AUDIO_BUFF_SIZE = 4096;

Mix_Music* g_music;

Effects* g_effects;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

bool enter_pressed = false;
bool right_pressed = false;
bool left_pressed = false;
bool up_pressed = false;
bool down_pressed = false;
bool succeed = false;
bool failed = false;
bool next_level = false;
bool death_played = false;

GLuint is_player_dead_location;
GLuint text_texture_id;

Scene* g_current_scene;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
MenuScreen* g_menu_screen;

Scene* g_levels[4];
SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr) { shutdown(); }

#ifdef _WINDOWS
    glewInit();
#endif

    Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);
    Mix_AllocateChannels(16);
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    std::cout << "MUSIC IS BEING PLAYED" << std::endl;
    Mix_PlayMusic(g_music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME);

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());

    is_player_dead_location = glGetUniformLocation(g_shader_program.get_program_id(), "player_dead");

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    text_texture_id = Utility::load_texture(FONT_SPRITE_FILEPATH);

    g_menu_screen = new MenuScreen();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();

    g_levels[0] = g_menu_screen;
    g_levels[1] = g_level_a;
    g_levels[2] = g_level_b;
    g_levels[3] = g_level_c;

    switch_to_scene(g_levels[3]);

    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(FADEIN, 0.2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    if (g_current_scene->get_state().player->get_is_defeated()) failed = true;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_app_status = TERMINATED;
                break;

            case SDLK_SPACE:
                if (!failed && !succeed)
                {
                    if (left_pressed && fabs(g_current_scene->get_state().bullet->get_position().x - g_current_scene->get_state().player->get_position().x) < 1.0f)
                    {
                        g_current_scene->get_state().bullet->move_left();
                        Mix_PlayChannel(2, g_current_scene->get_state().shoot_sfx, 0);
                    }
                    else if (right_pressed && fabs(g_current_scene->get_state().bullet->get_position().x - g_current_scene->get_state().player->get_position().x) < 1.0f)
                    {
                        g_current_scene->get_state().bullet->move_right();
                        Mix_PlayChannel(2, g_current_scene->get_state().shoot_sfx, 0);
                    }
                }
                break;

            case SDLK_RETURN:
                enter_pressed = true;
            default:
                break;
            }
        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        if (!failed && !succeed)
        {
            g_current_scene->get_state().player->move_left();
            Mix_PlayChannel(-1, g_current_scene->get_state().walking_sfx, 0);
            Mix_VolumeChunk(g_current_scene->get_state().walking_sfx, MIX_MAX_VOLUME / 2.0f);
            right_pressed = false;
            left_pressed = true;
        }
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        if (!failed && !succeed)
        {
            g_current_scene->get_state().player->move_right();
            Mix_PlayChannel(-1, g_current_scene->get_state().walking_sfx, 0);
            Mix_VolumeChunk(g_current_scene->get_state().walking_sfx, MIX_MAX_VOLUME / 2.0f);
            left_pressed = false;
            right_pressed = true;
        }
    }
    else if (key_state[SDL_SCANCODE_UP])
    {
        if (!failed && !succeed)
        {
            g_current_scene->get_state().player->move_up();
            Mix_PlayChannel(-1, g_current_scene->get_state().walking_sfx, 0);
            Mix_VolumeChunk(g_current_scene->get_state().walking_sfx, MIX_MAX_VOLUME / 2.0f);
        }
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        if (!failed && !succeed)
        {
            g_current_scene->get_state().player->move_down();
            Mix_PlayChannel(-1, g_current_scene->get_state().walking_sfx, 0);
            Mix_VolumeChunk(g_current_scene->get_state().walking_sfx, MIX_MAX_VOLUME / 2.0f);
        }
    }

    if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    if (g_current_scene->get_state().player->get_is_defeated())
    {
        failed = true;
        if (!death_played)
        {
            Mix_PlayChannel(1, g_current_scene->get_state().death_sfx, 0);
            death_played = true;
        }
    }

    if (g_current_scene == g_menu_screen && enter_pressed)
    {
        switch_to_scene(g_levels[1]);
        g_effects->start(FADEIN, 0.5);
    }

    if (g_current_scene == g_level_a && g_level_a->get_level_win())
    {
        switch_to_scene(g_levels[2]);
        g_effects->start(FADEIN, 0.5);
    }

    if (g_current_scene == g_level_b && g_level_b->get_level_win())
    {
        switch_to_scene(g_levels[3]);
        g_effects->start(FADEIN, 0.5);
    }

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));
}

void render_background()
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(g_current_scene->get_state().player->get_position().x, g_current_scene->get_state().player->get_position().y, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 7.5f, 1.0f));

    g_shader_program.set_model_matrix(model_matrix);
    glUseProgram(g_shader_program.get_program_id());

    float vertices[] = { -1, 1, -1, -1, 1, -1, -1, 1, 1, -1, 1, 1 };
    float tex_coords[] = { 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1 };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, g_background_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    render_background();
    g_shader_program.set_view_matrix(g_view_matrix);

    g_current_scene->render(&g_shader_program);
    g_effects->render();

    bool is_player_dead = g_current_scene->get_state().player->get_is_defeated();
    glUniform1i(is_player_dead_location, is_player_dead ? 1 : 0);

    if (is_player_dead)
    {
        Utility::draw_text(&g_shader_program, text_texture_id, "You Died", 0.3, 0.03f, glm::vec3(g_current_scene->get_state().player->get_position().x - 2.0f, g_current_scene->get_state().player->get_position().y, 0.0f));
    }

    if (g_level_c->get_level_win())
    {
        succeed = true;
        Utility::draw_text(&g_shader_program, text_texture_id, "Great Job!", 0.3, 0.03f, glm::vec3(g_current_scene->get_state().player->get_position().x - 2.0f, g_current_scene->get_state().player->get_position().y, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    delete g_menu_screen;
    delete g_level_a;
    delete g_level_b;
}

int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
