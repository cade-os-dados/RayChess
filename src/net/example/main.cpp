#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp> // Garante que a exceção seja encontrada

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// Domínio do servidor que vamos acessar
const std::string SERVER_HOST = "www.google.com";
const std::string SERVER_PORT = "https"; // A porta padrão HTTPS é 443

void run_client() {
    try {
        // 1. Configurar o Contexto SSL
        asio::ssl::context ctx(asio::ssl::context::sslv23);
        

        // Tente carregar manualmente o bundle de certificados CA
        const char* CA_CERT_PATH = "/etc/ssl/certs/ca-certificates.crt"; // Comum no Ubuntu/Debian
        // const char* CA_CERT_PATH = "/etc/pki/tls/certs/ca-bundle.crt"; // Comum no Fedora/CentOS

        std::cout << "Tentando carregar certificados de: " << CA_CERT_PATH << std::endl;
        
        try {
            // Tenta carregar o arquivo específico
            ctx.load_verify_file(CA_CERT_PATH);
            std::cout << "Certificados CA carregados com sucesso." << std::endl;
        } catch (const boost::system::system_error& e) {
            std::cerr << "AVISO: Falha ao carregar o arquivo de certificado CA. Erro: " << e.what() << std::endl;
            // Se falhar, ele continuará a usar os caminhos padrão, mas você tem mais diagnóstico.
        }

        // // Carrega os certificados CA padrão do sistema
        // ctx.set_default_verify_paths();
        // // Permite a verificação da cadeia de confiança do certificado
        // ctx.set_verify_mode(asio::ssl::verify_peer); 

        // 2. Inicializar I/O e Resolver
        asio::io_context io_context;
        tcp::resolver resolver(io_context);

        // 3. Criar o Stream SSL
        asio::ssl::stream<tcp::socket> ssl_stream(io_context, ctx);
        
        // **CORREÇÃO: Verificação do Nome do Host**
        ssl_stream.set_verify_callback(
            asio::ssl::host_name_verification(SERVER_HOST));
        
        // 4. Resolver o Endereço (DNS)
        auto endpoints = resolver.resolve(SERVER_HOST, SERVER_PORT);

        // 5. Conectar o Socket TCP Subjacente
        asio::connect(ssl_stream.lowest_layer(), endpoints);
        std::cout << "Socket TCP conectado com sucesso." << std::endl;

        // 6. Realizar o Handshake SSL/TLS
        ssl_stream.handshake(asio::ssl::stream_base::client);
        std::cout << "Handshake SSL/TLS concluído. Conexão segura estabelecida." << std::endl;

        // 7. Enviar a Requisição HTTP
        std::string request = 
            "GET / HTTP/1.1\r\n"
            "Host: " + SERVER_HOST + "\r\n"
            "User-Agent: C++ Asio Client v1.0\r\n" // <<< ADICIONE ESTA LINHA
            "Accept: */*\r\n"                      // <<< ADICIONE ESTA LINHA
            "Connection: close\r\n"
            "\r\n";
        
        asio::write(ssl_stream, asio::buffer(request));
        std::cout << "Requisição enviada." << std::endl;

        // 8. Receber a Resposta (LEITURA EM DUAS ETAPAS)
        asio::streambuf response;
        boost::system::error_code ec;

        // ETAPA A: Ler o cabeçalho (até encontrar a linha em branco \r\n\r\n)
        size_t header_bytes = asio::read_until(ssl_stream, response, "\r\n\r\n", ec);
        
        if (ec) {
            std::cerr << "Erro ao ler cabeçalho: " << ec.message() << std::endl;
            // Se falhar aqui (timeout, etc.), lance o erro
            throw boost::system::system_error(ec);
        }

        // Exibir o cabeçalho lido (opcional)
        std::cout << "Header lido: " << header_bytes << " bytes." << std::endl;
        // Se você quiser ver o cabeçalho, descomente as linhas abaixo
        // std::istream response_stream(&response);
        // std::string line;
        // while (std::getline(response_stream, line) && line != "\r") {
        //     std::cout << line << "\n";
        // }
        // std::cout << "\n";

        // ETAPA B: Ler o restante do corpo (até o servidor fechar a conexão)
        // O asio::read_until pode ter lido mais do que o \r\n\r\n.
        // O asio::read() continua lendo até o EOF.
        size_t body_bytes = asio::read(ssl_stream, response, ec);
        
        if (ec != asio::error::eof && ec != boost::asio::ssl::error::stream_truncated) {
            // Se o erro não for 'eof' (End Of File), significa que houve um erro real
            throw boost::system::system_error(ec);
        }

        // 9. Imprimir a Resposta
        // O response agora contém tanto o header quanto o body
        size_t total_bytes = header_bytes + body_bytes; 
        std::cout << "Resposta recebida (" << total_bytes << " bytes):" << std::endl;
        std::cout << &response << std::endl;

       // 10. Fechar o Stream SSL (Shutdown)
        
        // Crie uma nova error_code para o shutdown
        boost::system::error_code shutdown_ec; 
        
        // Tente realizar o shutdown
        ssl_stream.shutdown(shutdown_ec);
        
        // Verifique se o erro foi o esperado (stream truncated), o que significa
        // que o servidor já fechou o lado dele.
        if (shutdown_ec == asio::error::eof || 
            shutdown_ec == asio::ssl::error::stream_truncated) 
        {
            // O servidor fechou primeiro. Consideramos um encerramento normal.
            std::cout << "Conexão fechada: Servidor fechou o stream TLS primeiro." << std::endl;
        } else if (shutdown_ec) {
            // Outro erro de shutdown (algo grave)
            std::cerr << "Erro inesperado durante o shutdown: " << shutdown_ec.message() << std::endl;
            // Se for um erro que você queira propagar:
            // throw boost::system::system_error(shutdown_ec);
        } else {
            // Shutdown bilateral (handshake) bem-sucedido
            std::cout << "Conexão fechada com SSL shutdown bilateral." << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

int main() {
    run_client();
    return 0;
}