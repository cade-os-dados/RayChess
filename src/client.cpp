#include "net/client.hpp"
#include "net/parser.hpp"
#include <thread>
#include "net/safe_queue.hpp"

MessageQueue request_queue, response_queue;
// ... includes e MessageQueue declarations ...

// A função 'rede()' permanece a mesma:
// Ela lê da request_queue, faz a requisição síncrona, e coloca o resultado na response_queue.
void rede()
{  
    Client client(boost::asio::ssl::verify_none);
    client.connect("127.0.0.1","4433");
    client.handshake();
    std::cout << "handshake ok\n";

    while(true)
    {
        // Bloqueia e espera por uma mensagem na fila
        std::string msg_to_send = request_queue.pop(); 
        
        if (msg_to_send == "__STOP__") break;
        
        // Requisição Síncrona (envia e espera pela resposta)
        std::string response = client.request(msg_to_send); 
        
        // Coloca a resposta na fila para o main (console) ler
        response_queue.push(response);
    }

    client.run(); // O run só será chamado após o loop terminar
}

int main() {
    std::thread new_thread(rede);
    std::string user_input;
    
    // --- Loop para simular o CIN (Leitura de Console) ---
    std::cout << "🌐 Conectado. Digite a mensagem que deseja enviar (ou 'sair' para terminar):\n";
    
    // O loop continua até o usuário digitar 'sair'
    while (true) {
        std::cout << "> ";
        // Leitura da linha completa do console
        std::getline(std::cin, user_input); 
        
        if (user_input == "sair") {
            break;
        }
        
        if (user_input.empty()) {
            continue; // Evita enviar strings vazias
        }

        // 1. Envia a requisição digitada para a thread 'rede()'
        std::cout << "   [INFO] Enviando: '" << user_input << "'\n";
        request_queue.push(user_input);

        // 2. Espera pela resposta que virá da thread 'rede()'
        // A função pop() aqui será BLOCANTE (ela espera a resposta_queue.push(response) acontecer)
        std::cout << "   [INFO] Aguardando resposta...\n";
        std::string server_response = response_queue.pop();
        
        // 3. Exibe a resposta recebida
        std::cout << "   [SERVER] " << server_response << "\n";
    }

    // --- Finalização ---
    
    // Sinaliza para a thread 'rede()' que ela deve parar
    request_queue.push("__STOP__"); 
    
    std::cout << "Encerrando thread de rede...\n";
    new_thread.join(); // Espera a thread 'rede' terminar
    
    std::cout << "Programa encerrado.\n";
    return 0;
}