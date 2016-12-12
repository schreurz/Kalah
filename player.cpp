//
//  player.cpp
//  Kalah
//
//  Created by Zack Schreur on 12/11/16.
//  Copyright © 2016 Zack Schreur. All rights reserved.
//

#include <iostream>
using std::cout; using std::endl;
#include <sstream>
using std::ostringstream;
#include  <vector>
using std::vector;
#include <random>
using std::default_random_engine; using std::uniform_int_distribution;
#include <string>
using std::string;
#include "player.h"
#include "gameplay.h"

int Player::super_strat(vector<int> board, default_random_engine& dre, string& description) {
  
  if (this->move_sequence.empty()) {
    this->move_sequence = get_best_ms(*this, board, dre);
    this->og_move_sequence = this->move_sequence;
  }
  
  ostringstream oss;
  oss << "Execute move #" << og_move_sequence.size() - move_sequence.size() + 1<< " from sequence: ";
  for (auto m : this->og_move_sequence) {
    oss << m << ' ';
  }
  
  int index_to_return = this->move_sequence.front();
  this->move_sequence.pop_front();
  
  description = oss.str();
  
  return index_to_return;
  
}

int Player::random_move(vector<int> board, default_random_engine &dre, string& description){
  
  /*
   Returns a random index of a non-zero house
   if player houses = {4,0,4,4}
   then it returns a random value between 0 and 3
   the returned value will not be 1
   */
  
  if (this->check_houses(board)) {
    cout << "error: try to grab random index of empty board\n";
    return -1;
  }
  
  uniform_int_distribution<int> dist(index_range[0], index_range[1]);
  int i = dist(dre);
  while (board[i] == 0){
    i = dist(dre);
  }
  description = "Random move";
  
  return i;
}

int Player::simple_strat(vector<int> board, string& description) {
  
  int low = index_range[0];
  int high= index_range[1];
  
  for (int i=low; i<high+1; i++)
    // Rule for placing last stone in store
    // 3 + 4 = 6 + 1
    // 7 = 7
    if (board[i] + i == high + 1) {
      //cout << i << endl;
      description = "Move to place last stone in store";
      return i;
    }
  
  for (int i=low; i<=high; ++i)
    if (board[(high + low) - i] > 0) {
      description = "Move closest legal pit to store";
      return ((high + low) - i);
    }
  
  cout << "error: try to make a move on empty board\n";
  return -1;
}

bool Player::check_houses(vector<int> board) {
  // returns false if there are rocks in player's houses
  // returning true means game over
  
  for(int i=index_range[0]; i<=index_range[1]; i++)
    if (board[i])
      return false;
  return true;
}

void Player::clear_board(vector<int> &board) {
  // moves all rocks on player's side to store
  for (int i=index_range[0]; i<=index_range[1];i++) {
    this->store += board[i];
    board[i] = 0;
  }
}

int Player::get_average_score() {
  double ave = 0.0;
  for(auto itr=this->scores.begin(); itr!=this->scores.end(); itr++) {
    ave += *itr;
  }
  return ( ave / ((int)this->scores.size()) );
}

int Player::get_median_score() {
  sort (this->scores.begin(), this->scores.end());
  return (this->scores[((int)this->scores.size())/2]);
}

int Player::get_lowest_score() {
  sort (this->scores.begin(), this->scores.end());
  return (this->scores.front());
}

int Player::get_highest_score() {
  sort (this->scores.begin(), this->scores.end());
  return (this->scores.back());
}

bool at_last_sequence(deque<int> ms, int high_index) {
  for (auto move : ms)
    if (move != high_index)
      return false;
  return true;
}

vector<int> execute_sequence(deque<int> move_sequence, vector<int> board, Player& p) {
  for (auto i : move_sequence) {
    move(board, i, p);
  }
  return board;
}

