#ifndef GAME_HPP
#define GAME_HPP

#include "board.hpp"
#include "player.hpp"

enum GameMode{PVP, PVE, EVE};

class Game
{
    Board *board;
    Player players[2];
    GameMode gameMode;

    void setPlayers();

    public:
        Game();
        ~Game();
        void loopGame();
        void exitGame();
        void enterGame();
        void newBoard();
        bool hasOneWin(); 
        void setGameMode(GameMode gameMode);
};

#endif //GAME_HPP