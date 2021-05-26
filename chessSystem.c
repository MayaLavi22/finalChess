#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include "chessSystem.h"
#include "map.h"
#include "tournament.h"
#include "game.h"
#include "player.h"
#include "utilities.h"

struct chess_system_t
{
    Map tournaments;
    Map players;
};

static void updateRemovedPlayers(Map tournaments,int player_id);
static int numOfPlayersInTournament(Map games, Map players);
static Player getPlayerDataById(ChessSystem chess, int player_id);
static MapResult addPlayers(ChessSystem chess, int first_player, int second_player, Winner winner, int play_time);
static void updatePlayerFieldsTRemoved(ChessSystem chess, int tournament_id);
static int tournamentWinnerId(ChessSystem chess, Map games);
static void calculatePlayerPoints(int* wins_ptr, int* losses_ptr, int* draws_ptr, int* score_ptr, Winner result, int player_num);
static bool allTournamentsGoing(ChessSystem chess);
static void maxLevelPlayerId(Map player_copy,int* player_ptr,double* level_ptr);
static int maxGameTime (Map game);
static double avgGameTime (Map game);
static void updatePlayerFieldsPRemoved (ChessSystem chess, Game game,int player_num);

ChessSystem chessCreate()
{
    ChessSystem chess = malloc(sizeof(*chess));
    if(chess==NULL){
        return NULL;
    }
    chess->tournaments=mapCreate(copyTournamentData,copyId,freeTournamentData,freeId,compareIds);
    chess->players=mapCreate(copyPlayerData,copyId,freePlayerData,freeId,compareIds);
    //failure in creating the maps
    if (chess->players==NULL || chess->tournaments==NULL){ 
        chessDestroy(chess);
        return NULL;
    }
    return chess;
}

