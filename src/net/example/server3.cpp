#include "../server.hpp"
#include <ctime>
#include <thread>
#include "../parser.hpp"

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

int main()
{
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 4433));
    generate_ca();
    auto ctx = context_with_ca();
    for(;;)
    {
        Session session(io_context, acceptor, ctx);
        session.handshake();
        while(session.IsActive())
        {
            std::string mensagem = session.rcv();
            std::cout << mensagem << std::endl;
            if(is_sync_move(mensagem))
            {
                SyncMove move = parse(mensagem);
                std::cout << "Move: " << move.piece << move.x << move.y << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            session.write(make_daytime_string());
        }
    }
}