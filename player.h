#ifndef PLAYER_H_
#define PLAYER_H_

#include "map.h"

typedef struct player_t *Player;

Player playerDataCreate();

MapDataElement copyPlayerData (MapDataElement data);

void freePlayerData(MapDataElement data);

int playerGetGamesWon(Player player);

void playerAddAGameWon(Player player);

int playerGetGamesLost(Player player);

void playerAddAGameLost(Player player);

int playerGetGamesWithDraw(Player player);

void playerAddAGameWithDraw(Player player);

int playerGetGamesPrticipated(Player player);

void playerAddAGamePrticipated(Player player);

void playerRemoveAGamePrticipated(Player player);

void playerRemoveDraw(Player player);

void playerRemoveLoss(Player player);

void playerRemoveWin(Player player);

void playerAddAGamePlayTime(Player player, int play_time);

int playerGetPlayTime(Player player);

double playerGetLevel(Player player);
void playerResetData(Player player);
#endif /* PLAYER_H_ */