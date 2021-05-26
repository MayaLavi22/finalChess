#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "chessSystem.h"
#include "map.h"
#include "utilities.h"

struct game_t
{
    int first_player;
    int second_player;
    Winner winner;
    int play_time;
};


Game gameDataCreate(int first_player, int second_player, Winner winner, int play_time)
{
    if(play_time <= 0 || second_player<=0 || first_player <=0)
        return NULL;
    Game data = malloc(sizeof(*data));
    if(data == NULL)
        return NULL;
    
    data->first_player = first_player;
    data->second_player = second_player;
    data->winner = winner;
    data->play_time = play_time;

    return data;
}

MapDataElement copyGameData(MapDataElement data)
{
    if(data == NULL)
        return NULL;
    if(((Game)data)->play_time <= 0 || ((Game)data)->first_player<=0 || ((Game)data)->second_player<=0)
        return NULL;
    Game copy = malloc(sizeof(*copy));
    if(copy == NULL)
        return NULL;

    copy->first_player = ((Game)data)->first_player;
    copy->second_player = ((Game)data)->second_player;
    copy->play_time = ((Game)data)->play_time;
    copy->winner = ((Game)data)->winner;

    return copy;
}

void freeGameData(MapDataElement data)
{
    free(data);
    //all of GameData's fields don't need to be freed
}

int gameGetFirstPlayer(Game game){
    return game->first_player;
}
int gameGetSecondPlayer(Game game){
    return game->second_player;
}

void gameDefaultPlayer1Id(Game game){
    game->first_player=-1;
}
void gameDefaultPlayer2Id(Game game){
    game->second_player=-1;
}

Winner gameGetWinner(Game game)
{
    return game->winner;
}

void gameSetWinner(Game game, Winner winner)
{
    game->winner = winner;
}

bool playerWasInGame(Game game, int player_id)      
{
    if (game->first_player==player_id || game->second_player==player_id){
        return true;
    }
    return false;
}

int gameGetTime (Game game)
{
    return game->play_time;
}