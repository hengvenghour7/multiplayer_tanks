#include <iostream>
#include <boost/asio.hpp>
#include <vector>

using boost::asio::ip::tcp;

int playerID = 0;
struct playerLocation {
    int id{0};
    int x{0};
    int y{0};
};
int x = 300;
std::vector<playerLocation> allPlayerLocation {};
class tcp_connection
    :public std::enable_shared_from_this<tcp_connection>
    {
        public:
            typedef std::shared_ptr<tcp_connection> pointer;
            static pointer create(boost::asio::io_context& io_context) {
                return pointer(new tcp_connection(io_context));
            }
            tcp::socket& socket() {
                return socket_;
            }
            void start() {
                playerID++;
                allPlayerLocation.emplace_back(playerLocation{playerID, x, 300});
                x+=30;
                std::string player_update{};
                for (playerLocation player: allPlayerLocation) {
                    player_update = player_update + "," + std::to_string(player.id) + "," + std::to_string(player.x) + "," + std::to_string(player.y);
                }
                std::string playerID_message = "playerID " + std::to_string(playerID) + player_update;
                boost::asio::async_write(socket_, boost::asio::buffer(playerID_message), [](const boost::system::error_code& /*ec*/, std::size_t /*bytes_transferred*/){});
                boost::system::error_code ec;
                handle_read(ec);
            }
        private:
            tcp_connection(boost::asio::io_context& io_context) : socket_(io_context) {
            }
            void handle_read (const boost::system::error_code&) {
                auto self = shared_from_this();
                socket_.async_read_some(boost::asio::buffer(data_), [this, self](const boost::system::error_code& ec, std::size_t len) {
                    std::string playerData(data_.data(), len);
                    handle_write(ec, len, playerData);
                });
            }
            void handle_write(const boost::system::error_code&, size_t len, std::string data) {
                auto self = shared_from_this();
                std::string data_2 = data;
                data_2 = data_2.substr(data_2.find(',') + 1);
                std::istringstream iss(data_2);
                std::string token;
                playerLocation playerUpdate{};
                while (std::getline(iss, token, ',')) {
                    int id = std::stoi(token);

                    if (!std::getline(iss, token, ',')) break;
                    int x = std::stoi(token);

                    if (!std::getline(iss, token, ',')) break;
                    int y = std::stoi(token);
                    playerUpdate = {id, x, y};
                }
                // std::cin.get();
            //     socket_.async_read_some(boost::asio::buffer(data_), [this, self](const boost::system::error_code& ec, std::size_t length) {
            //         std::string player_update{};
            //         for (playerLocation player: allPlayerLocation) {
            //             player_update = player_update + "," + std::to_string(player.id) + "," + std::to_string(player.x) + "," + std::to_string(player.y);
            //         }
            //         boost::asio::async_write(socket_, boost::asio::buffer(player_update),
            //         std::bind(&tcp_connection::handle_read, shared_from_this(), boost::asio::placeholders::error));
            // });
            for (playerLocation& player : allPlayerLocation) {
                if (player.id == playerUpdate.id) {
                    player = playerUpdate;
                    break;
                }
            }
            std::string player_update{};
                    for (playerLocation player: allPlayerLocation) {
                        player_update = player_update + "," + std::to_string(player.id) + "," + std::to_string(player.x) + "," + std::to_string(player.y);
                    }
            // for easy data checking
            // std::cin.get();
            boost::asio::async_write(socket_, boost::asio::buffer(player_update),
                    std::bind(&tcp_connection::handle_read, shared_from_this(), boost::asio::placeholders::error));
            }
            std::array<char, 1024> data_;
            tcp::socket socket_;
            std::string message_;
    };
class tcp_server {
    public:
    tcp_server (boost::asio::io_context& io_context)
    : io_context_(io_context),
    acceptor_(io_context, tcp::endpoint(tcp::v4(), 8080)) {
        {
            start_accept();
        }
    }
    private: 
        void start_accept() {
            std::cout<<"new connection is accepted";
            tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
            acceptor_.async_accept(new_connection->socket(),
            std::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
        }
        void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error) {
           if (!error) {
            new_connection->start();
           }
           start_accept();
        }
        boost::asio::io_context& io_context_;
        tcp::acceptor acceptor_;
};

int main () {
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}