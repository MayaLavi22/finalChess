#ifndef GAME_H_
#define GAME_H_

 #include "chessSystem.h"
 #include "map.h"
 
typedef struct game_t *Game;
    
Game gameDataCreate(int first_player, int second_player, Winner winner, int play_time);

/** copy Data function - type : Game*/
MapDataElement copyGameData(MapDataElement data);

/** free Data function - type : Game*/
void freeGameData(MapDataElement data);

int gameGetFirstPlayer(Game game);

int gameGetSecondPlayer(Game game);

void gameDefaultPlayer1Id(Game game);

void gameDefaultPlayer2Id(Game game);

Winner gameGetWinner(Game game);

void gameSetWinner(Game game, Winner winner);

bool playerWasInGame(Game game, int player_id);

int gameGetTime (Game game);

#endif /* GAME_H_ */
