//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "ssl_security.hpp"
#include "safe_queue.hpp"

std::mutex cv_mutex;
std::condition_variable cv;
// MessageQueue request_queue, response_queue;

using boost::asio::ip::tcp;
using boost::system::error_code;

void push_and_notify(std::string response)
{
    {
        std::cout << "Trying to lock...\n";
        std::lock_guard<std::mutex> lock(cv_mutex);
        std::cout << "Locked\n";
        response_queue.push(response);
    }
    std::cout << "Pushed, notifying\n";
    cv.notify_one();
    std::cout << "Notified...\n";
}

void finish(SSL_stream& ssl_stream, boost::system::error_code error)
{
    if(error)
        std::cout << "Error: " << error << std::endl;

    ssl_stream.shutdown(error);
    ssl_stream.lowest_layer().close(error);
    std::cout << "shutdown sucessful\n";
}

class Session {
    SSL_stream ssl_stream;
    char data[1024];
    bool active{true};
public:
    bool IsActive(){return active;}
    Session(ssl_context& ctx, tcp::socket socket) 
        : ssl_stream(std::move(socket), ctx)
    {
        this -> handshake();  
    }
    
    void start() { do_read(); }
    ~Session(){ std::cout << "Se autodestruindo\n"; boost::system::error_code ec; finish(ssl_stream, ec); }
private:
    void do_read(int timeout = 30)
    {
        error_code ec;
        // std::cout << "reading...\n";
        std::size_t length = ssl_stream.read_some(boost::asio::buffer(data), ec);
        // std::cout << "readed...\n";

        if(!ec)
        {
            // std::cout << "pushing...\n";
            request_queue.push(std::string(data,length));
            do_write();
        }else{
            std::cout << ec << std::endl;
        }
    }

    void do_write()
    {
        std::cout << "Waiting response\n";

        std::string response;
        // Agora espera resposta
        {
            std::unique_lock<std::mutex> lock(cv_mutex);
            cv.wait(lock, []{ return !response_queue.empty(); }); // libera mutex depois retoma!
            response = response_queue.pop();
        }
  
        error_code ec;
        std::cout << "Sending... " << response << std::endl;
        ssl_stream.write_some(boost::asio::buffer(response),ec);
        if(!ec)
        {
            do_read();
        }
    }

    void handshake(){ 
        boost::system::error_code ec;
        ssl_stream.handshake(SERVER,ec); 
        if(ec)
            std::cout << "handshake failed: " << ec.message() << std::endl;
        else
            std::cout << "handshake ok\n"; 
    }
};

class Server
{
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
            ctx(server_ca())
    {
        do_accept(io_context);
    }

private:
    void do_accept(boost::asio::io_context& io_context)
    {
        boost::system::error_code ec;
        tcp::socket socket(io_context);

        // Bloqueia até aparecer uma conexão
        acceptor_.accept(socket, ec);

        if (!ec)
        {
            // Cria sessão e roda
            {
                auto session = Session(ctx, std::move(socket));
                session.start();
            } // sai de escopo

            // Aqui você decide: espera a sessão terminar antes de aceitar outra
            // ou simplesmente encerra o servidor após a primeira conexão

            std::cout << "accepting...\n";
            do_accept(io_context);
        }
    }

    tcp::acceptor acceptor_;
    ssl_context ctx;
};