void chessDestroy(ChessSystem chess)
{
    if (chess==NULL){
        return;
    }
    mapDestroy(chess->players);
    mapDestroy(chess->tournaments);
    free(chess);
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id, int max_games_per_player, const char* tournament_location)
{
    if(chess==NULL||tournament_location==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id<=0){
        return CHESS_INVALID_ID;
    }
    if(mapContains(chess->tournaments,&tournament_id)){
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if (!isTournamentLocationValid(tournament_location)){
        return CHESS_INVALID_LOCATION;
    }
    if (max_games_per_player<=0){
        return CHESS_INVALID_MAX_GAMES;
    }
    Tournament tournamentData= tournamentDataCreate(tournament_location, max_games_per_player);
    if(tournamentData==NULL){
        return CHESS_OUT_OF_MEMORY;
    }
    MapResult result= mapPut(chess->tournaments, &tournament_id, tournamentData);
    if(result!=MAP_SUCCESS){
        freeTournamentData(tournamentData);
        return CHESS_OUT_OF_MEMORY;
    }
    freeTournamentData(tournamentData);
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner, int play_time)
{
    if(chess==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id<=0 || first_player<=0 || second_player<=0 ||first_player==second_player){
        return CHESS_INVALID_ID;
    }
    if (winner!=FIRST_PLAYER && winner!=SECOND_PLAYER &&winner!=DRAW){
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments,&tournament_id)){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament tournament=mapGet(chess->tournaments,&tournament_id);
    if(checkIfFinishedTournament(chess->tournaments, tournament_id)){
        return CHESS_TOURNAMENT_ENDED;
    }
    if (tournamentIsGameExist(chess->tournaments,tournament_id,first_player,second_player)){
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time<0){
        return CHESS_INVALID_PLAY_TIME;
    }
    if (tournametPlayerExeededGames(tournament,first_player,second_player)){
        return CHESS_EXCEEDED_GAMES;
    }
    Game gameData=gameDataCreate(first_player,second_player,winner,play_time);
    int game_key=mapGetSize(tournamentGetGameMap(tournament))+1;
    MapResult result= mapPut(tournamentGetGameMap(tournament),&game_key,gameData);
    if(result!=MAP_SUCCESS){
        freeGameData(gameData);
        return CHESS_OUT_OF_MEMORY;
    }
    result=addPlayers(chess, first_player, second_player, winner ,play_time);
    if(result!=MAP_SUCCESS){
        freeGameData(gameData);
        return CHESS_OUT_OF_MEMORY;
    }
    freeGameData(gameData);
    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    if (chess==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id<=0){
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments,&tournament_id)){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    updatePlayerFieldsTRemoved(chess, tournament_id);
    mapRemove(chess->tournaments,&tournament_id);
    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if(chess==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if(player_id<=0){
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->players,&player_id)){
        return CHESS_PLAYER_NOT_EXIST;
    }
    MAP_FOREACH(MapKeyElement,tournament_key,chess->tournaments){
        Tournament t_data=mapGet(chess->tournaments, tournament_key);
        bool remove=false;
        Map games=tournamentGetGameMap(t_data);
        MAP_FOREACH(MapKeyElement,game_key,games){
            Game g_data=mapGet(games,game_key);
            if (!checkIfFinishedTournament(chess->tournaments,*(int*)tournament_key)){
                if(player_id==gameGetFirstPlayer(g_data)){
                    gameDefaultPlayer1Id(g_data);
                    remove=true;
                    updatePlayerFieldsPRemoved(chess,g_data,1);
                }
                else if (player_id==gameGetSecondPlayer(g_data)){
                    gameDefaultPlayer2Id(g_data);
                    remove=true;
                    updatePlayerFieldsPRemoved(chess,g_data,2);
                }
            }
            else{
                if(player_id==gameGetFirstPlayer(g_data)){
                    remove=true;
                }
                else if (player_id==gameGetSecondPlayer(g_data)){
                    remove=true;
                }
            }
            free(game_key);
        }
        if(remove){
            tournamentAddRemovedPlayers(t_data);
        }
        free(tournament_key);
    }
    mapRemove(chess->players,&player_id);
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if (chess==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(!mapContains(chess->tournaments, &tournament_id)){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if(checkIfFinishedTournament(chess->tournaments, tournament_id)){
        return CHESS_TOURNAMENT_ENDED;
    }
    Map games=tournamentGetGameMap(mapGet(chess->tournaments,&tournament_id));
    if (mapGetSize(games)==0){
        return CHESS_NO_GAMES;
    }
    int winner_id=tournamentWinnerId(chess,games);
    tournamentSetWinner(mapGet(chess->tournaments,&tournament_id), winner_id);
    return CHESS_SUCCESS;
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if(!chess){
        *chess_result=CHESS_NULL_ARGUMENT;
        return -1;
    }
    if(player_id<=0){
        *chess_result= CHESS_INVALID_ID;
        return -1;
    }
    if(!mapContains(chess->players,&player_id)){
        *chess_result= CHESS_PLAYER_NOT_EXIST;
        return -1;
    }
    Player p_data=mapGet(chess->players,&player_id);
    int games_participated=playerGetGamesPrticipated(p_data);
    int games_total_time=playerGetPlayTime(p_data);
    double avg= (double)games_total_time/games_participated;
    *chess_result= CHESS_SUCCESS;
    return avg;
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    if(file==NULL|| chess==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    Map player_copy=mapCopy(chess->players);
    Map copy_player=mapCopy(chess->players);
    if (player_copy==NULL || copy_player==NULL){
        mapDestroy(player_copy);
        mapDestroy(copy_player);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(MapKeyElement,player_key,player_copy){
        int player_id=-1;
        double max_level=-100;
        int* player_ptr=&player_id;
        double* level_ptr=&max_level;
        maxLevelPlayerId(copy_player,player_ptr,level_ptr);
        mapRemove(copy_player,player_ptr);
        fprintf(file,"%d %.2f\n",*player_ptr,*level_ptr);
        free(player_key);
    }
    mapDestroy(player_copy);
    mapDestroy(copy_player);
    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    if(chess==NULL||path_file==NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if(allTournamentsGoing(chess)){
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    FILE* file = fopen(path_file, "w");
    if(file==NULL){
        return CHESS_SAVE_FAILURE;
    }
    MAP_FOREACH(MapKeyElement,tournament_key,chess->tournaments){
        Tournament tournament=mapGet(chess->tournaments,tournament_key);
        if (checkIfFinishedTournament(chess->tournaments,*(int*)tournament_key)){
            Map games=tournamentGetGameMap(tournament);
            int winner= tournamentGetWinnerId(tournament);
            int longest_time=maxGameTime(games);
            double avg_time=avgGameTime(games);
            const char* location=tournamentGetLocation(tournament);
            int num_games=mapGetSize(games);
            int num_players=tournamentGetRemovedPlayers(tournament)+numOfPlayersInTournament(games, chess->players);
            fprintf(file,"%d\n%d\n%.2f\n%s\n%d\n%d\n",winner,longest_time,avg_time,location,num_games,num_players);
        }
        free(tournament_key);
    }
    if(fclose(file) == EOF){
        return CHESS_SAVE_FAILURE;
    }
    return CHESS_SUCCESS;
}

/* ---------------------------------------STATIC FUNCTIONS--------------------------------------*/
static void updatePlayerFieldsPRemoved(ChessSystem chess, Game game, int player_num)
{
    if(player_num==1){
        if(gameGetWinner(game)==FIRST_PLAYER)
        {
            gameSetWinner(game, SECOND_PLAYER);
            int player2_id=gameGetSecondPlayer(game);
            playerAddAGameWon(mapGet(chess->players,&player2_id));
            playerRemoveLoss(mapGet(chess->players,&player2_id));
        }
        if(gameGetWinner(game) == DRAW)
        {
            gameSetWinner(game, SECOND_PLAYER);
            int player2_id=gameGetSecondPlayer(game);
            playerAddAGameWon(mapGet(chess->players,&player2_id));
            playerRemoveDraw(mapGet(chess->players,&player2_id));
        }
    }
    if(player_num==2){
        if(gameGetWinner(game)==SECOND_PLAYER)
        {
            gameSetWinner(game, FIRST_PLAYER);
            int player1_id=gameGetFirstPlayer(game);
            playerAddAGameWon(mapGet(chess->players,&player1_id));
            playerRemoveLoss(mapGet(chess->players,&player1_id));
        }
        if(gameGetWinner(game) == DRAW)
        {
            gameSetWinner(game, FIRST_PLAYER);
            int player1_id=gameGetFirstPlayer(game);
            playerAddAGameWon(mapGet(chess->players,&player1_id));
            playerRemoveDraw(mapGet(chess->players,&player1_id));
        }
    }
}

static MapResult addPlayers(ChessSystem chess, int first_player, int second_player, Winner winner, int play_time)
{
    Player player=playerDataCreate();
    bool contained1=true;
    bool contained2=true;
    MapResult result1=MAP_SUCCESS,result2=MAP_SUCCESS;
    if (!mapContains(chess->players,&first_player)){
        result1= mapPut(chess->players,&first_player,player);
        contained1=false;
    }
    if(!mapContains(chess->players,&second_player)){
        contained2=false;
        result2=mapPut(chess->players,&second_player,player);
    }
    if(result1!=MAP_SUCCESS || result2!=MAP_SUCCESS){
        freePlayerData(player);
        return MAP_OUT_OF_MEMORY;
    }
    if(!contained1){
        updateRemovedPlayers(chess->tournaments,first_player);
    }
    if(!contained2){
        updateRemovedPlayers(chess->tournaments,second_player);
    }
    if (winner==FIRST_PLAYER){
        playerAddAGameWon(getPlayerDataById(chess,first_player));
        playerAddAGameLost(getPlayerDataById(chess,second_player));
    }
    if (winner==SECOND_PLAYER){
        playerAddAGameLost(getPlayerDataById(chess, first_player));
        playerAddAGameWon(getPlayerDataById(chess, second_player));
    } 
    if(winner==DRAW){
        playerAddAGameWithDraw(getPlayerDataById(chess, first_player));
        playerAddAGameWithDraw(getPlayerDataById(chess, second_player));
    }
    playerAddAGamePrticipated(getPlayerDataById(chess, first_player));
    playerAddAGamePrticipated(getPlayerDataById(chess, second_player));
    playerAddAGamePlayTime(getPlayerDataById(chess, first_player),play_time);
    playerAddAGamePlayTime(getPlayerDataById(chess, second_player),play_time);
    freePlayerData(player);
    return MAP_SUCCESS;
}

static void updatePlayerFieldsTRemoved(ChessSystem chess, int tournament_id)
{
    Tournament t_data=mapGet(chess->tournaments, &tournament_id);
    Map games=tournamentGetGameMap(t_data);
    MAP_FOREACH(MapKeyElement,game_key,games){
        Game g_data = mapGet(games, game_key);
        if(gameGetWinner(g_data) == FIRST_PLAYER){
            playerRemoveWin(getPlayerDataById(chess, gameGetFirstPlayer(g_data)));
            playerRemoveLoss(getPlayerDataById(chess, gameGetSecondPlayer(g_data)));
        }
        else if(gameGetWinner(g_data) == SECOND_PLAYER){
            playerRemoveWin(getPlayerDataById(chess, gameGetSecondPlayer(g_data)));
            playerRemoveLoss(getPlayerDataById(chess, gameGetFirstPlayer(g_data)));
        }
        else if(gameGetWinner(g_data) == DRAW){
            playerRemoveDraw(getPlayerDataById(chess, gameGetSecondPlayer(g_data)));
            playerRemoveDraw(getPlayerDataById(chess, gameGetFirstPlayer(g_data)));
        }
        playerRemoveAGamePrticipated(getPlayerDataById(chess, gameGetFirstPlayer(g_data)));
        playerRemoveAGamePrticipated(getPlayerDataById(chess, gameGetSecondPlayer(g_data)));
        free(game_key);
    }
}

static Player getPlayerDataById(ChessSystem chess, int player_id)
{
   return mapGet(chess->players, &player_id);
}

static int tournamentWinnerId(ChessSystem chess, Map games)
{
    int max_score = 0;
    int max_wins=0; 
    int min_loss=mapGetSize(games)+1; //Can't reach this number of losses
    int curr_winner = -1;
    int wins=0, losses=0, draws=0, score=0;
    int *wins_ptr=&wins, *losses_ptr=&losses, *draws_ptr=&draws, *score_ptr=&score; 
    MAP_FOREACH(MapKeyElement,player_key,chess->players){
        int curr_id=*(int*)player_key;
        *wins_ptr=0; *losses_ptr=0; *draws_ptr=0; *score_ptr=0;
        MAP_FOREACH(MapKeyElement,game_key,games){
            Game g_data=mapGet(games,game_key);
            if (playerWasInGame(g_data,curr_id)){
                if(gameGetFirstPlayer(g_data)==curr_id){
                calculatePlayerPoints(wins_ptr, losses_ptr, draws_ptr, score_ptr, gameGetWinner(g_data), 1);
            }
            else {
                calculatePlayerPoints(wins_ptr, losses_ptr, draws_ptr, score_ptr, gameGetWinner(g_data), 2);
            }
            }
            free(game_key);
        }
        if(*score_ptr>max_score){
            curr_winner=curr_id;
        }
        else if (*score_ptr==max_score){
            if(*losses_ptr < min_loss){
                curr_winner=curr_id;
            }
            else if(*losses_ptr==min_loss){
                if(*wins_ptr > max_wins){
                    curr_winner=curr_id;
                }
                else if (curr_winner>curr_id){
                    curr_winner=curr_id;
                }
            }
        }
        if(curr_winner==curr_id)
        {
            max_score=*score_ptr;
            max_wins=*wins_ptr;
            min_loss=*losses_ptr;
        }
        free(player_key);
    }
    return curr_winner;
}

static void calculatePlayerPoints(int* wins_ptr, int* losses_ptr, int* draws_ptr, int* score_ptr, Winner result, int player_num)
{
    if(result == DRAW){
        (*draws_ptr)++;
    }
    else if((result == FIRST_PLAYER && player_num == 1) || (result == SECOND_PLAYER && player_num == 2)){
        (*wins_ptr)++;
    }
    else{
        (*losses_ptr)++;
    }
    (*score_ptr) = 2*(*wins_ptr) + (*draws_ptr);
}

static void maxLevelPlayerId(Map players, int* player_ptr, double* level_ptr)
{
    MAP_FOREACH(MapKeyElement,player_key,players){
        int curr_id=*(int*)player_key;
        double curr_level=playerGetLevel(mapGet(players,player_key));
        if(curr_level>*level_ptr){
            *player_ptr=curr_id;
            *level_ptr=curr_level;
        }
        free(player_key);
    }
    
}

static int maxGameTime (Map game)
{
    double max_time=0;
    MAP_FOREACH (MapKeyElement,game_key,game){
        int curr_time=gameGetTime(mapGet(game,game_key));
        if (curr_time>max_time){
            max_time=curr_time;
        }
        free(game_key);
    }
    return max_time;
}

static double avgGameTime (Map game)
{
    int sum=0;
    int g_size=mapGetSize(game);
    if (g_size==0){
        return sum;
    }
    MAP_FOREACH (MapKeyElement,game_key,game){
        sum+=gameGetTime(mapGet(game,game_key));
        free(game_key);
    }
    double avg=(double)sum/g_size;
    return avg;
}

static bool allTournamentsGoing(ChessSystem chess)
{
    MAP_FOREACH(MapKeyElement,tournament_key,chess->tournaments){
        int tournament_id=*(int*)tournament_key;
        if (checkIfFinishedTournament(chess->tournaments,tournament_id)){
            free(tournament_key);
            return false;
        }
        free(tournament_key);
    }
    return true;
}

static int numOfPlayersInTournament(Map games, Map players)
{
    int count = 0;
    bool counted=false;
    MAP_FOREACH(MapKeyElement, player_key, players){
        int player_id = *(int*)player_key;
        MAP_FOREACH(MapKeyElement, game_key, games){
            Game g_data = mapGet(games, game_key);
            if(gameGetFirstPlayer(g_data)==player_id || gameGetSecondPlayer(g_data)==player_id){
                counted = true;
            }
            free(game_key);
        }
        if(counted){
            count++;
        }
        free(player_key);
    }
    return count;
}

static void updateRemovedPlayers(Map tournaments,int player_id)
{
    MAP_FOREACH(MapKeyElement,tournament_key,tournaments){
        Tournament tournament=mapGet(tournaments,tournament_key);
        if(checkIfFinishedTournament(tournaments,*(int*)tournament_key)){
            bool remove=false;
            Map games=tournamentGetGameMap(tournament);
            MAP_FOREACH(MapKeyElement,game_key,games){
                Game g_data=mapGet(games,game_key);
                if (player_id==gameGetFirstPlayer(g_data)||player_id==gameGetSecondPlayer(g_data)){
                    remove=true;
                }
                free(game_key);
            }
            if(remove){
                tournamentSubRemovedPlayers(tournament);
            }
        }
        free(tournament_key);
    }
}

