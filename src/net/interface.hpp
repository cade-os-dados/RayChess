#include <thread>
#include <string>
#include <iostream>
#include "client.hpp"
#include "safe_queue.hpp"
#include "sync_server.hpp"

void start_client()
{  
    Client client(boost::asio::ssl::verify_none);
    client.connect("127.0.0.1","4433");
    client.handshake();
    std::cout << "handshake ok\n";

    
    client.write("ping");
    // std::cout << "Receveid: " << response << std::endl;
    // response_queue.push(response);

    std::thread receiver([&client](){
        while(true)
        {
            std::string response = client.rcv();
            std::cout << "Received: " << response << std::endl;
            response_queue.push(response);
        }
    });

    std::thread sender([&client](){
        while(true)
        {
            if(!request_queue.empty())
            {
                // Bloqueia e espera por uma mensagem na fila
                std::string msg_to_send = request_queue.pop(); 
                
                if (msg_to_send == "__STOP__") break;
                
                client.write(msg_to_send);
            }
        }
    });

    sender.join();
    receiver.join();
    client.run(); // O run só será chamado após o loop terminar
}

void start_server()
{
    boost::asio::io_context io_context;
    Server s(io_context, 4433);
    io_context.run();
}