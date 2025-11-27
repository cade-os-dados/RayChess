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

using boost::asio::ip::tcp;

void finish(SSL_stream& ssl_stream, boost::system::error_code error)
{
    if(error)
        std::cout << "Error: " << error << std::endl;

    ssl_stream.shutdown(error);
    ssl_stream.lowest_layer().close(error);
    std::cout << "shutdown sucessful\n";
}

class Session{
    SSL_stream* ssl_stream;
    char data[1024];
    bool active{true};
public:
    ~Session(){ delete ssl_stream; }
    bool IsActive(){return active;}
    Session(boost::asio::io_context& io, tcp::acceptor& acceptor, ssl_context& ctx)
    {
        tcp::socket socket(io);
        acceptor.accept(socket);
        ssl_stream = new SSL_stream(std::move(socket), ctx);
    }
    void handshake(){ 
        boost::system::error_code ec;
        ssl_stream->handshake(SERVER,ec); 
        if(ec)
            std::cout << "handshake failed: " << ec.message() << std::endl;
        else
            std::cout << "handshake ok\n"; 
    }
    std::string rcv()
    {
        boost::system::error_code ec;
        size_t len = ssl_stream -> read_some(boost::asio::buffer(data), ec);
        if (ec == boost::asio::error::eof || len == 0) {
            std::cout << "Conexão encerrada pelo cliente\n";
            finish(*ssl_stream, ec);
            active = false;
        } else if (ec) {
            throw boost::system::system_error(ec);
        }
        return std::string(data, len);
    }

    void write(std::string_view string)
    {
        if(active)
        {
            boost::system::error_code ec;
            ssl_stream -> write_some(boost::asio::buffer(string), ec);
            if (ec) throw boost::system::system_error(ec);
        }
    }
};