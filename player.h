//
//  player.h
//  Kalah
//
//  Created by Zack Schreur on 12/11/16.
//  Copyright © 2016 Zack Schreur. All rights reserved.
//

#ifndef player_h
#define player_h

#include <vector>
using std::vector;
#include <deque>
using std::deque;
#include <string>
using std::string;
#include <random>
using std::default_random_engine;

class Player {
  
public:
  int store=0;
  vector<int> index_range; // low and high index of houses; inclusive
  vector<int> scores;
  deque<int> og_move_sequence;
  deque<int> move_sequence;
  int wins = 0;
  int ties = 0;
  int goes_first = 0;
  
  Player(int start_index, int end_index): index_range{start_index, end_index}{};
  
  int random_move(vector<int>, default_random_engine&, string&);
  int simple_strat(vector<int>, string&);
  int super_strat(vector<int>, default_random_engine&, string&);
  bool check_houses(vector<int>);
  void clear_board(vector<int>&);
  
  // statistics
  int get_average_score();
  int get_median_score();
  int get_highest_score();
  int get_lowest_score();
};

bool at_last_sequence(deque<int>, int);
vector<int> execute_sequence(deque<int>, vector<int>, Player&);
deque<int> compare_ms(deque<int>, deque<int>, vector<int>, Player, default_random_engine&);
deque<int> get_best_ms(Player, vector<int>, default_random_engine&);

#endif /* player_h */
