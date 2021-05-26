#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_

#include "map.h"

typedef struct tournament_t *Tournament;
    
Tournament tournamentDataCreate(const char* location, int max_games_per_player);

/** copy Data function - type : Tournament*/
MapDataElement copyTournamentData(MapDataElement data);

/** free Data function - type : Tournament*/
void freeTournamentData(MapDataElement data);

bool isTournamentLocationValid(const char* tournament_location);

bool checkIfFinishedTournament(Map tournaments,int tournament_id);

int tournamentGetWinnerId(Tournament tournament);

int tournamentGetMaxGames(Tournament tournament);

Map tournamentGetGameMap(Tournament tournament);

void tournamentSetWinner(Tournament tournament, int winner_id);

const char* tournamentGetLocation( Tournament tournament);

int mapPlayerSize(Tournament tournament);

bool tournametPlayerExeededGames (Tournament tournament, int player1_id, int player2_id);

int tournamentGetRemovedPlayers(Tournament tournament);

void tournamentAddRemovedPlayers(Tournament tournament);

void tournamentSubRemovedPlayers(Tournament tournament);

bool tournamentIsGameExist(Map tournaments,int tournament_id,int first_player,int second_player);

#endif /* TOURNAMENT_H_ */