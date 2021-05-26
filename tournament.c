#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tournament.h"
#include "game.h"
#include "map.h"
#include "utilities.h"

struct tournament_t
{
    const char* location;
    Map game_map;
    int winner_id; 
    int max_games_per_player;
    int removed_players;
};

Tournament tournamentDataCreate(const char* location, int max_games_per_player)
{
    if(location==NULL || max_games_per_player <= 0){
        return NULL;
    }
    Tournament data = malloc(sizeof(*data));
    if(data == NULL){
        return NULL;
    }
    data->location = location;
    data->max_games_per_player = max_games_per_player;
    data->game_map = mapCreate(copyGameData, copyId, freeGameData, freeId, compareIds);
    if(data->game_map==NULL){
        return NULL;
    }
    data->winner_id = -1; //default
    data->removed_players=0;
    return data;
}

MapDataElement copyTournamentData(MapDataElement data)
{
    if(data == NULL){
        return NULL;
    }
    
    if(((Tournament)data)->location == NULL || ((Tournament)data)->game_map == NULL || 
        ((Tournament)data)->max_games_per_player <=0  ||((Tournament)data)->max_games_per_player <0/*|| ((Tournament)data)->player_history == NULL*/){
        return NULL;
    }  
    Tournament copy=malloc(sizeof(*copy));
    if(copy == NULL){
        return NULL;
    }
    copy->location = ((Tournament)data)->location;
    copy->max_games_per_player = ((Tournament)data)->max_games_per_player;
    copy->game_map = mapCopy(((Tournament)data)->game_map);
    if(copy->game_map==NULL){
        return NULL;
    }
    copy->winner_id = ((Tournament)data)->winner_id;
    copy->removed_players=((Tournament)data)->removed_players;
    return copy;
}

void freeTournamentData(MapDataElement data)
{
    mapDestroy(((Tournament)data)->game_map);
    free(data);
}

bool isTournamentLocationValid(const char* tournament_location)
{
    if (tournament_location[0]<'A'||tournament_location[0]>'Z'){
        return false;
    }
    const char* ptr=tournament_location;
    ptr++;
    while (*ptr){
        if ((*ptr<'a'|| *ptr>'z') && *ptr!=' '){
            return false;
        }
        ptr++; 
    }
    return true;
}

int tournamentGetWinnerId(Tournament tournament)
{
    return tournament->winner_id;
}

int tournamentGetMaxGames(Tournament tournament)
{
    return tournament->max_games_per_player;
}

Map tournamentGetGameMap(Tournament tournament)
{
    return tournament->game_map;
}

int tournamentGetRemovedPlayers(Tournament tournament)
{
    return tournament->removed_players;
}
void tournamentAddRemovedPlayers(Tournament tournament)
{
    tournament->removed_players+=1;
}

void tournamentSubRemovedPlayers(Tournament tournament)
{
    tournament->removed_players-=1;
}

bool checkIfFinishedTournament(Map tournaments,int tournament_id)
{
    Tournament tournament=mapGet(tournaments,&tournament_id);
    if (tournament->winner_id==-1){
        return false;
    }
    return true;
}

void tournamentSetWinner(Tournament tournament, int winner_id)
{
    tournament->winner_id=winner_id;
}

const char* tournamentGetLocation( Tournament tournament)
{
    return tournament->location;
}

bool tournametPlayerExeededGames (Tournament tournament, int player1_id, int player2_id)
{
    int max_p1=0;
    int max_p2=0;
    MAP_FOREACH(MapKeyElement,game_key,tournament->game_map){
        Game g_data=mapGet(tournament->game_map,game_key);
        int p_id1=gameGetFirstPlayer(g_data);
        int p_id2=gameGetSecondPlayer(g_data);
        if(player1_id==p_id1||player1_id==p_id2){
            max_p1++;
        }
        if(player2_id==p_id1||player2_id==p_id2){
            max_p2++;
        }
        free(game_key);
    }
    if(max_p1>=tournament->max_games_per_player||max_p2>=tournament->max_games_per_player){
        return true;
    }
    return false;
}

bool tournamentIsGameExist(Map tournaments,int tournament_id,int first_player,int second_player)
{
    Tournament tournament=mapGet(tournaments,&tournament_id);
    Map games=tournamentGetGameMap(tournament);
    MAP_FOREACH(MapKeyElement,game_key,games){
        Game data=mapGet(games,game_key);
        if((gameGetFirstPlayer(data)==first_player && gameGetSecondPlayer(data) == second_player) ||
            (gameGetFirstPlayer(data)==second_player && gameGetSecondPlayer(data)==first_player)){
                free(game_key);
                return true;
        }
        free(game_key);
    }
    return false;
}