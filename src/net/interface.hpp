#include <thread>
#include <string>
#include <iostream>
#include "client.hpp"
#include "safe_queue.hpp"
#include "sync_server.hpp"
#include <atomic>

// uso externo no código...
std::atomic<bool> START_CLIENT_NETWORK{false};
std::atomic<bool> START_SERVER_NETWORK{false};
std::atomic<bool> RUNNING_CLIENT_FLAG{true};

void start_client()
{  
    Client client(boost::asio::ssl::verify_none);
    client.connect("127.0.0.1","4433");
    client.handshake();
    std::cout << "handshake ok\n";

    
    // client.write("ping");
    
    // std::cout << "Receveid: " << response << std::endl;
    // response_queue.push(response);

    // std::thread receiver([&client]{
    //     while(RUNNING_CLIENT_FLAG)
    //     {
    //         std::string response = client.rcv();
    //         if(response.size() > 0)
    //         {
    //             std::cout << "Received: " << response << std::endl
    //                 << "FLAG: " << RUNNING_CLIENT_FLAG << std::endl;
    //             response_queue.push(response);
    //         }
    //     }
    // });

    // std::thread([&client]{
    //     while(RUNNING_CLIENT_FLAG)
    //     {
    //         std::string response = client.rcv();
    //         if(response.size() > 0)
    //         {
    //             std::cout << "Received: " << response << std::endl
    //                 << "FLAG: " << RUNNING_CLIENT_FLAG << std::endl;
    //             response_queue.push(response);
    //         }
    //     }
    //     std::cout << "OK finished\n";
    // }).detach();

    client.async_rcv([&](std::string response){
        std::cout << "Received: " << response << std::endl
                << "FLAG: " << RUNNING_CLIENT_FLAG << std::endl;
        response_queue.push(response);
    });

    std::thread sender([&client](){
        while(RUNNING_CLIENT_FLAG)
        {
            if(!request_queue.empty())
            {
                // Bloqueia e espera por uma mensagem na fila
                std::string msg_to_send = request_queue.pop(); 
                
                if (msg_to_send == "__STOP__") break;
                
                client.write(msg_to_send);
            }
        }

        // cancel read operation
        client.cancel();

        std::cout << "finishing...\n";
        client.write("Finish connection");
    });
    
    client.run(); // O run só será chamado após o loop terminar
    sender.join();
    std::cout << "Sender joinned\n";
    // receiver.join();
    
    std::cout << "OK exit scope\n";
}

void start_server()
{
    boost::asio::io_context io_context;
    Server s(io_context, 4433);
    io_context.run();
}