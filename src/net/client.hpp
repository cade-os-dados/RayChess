#pragma once
#include <iostream>
#include <memory>
#include "ssl_security.hpp"

#define DEBUG true

using boost::asio::ip::tcp;
namespace asio = boost::asio;

auto find_endpoints(boost::asio::io_context& io, const char* ip, const char* port)
{
    tcp::resolver resolver(io);
    auto endpoints = resolver.resolve("127.0.0.1", "4433");
    return endpoints;
}

void connect_to_server(SSL_stream& ssl_stream, boost::asio::io_context& io, const char* ip, const char* port)
{
    auto endpoints = find_endpoints(io,ip,port);
    boost::asio::connect(ssl_stream.lowest_layer(),endpoints);
}

class Client{
private:
    std::array<char,128> buffer;
    boost::system::error_code error;
    SSL_stream* ssl_stream;
    int max_retries{3}, retries{0};
    boost::asio::io_context io;
    std::atomic<bool> cancelled{false};
public:
    Client(boost::asio::ssl::verify_mode mode = boost::asio::ssl::verify_peer)
    {
        auto ctx = load_ca();
        ctx.set_verify_mode(mode);
        ssl_stream = new SSL_stream(io, ctx);
    }
    ~Client(){ this -> shutdown(); delete ssl_stream; }
    void shutdown(){
        boost::system::error_code ec;
        ssl_stream->shutdown(ec);
        if(ec) std::cerr << "Shutdown error: " << ec.message() << std::endl;
    }
    void connect(std::string_view ip, std::string_view port)
    {
        connect_to_server(*ssl_stream, io, ip.data(), port.data());
    }
    void handshake(){ssl_stream -> handshake(CLIENT);}
    void run(){io.run();}

    void write(std::string_view string)
    {
        if(DEBUG)
            std::cout << "Requisicao enviada." << std::endl;
        ssl_stream -> write_some(asio::buffer(string));
    }

    /*
        Aqui ele trava por que read_some é síncrono

        Para nao modificar podemos tentar mudar um pouco o fluxo
        no jogo...

        Quando dá o endgame, tem que colocar tanto em server quanto em client
        se deseja continuar e meio que criar um handshake para saber que
        está tudo certo...
    */
    std::string rcv()
    {
        size_t len = ssl_stream -> read_some(asio::buffer(buffer),error);
        if(DEBUG)
            std::cout << buffer.data() << std::endl;
        if (error != asio::error::eof && error != boost::asio::ssl::error::stream_truncated && error) {
            // Se o erro não for 'eof' (End Of File), significa que houve um erro real
            throw boost::system::system_error(error);
        }
        retries = len == 0 ? retries + 1 : retries;
        if(retries == max_retries) throw std::runtime_error("Max retries exceeded");
        return std::string(buffer.data(), len);
    }


    void async_rcv(std::function<void(std::string)> lambda)
    {
        std::cout << "callback called\n";
        ssl_stream->async_read_some(
            asio::buffer(buffer),
            [this,lambda](const boost::system::error_code& error, std::size_t len)
            {
                if (cancelled) {
                    // Cancelado manualmente
                    return;
                }

                if (error == asio::error::operation_aborted) {
                    // Cancelamento via cancel()
                    return;
                }

                if (error) {
                    throw boost::system::system_error(error);
                }

                std::string data(buffer.data(), len);
                std::cout << "Data:" << data << std::endl;
                // processar data
                lambda(data);
                std::cout << "lambda called\n";
                async_rcv(lambda);
            }
        );
    }

    void cancel()
    {
        cancelled.store(true);
        ssl_stream->lowest_layer().cancel();
    }

    std::string request(std::string_view string)
    {
        this -> write(string);
        return this -> rcv();
    }
};