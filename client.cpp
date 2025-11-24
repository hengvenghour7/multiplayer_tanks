// Prevent Windows API conflicts
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Avoid collisions
#define NOGDI           // This prevents GDI functions like Rectangle()
#define NOUSER          // This prevents user functions like CloseWindow, ShowCursor

// Include Raylib first
#include "raylib.h"
#include "raymath.h"

#include <iostream>
#include <boost/asio.hpp>
#include <raylib.h>

using boost::asio::ip::tcp;
struct Player {
    int id;
    int x;
    int y;
};

class Game {
    std::vector<Player> allPlayers{};
    public:
        Game(std::vector<Player> player): allPlayers{player} {
        }
        void drawAllPlayers () {
            for (Player player : allPlayers) {
                DrawCircle(player.x + 100, player.y, 30, RED);
            }
        }
        void updatePlayer(int playerID, int speedx, int speedy) {
            auto it = std::find_if(allPlayers.begin(), allPlayers.end(),
            [playerID](const Player& p) {
                return p.id == playerID;
            });
            std::cout<<"found id"<< it->id;
            it->x += speedx;
            it->y += speedy;
        }
        void updateAllPlayers(std::vector<Player> players) {
            allPlayers = players;
            // std::cout<<"all playyyyy" << allPlayers.size();
        }
        std::vector<Player>::iterator getThisPlayer (int playerID) {
            auto it = std::find_if(allPlayers.begin(), allPlayers.end(),
            [playerID](const Player& p) {
                return p.id == playerID;
            });
            return it;
        }
        std::vector<Player>* getAllPlayers() {
            return &allPlayers;
        }
};
int main() {
    int playerID = 0;
    std::vector<Player> players{};
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "8080");
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        InitWindow(800, 600, "Simple Raylib C++ Window");

    // Circle position
    float x = 400.0f;
    float y = 300.0f;
    float speed = 200.0f;

    SetTargetFPS(60);

    boost::system::error_code error;
    boost::system::error_code send_err;
    std::array<char, 128> buf;
    // socket.non_blocking(true);
    size_t len = socket.read_some(boost::asio::buffer(buf), error);
    // std::string firstmsg = "playerID" + playerID;
    // socket.write_some(boost::asio::buffer(firstmsg), send_err);
    // socket.read_some(boost::asio::buffer(buf), error);
    std::string playerMsg{std::string(buf.data(), len)};
    std::istringstream iss(playerMsg);
    std::string testText;
    iss >> testText >> playerID;
    std::string token1;
    std::getline(iss, token1, ',');
    std::cout<<"oo player"<< playerMsg;
    while (std::getline(iss, token1, ',')) {
        int id = std::stoi(token1);
        std::getline(iss, token1, ',');
        int x = std::stoi(token1);
        std::getline(iss, token1, ',');
        int y = std::stoi(token1);
        
        players.push_back({id, x, y});
        std::cout<< "how many times?";
    }
    std::cout<< "eeerrrr" << playerMsg;
    Game game1(players);
    std::string uu = token1;
    while (!WindowShouldClose()) {
        // Update
            // std::string player_update2{};
            // for (Player player: *game1.getAllPlayers()) {
            //         player_update2 = player_update2 + "," + std::to_string(player.id) + "," + std::to_string(player.x) + "," + std::to_string(player.y);
            // }
            // socket.write_some(boost::asio::buffer(player_update2), send_err);
            // std::cout<<"lllluuuu"<<player_update2;

            std::string player_update2{};
            auto thisPlayer = game1.getThisPlayer(playerID);
            player_update2 = player_update2 + "," + std::to_string(thisPlayer->id) + "," + std::to_string(thisPlayer->x) + "," + std::to_string(thisPlayer->y);
            // for (Player player: *game1.getAllPlayers()) {
            //         player_update2 = player_update2 + "," + std::to_string(player.id) + "," + std::to_string(player.x) + "," + std::to_string(player.y);
            // }
            socket.write_some(boost::asio::buffer(player_update2), send_err);
            std::cout<<"yuuuu "<< player_update2;
            std::array<char, 128> buf2;

        size_t len2 = socket.read_some(boost::asio::buffer(buf2), error);

        if (error) {
            std::cerr << "Error reading from server: " << error.message() << std::endl;
        } else if (len2 > 0) {
            // Construct string from the exact number of bytes read
            std::string playerMsg2(buf2.data(), len2);

            // Optional: print the raw message for debugging
            // players = {};
            // Parse the message
            players = {};
            playerMsg2 = playerMsg2.substr(playerMsg2.find(',') + 1);
            std::istringstream iss(playerMsg2);
            // std::string firstWord;
            // Now parse the rest as CSV of players
            std::string token;
            std::cout << "oo player: " << playerMsg2;
            while (std::getline(iss, token1, ',')) {
            int id = std::stoi(token1);
            std::getline(iss, token1, ',');
            int x = std::stoi(token1);
            std::getline(iss, token1, ',');
            int y = std::stoi(token1);
            
            players.push_back({id, x, y});
            }
            // Now 'players' contains all initial player positions
            std::cout<<"vvvvv" << players.size();
            game1.updateAllPlayers(players);
            std::cout<<"firrrrrr" << (*game1.getAllPlayers()).size();
        }
        
        
        // std::cin.get();
            if (IsKeyDown(KEY_RIGHT)) {
                    std::string msg = "hello";
            };
            if (IsKeyDown(KEY_LEFT))  x -= speed * GetFrameTime();
            if (IsKeyDown(KEY_UP))    {
                game1.updatePlayer(playerID, 0, -5);
            };
            if (IsKeyDown(KEY_DOWN))  {
                game1.updatePlayer(playerID, 0, 5);
            };

            // Draw
            BeginDrawing();
            ClearBackground(RAYWHITE);
            game1.drawAllPlayers();
            // DrawCircle((int)x, (int)y, 30, BLUE);
            DrawText("Use arrow keys to move the circle!", 10, 10, 20, DARKGRAY);
            // std::string player_update{};
            // for (Player player: *game1.getAllPlayers()) {
            //         player_update = player_update + "," + std::to_string(player.id) + "," + std::to_string(player.x) + "," + std::to_string(player.y);
            // }
            // socket.write_some(boost::asio::buffer(player_update), send_err);
            EndDrawing();
        }

        CloseWindow();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}