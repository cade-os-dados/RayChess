#include "../client.hpp"
#include "../parser.hpp"

int main() {
    Client client;
    client.connect("127.0.0.1","4433");
    client.handshake();

    std::cout << client.request("hello") << std::endl; // request = send -> receive
    std::cout << client.request("world!") << std::endl;
    client.request(to_str({14,0,0}));

    client.run();

    char ola[100];
    std::cin >> ola;
}