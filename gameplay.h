//
//  gameplay.h
//  Kalah
//
//  Created by Zack Schreur on 12/11/16.
//  Copyright © 2016 Zack Schreur. All rights reserved.
//

#ifndef KALAH_GAMEPLAY_H
#define KALAH_GAMEPLAY_H

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <random>
using std::default_random_engine;
#include <fstream>
using std::ofstream;

#include "player.h"

inline int wrapping_index(size_t size, int index);
int move(vector<int> &board, int house_index, Player &p);
string board_to_string(vector<int> board, int store1=0, int store2=0);
int play_game(vector<int> board, Player &p1, Player &p2,
              default_random_engine &dre, bool single_play,
              ofstream& file);
vector<int> create_board(int size, int value);

#endif /* KALAH_GAMEPLAY_H */
