/*
    Menu scene for rendering

    -> O que vamos precisar de widgets:

    1. Buttons em que podemos inserir nomes customizados

        Aqui teremos button para play local, online, contra IA futuramente
        Depois escolhe server ou client...

    Antes de prosseguir vamos criar o Button na API do Ray3D - seria bom o HUD ser um submodulo para ficar melhor

    2. Digitar o IP: precisaremos de uma input box (já temos aliás, só melhorar a implementação...)
*/

#pragma once
#include <raylib.h>
#include "widgets/input_text.hpp"
#include "widgets/button.hpp"
#include "scenes.hpp"


static Button init_button()
{
    static Button button({300,400,200,100}); // criado só uma vez
    button.SetText("Online");
    button.SetBackgroundColor(ORANGE);
    return button;
}

void draw_menu_background()
{
    DrawRectangle(0,0,1280,720,YELLOW); // fundo yellow
}

// void render_menu_scene(InputText& my_input_text)
SCENE render_menu_scene()
{
    // static InputText my_input_text({300,400,100,100}); // criado só uma vez
    static Button button = init_button();

    BeginDrawing(); 
        ClearBackground(RAYWHITE);
        draw_menu_background();
        button.Draw();
    EndDrawing(); 

    if(button.Clicked())
    {
        RUNNING_CLIENT_FLAG.store(true);
        RUNNING_SERVER_FLAG.store(true);
        START_CLIENT_NETWORK.store(true);
        START_SERVER_NETWORK.store(true);
        return GAME_SCENE;
    }else{
        return MENU_SCENE;
    }
}

SCENE render_continue_scene()
{
    /*-------------- DEFINITION ------------ */
    static Button botao_sim({100,400,200,100});
    botao_sim.SetText("Sim");
    botao_sim.SetBackgroundColor(GREEN);

    static Button botao_nao({400,400,200,100});
    botao_nao.SetText("Nao");
    botao_nao.SetBackgroundColor(RED);

    /*-------------- RENDER ------------ */
    BeginDrawing(); 
        ClearBackground(RAYWHITE);
        draw_menu_background();
        botao_sim.Draw();
        botao_nao.Draw();
    EndDrawing(); 

    /*-------------- LOGIC ------------ */
    if(botao_sim.Clicked())
        return GAME_SCENE;
    if(botao_nao.Clicked())
    {
        RUNNING_CLIENT_FLAG.store(false);
        RUNNING_SERVER_FLAG.store(false);
        return MENU_SCENE;
    }
    return CONTINUE_SCENE;
}