deque<int> compare_ms(deque<int> ms1, deque<int> ms2, vector<int> board, Player p, default_random_engine& dre) {
  
  // 2.5 * my store change
  // 1.0 * count my side change
  // (-1.0) * count opponent side change
  // (-1.0) * count opponent empty
  // +4.0 last index open
  
  vector<int> board_1, board_2;
  
  Player p_1 = p;
  Player p_2 = p;
  
  size_t my_low, my_high, opp_low, opp_high; // inclusive
  my_low   = p.index_range[0];
  my_high  = p.index_range[1];
  opp_low  = (p.index_range[1]+1)%board.size();
  opp_high = opp_low+(my_high-my_low);
  
  board_1 = execute_sequence(ms1, board, p_1); // creates new board after ms1
  board_2 = execute_sequence(ms2, board, p_2); // creates new board after ms2
  
  size_t my_side_init, opp_side_init, my_side_1,
  opp_side_1, my_side_2, opp_side_2,
  opp_empty_1, opp_empty_2;
  
  my_side_init = count_if(board.begin()+my_low,
                          board.begin()+my_high+1,
                          [] (int i) {return (bool) i;});
  
  opp_side_init= count_if(board.begin()+opp_low,
                          board.begin()+opp_high+1,
                          [] (int i) {return (bool) i;});
  
  my_side_1 = count_if(board_1.begin()+my_low,
                       board_1.begin()+my_high+1,
                       [] (int i) {return (bool) i;});
  
  opp_side_1= count_if(board_1.begin()+opp_low,
                       board_1.begin()+opp_high+1,
                       [] (int i) {return (bool) i;});
  
  my_side_2 = count_if(board_2.begin()+my_low,
                       board_2.begin()+my_high+1,
                       [] (int i) {return (bool) i;});
  
  opp_side_2= count_if(board_2.begin()+opp_low,
                       board_2.begin()+opp_high+1,
                       [] (int i) {return (bool) i;});
  
  opp_empty_1 = count_if(board_1.begin()+opp_low,
                         board_1.begin()+opp_high+1,
                         [] (int i) {return !((bool) i);});
  
  opp_empty_2 = count_if(board_2.begin()+opp_low,
                         board_2.begin()+opp_high+1,
                         [] (int i) {return !((bool) i);});
  
  // Calculate move sequence score
  double ms1_score, ms2_score;
  
  ms1_score = 2.5 * (p_1.store - p.store);
  ms2_score = 2.5 * (p_2.store - p.store);
  
  
  ms1_score += 1.0 * (my_side_1-my_side_init);
  ms2_score += 1.0 * (my_side_2-my_side_init);
  
  ms1_score -= 1.0 * (opp_side_1-opp_side_init);
  ms2_score -= 1.0 * (opp_side_2-opp_side_init);
  
  ms1_score += (!board_1[my_high]) ? 20.0 : 0.0;
  ms2_score += (!board_2[my_high]) ? 20.0 : 0.0;
  
  ms1_score -= 1.0 * opp_empty_1;
  ms2_score -= 1.0 * opp_empty_2;
  
  
  // Return the move sequence with the highest score
  // If score tied, return at random
  uniform_int_distribution<int> dist(0,1);
  if (ms1_score > ms2_score) {
    return ms1;
  } else if (ms2_score > ms1_score)
    return ms2;
  else {
    return dist(dre) ? ms1 : ms2;
  }
}

deque<int> get_best_ms(Player p, vector<int> board, default_random_engine& dre) {
  // board is stored as starting for new sequence checks
  deque<int> best_ms, temp_ms;
  
  vector<vector<int>> board_states;
  vector<int> temp_board;
  
  board_states.push_back(board);
  
  int move_made;
  
  int low = p.index_range[0];
  int high= p.index_range[1];
  
  
  for (int i=low; i<high+1;) { // i=2
    
    temp_board = board_states.back();
    if (!temp_board[i]) {
      ++i;
      continue;
    }
    
    temp_ms.push_back(i);
    
    move_made = move(temp_board, i, p);
    
    board_states.push_back(temp_board);
    
    if (move_made != 2) {
      
      // set new best ms
      if (best_ms.empty()) {
        best_ms = temp_ms;
      }
      
      else {
        best_ms = compare_ms(best_ms, temp_ms, temp_board, p, dre);
      }
      
      if (temp_ms.back() == high) {
        
        if (at_last_sequence(temp_ms, high)) {
          break;
        }
        temp_ms.pop_back();
        i = temp_ms.back();
        board_states.pop_back();
      }
      board_states.pop_back();
      temp_ms.pop_back();
      
      ++i;
    }
    else { // non-terminal move
      i = low ;
    }
  }
  
  if (!best_ms.size()) {
    // just error check
    // random move if didn't work
    string s;
    best_ms.push_back(p.random_move(board, dre, s));
  }
  return best_ms;
}

