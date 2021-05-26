#include "player.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

struct player_t
{
    int games_won;
    int games_lost;
    int games_with_draw;
    int games_participated;
    int games_play_time;
};

Player playerDataCreate(){
    Player player=malloc(sizeof(*player));
    if(player == NULL){
        return NULL;
    }
    player->games_won = 0;
    player->games_lost = 0;
    player->games_with_draw = 0;
    player->games_participated = 0;
    player->games_play_time = 0;
    return player;
}
MapDataElement copyPlayerData (MapDataElement data)
{
    if(data==NULL){
        return NULL;
    }
    Player copy = malloc(sizeof(*copy));
    if(copy == NULL){
        return NULL;
    }
    copy->games_won = ((Player)data)->games_won;
    copy->games_lost = ((Player)data)->games_lost;
    copy->games_with_draw = ((Player)data)->games_with_draw;
    copy->games_participated = ((Player)data)->games_participated;
    copy->games_play_time=((Player)data)->games_play_time;
    return copy;
}

void freePlayerData(MapDataElement data)
{
    free(data);
}

int playerGetGamesWon(Player player)
{
    return player->games_won;
}

void playerAddAGameWon(Player player)
{
    player->games_won = player->games_won + 1;
}

int playerGetGamesLost(Player player)
{
    return player->games_lost;
}

void playerAddAGameLost(Player player)
{
    player->games_lost = player->games_lost + 1;
}

int playerGetGamesWithDraw(Player player)
{
    return player->games_with_draw;
}

void playerAddAGameWithDraw(Player player)
{
    player->games_with_draw = player->games_with_draw + 1;
}

int playerGetGamesPrticipated(Player player)
{
    return player->games_participated;
}

void playerAddAGamePrticipated(Player player)
{
    player->games_participated = player->games_participated + 1;
}
void playerAddAGamePlayTime(Player player, int play_time)
{
    player->games_play_time+=play_time;
}
void playerRemoveAGamePrticipated(Player player)
{
    player->games_participated = player->games_participated -1;
}

void playerRemoveDraw(Player player)
{
    player->games_with_draw = player->games_with_draw - 1;
}

void playerRemoveLoss(Player player)
{
    player->games_lost = player->games_lost - 1;
}

void playerRemoveWin(Player player)
{
    player->games_won = player->games_won - 1;
}

int playerGetPlayTime(Player player)
{
    return player->games_play_time;
}

double playerGetLevel(Player player)
{
    double level=(double)(6*player->games_won-10*player->games_lost+2*player->games_with_draw)/(player->games_participated);
    return level;
}

void playerResetData(Player player)
{
    player->games_participated=-1;
}
