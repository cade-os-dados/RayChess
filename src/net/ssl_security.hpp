#pragma once
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSL_stream;
typedef boost::asio::ssl::context ssl_context;
#define SERVER boost::asio::ssl::stream_base::server
#define CLIENT boost::asio::ssl::stream_base::client

void set_ca(boost::asio::ssl::context& ctx)
{
    // Carregar certificado e chave (precisa gerar antes com OpenSSL)
    ctx.use_certificate_file("resources/ca/server.crt", boost::asio::ssl::context::pem);
    ctx.use_private_key_file("resources/ca/server.key", boost::asio::ssl::context::pem);
}

ssl_context load_ca()
{
    ssl_context ctx(ssl_context::tlsv13_client);
    try{ ctx.load_verify_file("resources/ca/server.crt"); }
    catch(const std::exception& e){ 
        std::cout << "Não foi possível carregar o certificado do servidor, favor solicitar ao administrador o certificado e inserir no caminho resources/ca/server.crt"; 
    }
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    return ctx;
}

ssl_context context_with_ca()
{
    ssl_context ctx(ssl_context::tlsv13_server);
    set_ca(ctx);
    return ctx;
}

inline bool run(std::string cmd) {
    int ret = std::system(cmd.data());
    return ret == 0;
}

void generate_ca(std::string bin = "openssl") {
    if(!fs::exists("ca"))
        fs::create_directories("ca");

    bool no_changes = true;

    if (!fs::exists("resources/ca/server.key")) {
        if (!run(bin+" genrsa -out resources/ca/server.key 2048")) return;
        std::cout << "Chave criptografica (PEM) gerada com sucesso!\n";
        no_changes = false;
    }

    if (!fs::exists("resources/ca/server.csr")) {
        if (!run(bin+" req -new -key resources/ca/server.key -out resources/ca/server.csr -config resources/server.cnf")) return;
        std::cout << "CSR gerado com sucesso!\n";
        no_changes = false;
    }

    if (!fs::exists("resources/ca/server.crt")) {
        if (!run(bin+" x509 -req -in resources/ca/server.csr -signkey resources/ca/server.key "
                 "-out resources/ca/server.crt -days 365 -extfile server.cnf -extensions v3_req")) return;
        std::cout << "Certificado assinado com sucesso!\n";
        no_changes = false;
    }

    if (no_changes)
        std::cout << "Certificado ja esta configurado!\n";
}

ssl_context server_ca()
{
    generate_ca();
    ssl_context ctx(ssl_context::tlsv13_server);
    set_ca(ctx);
    return ctx;
}