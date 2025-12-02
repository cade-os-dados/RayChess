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
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "ssl_security.hpp"
#include "safe_queue.hpp"

MessageQueue request_queue, response_queue;

using boost::asio::ip::tcp;

void finish(SSL_stream& ssl_stream, boost::system::error_code error)
{
    if(error)
        std::cout << "Error: " << error << std::endl;

    ssl_stream.shutdown(error);
    ssl_stream.lowest_layer().close(error);
    std::cout << "shutdown sucessful\n";
}

class Session : public std::enable_shared_from_this<Session> {
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
    void do_read()
    {
        auto self(shared_from_this());
        ssl_stream.async_read_some(boost::asio::buffer(data),
        [this,self](boost::system::error_code ec, std::size_t length)
        {
            // std::cout << std::string(data, length) << std::endl;
            // std::string texto = std::string(data,length) == "ULTIMA MENSAGEM" ? "FINISH" : "HELLOU!";
            request_queue.push(std::string(data, length+1));

            // std::cout << 

            while(response_queue.empty())
            {
                std::cout << "Waiting for response...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
            std::string next = response_queue.pop();


            if(!ec) do_write(next, next.size());
        });
    }

    void do_write(std::string msg, std::size_t length)
    {
        auto self(shared_from_this());
        std::cout << "Sending... " << msg << std::endl;
        ssl_stream.async_write_some(boost::asio::buffer(msg),
        [this,self](boost::system::error_code ec, std::size_t)
        {
            if(!ec) do_read();
        });
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
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(ctx,std::move(socket))->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    ssl_context ctx;
};