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
SCENE render_menu_scene(bool versao_antiga = true)
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
        if(versao_antiga)
        {
            RUNNING_CLIENT_FLAG.store(true);
            RUNNING_SERVER_FLAG.store(true);
            START_CLIENT_NETWORK.store(true);
            START_SERVER_NETWORK.store(true);
            return GAME_SCENE;
        }
        else
        {
            return NETWORK_SIDE_SCENE;
        }
    }else{
        return MENU_SCENE;
    }
}

SCENE render_network_side(NETWORK_SIDE* side, Player& player)
{
    static Button client_button({300,250,200,100}); // criado só uma vez
    client_button.SetText("Client");
    client_button.SetBackgroundColor(ORANGE);

    static Button server_button({300,350,200,100}); // criado só uma vez
    server_button.SetText("Server");
    server_button.SetBackgroundColor(ORANGE);

    /* ---------------- RENDER -------------- */
    BeginDrawing(); 
        ClearBackground(RAYWHITE);
        draw_menu_background();
        client_button.Draw();
        server_button.Draw();
    EndDrawing(); 

    /* -------------- LÓGICA ----------------*/

    if(client_button.Clicked())
    {
        // esvaziar a queue
        while(!request_queue.empty())
            request_queue.pop();

        RUNNING_CLIENT_FLAG.store(true);
        START_CLIENT_NETWORK.store(true);
        *side = CLIENT_SIDE;
        player = Player(PIECE_COLOR_VIOLET); // por enquanto deixemos assim...
        return GAME_SCENE;
    }

    if(server_button.Clicked())
    {
        RUNNING_SERVER_FLAG.store(true);
        START_SERVER_NETWORK.store(true);
        *side = SERVER_SIDE;
        player = Player(PIECE_COLOR_GOLD);
        return GAME_SCENE;
    }

    return NETWORK_SIDE_SCENE;
}

SCENE render_continue_scene(NETWORK_SIDE net_side)
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
    {
        // ignore_finish_connection_message();
        return (net_side == SERVER_SIDE) ? WAIT_CLIENT_RESPONSE_SCENE : GAME_SCENE;
    }
        
    if(botao_nao.Clicked())
    {
        if(net_side == CLIENT_SIDE)
        {
            RUNNING_CLIENT_FLAG.store(false);
        }else{
            RUNNING_SERVER_FLAG.store(false);
            push_and_notify("finish");
        }
        
        return MENU_SCENE;
    }
    return CONTINUE_SCENE;
}

SCENE render_wait_client_response_scene()
{
    /* Animação dos 3 pontinhos */
    static int frame = 0;
    frame++;
    std::string wait = "Waiting client connection";
    static std::string points = "";
    if(frame == 60)
    {
        if (points.size() < 3)
            points.push_back('.');
        else
            points = "";
        frame = 0;
    }

    BeginDrawing(); 
        ClearBackground(RAYWHITE);
        draw_menu_background();
        DrawText(
            (wait+points).c_str(),
            400, 300,24, BLACK // centralizar
        );
    EndDrawing();

    if(!request_queue.empty())
    {
        std::string front = request_queue.front();
        if(front == "ping")
        {
            // ignore_finish_connection_message();
            request_queue.pop();
            return GAME_SCENE;
        }
        else if(request_queue.front() == "Finish connection")
        {
            // debug
            // std::cout << "Received finish connection\n";
            push_and_notify("finish");
            return GAME_SCENE; // aqui vamos criar uma tela de esperando nova conexão...
        }
    }

    return WAIT_CLIENT_RESPONSE_SCENE;
}