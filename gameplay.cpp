//
//  gameplay.cpp
//  Kalah
//
//  Created by Zack Schreur on 12/11/16.
//  Copyright © 2016 Zack Schreur. All rights reserved.
//

#include <iostream>
using std::cout; using std::cin; using std::endl;
using std::right; using std::left;
#include <iomanip>
using std::setw; using std::setfill;
#include <sstream>
using std::ostringstream;
#include <vector>
using std::vector;
#include <random>
using std::default_random_engine; using std::uniform_int_distribution;
#include <string>
using std::string;
#include <fstream>
using std::ofstream;

#include "gameplay.hpp"
#include "player.h"

int wrapping_index(size_t size, int index) {
  return (index%size);
}

int move(vector<int> &board, int house_index, Player &p) {
  // Changes board based on which house is picked up
  // Updates player's store through execution
  // returns true when last stone is dropped in player's store
  // otherwise returns false
  
  // Value returned:
  int move_type = 0; // 0 = terminal no capture
  // 1 = terminal capture
  // 2 = non-terminal no capture
  
  int hand = board[house_index]; // rocks in player's hand
  board[house_index] = 0;        // remove rocks from house_index
  
  int low_index  = p.index_range[0];   // index of first house on player's board - inclusive
  int high_index = p.index_range[1]+1; // index of last house on player's board - exclusive
  
  // make moves until hand is empty
  
  // take from start house to store
  for (int index=house_index+1; index < high_index && hand > 0; ++index) {
    
    board[index]++;
    hand--;
    
    size_t opposite_index = board.size()-index-1;
    
    if ((board[index] == 1)          // seed placed in empty house
        && (hand == 0)                 // it is last seed placed
        && (board[opposite_index])) {  // opposite house has seeds in it
      
      p.store += 1 + board[opposite_index];
      board[index] = 0;
      board[opposite_index] = 0;
      move_type = 1;
    }
  }
  
  bool my_row=false;
  bool store =true;
  bool o_row =false;
  
  while(hand>0) {
    // Loop until hand is empty
    // Make move on player's row, player's store, or opponents row
    if (my_row) {
      for (int index=low_index; index<high_index && hand>0; index++) {
        board[index]++;
        hand--;
        
        size_t opposite_index = board.size()-index-1;
        
        if ((board[index] == 1)
            && (hand == 0)
            && (board[opposite_index])) {
          
          p.store += 1 + board[opposite_index];
          board[index] = 0;
          board[opposite_index] = 0;
          move_type = 1;
        }
        
      }
      my_row= false;
      store = true;
      o_row = false;
    }
    else if (store) {
      p.store++;
      hand--;
      my_row= false;
      store = false;
      o_row = true;
      if (!hand) {
        move_type = 2;
      }
    }
    else if (o_row) {
      
      int dif = high_index - low_index;
      
      for (int index = wrapping_index(board.size(), high_index);
           index < wrapping_index(board.size(), high_index) + dif && hand>0;
           index++) {
        
        board[index]++;
        hand--;
      }
      my_row=true;
      store = false;
      o_row = false;
    }
  }
  
  return move_type;
}

string board_to_string(vector<int> board, int store1=0, int store2=0) {
  // Return a string representation of the board
  ostringstream oss;
  
  oss << "  ";
  for (auto itr = board.rbegin() + board.size()/2; itr != board.rend(); itr++)
    oss << *itr << " ";
  oss << endl;
  
  oss << setw((int)board.size() + 1);
  oss << left << store1 << right << store2 << endl;
  
  oss << "  ";
  for (auto itr = board.begin() + board.size()/2; itr != board.end(); itr++)
    oss << *itr << " ";
  oss << endl;
  
  return oss.str();
}

int play_game(vector<int> board, Player &p1, Player &p2,
              default_random_engine &dre, bool single_play,
              ofstream& file) {
  
  bool game_over;
  int move_type; // 0 = 00 = terminal no capture
  // 1 = 01 = terminal capture
  // 2 = 10 = non-terminal no capture
  
  string description; // String used to represent descrption of move made
  int house;
  
  bool p1_first=true;
  if (!single_play) { // multi-play
    uniform_int_distribution<int> dist(0,1);
    p1_first = dist(dre);
  }
  
  // Player 2 plays before Player 1
  if (!p1_first) {
    ++p2.goes_first;
    do {
      house = p1.simple_strat(board, description);
      move_type = move(board, house, p2);
      game_over = (p2.check_houses(board));
    } while (move_type == 2 && !game_over);
  }
  else
    ++p1.goes_first;
  
  do {
    
    do {
      
      if (single_play) {
        file << endl << "---------------";
        file << endl << "Player 1's turn" << endl;
        file <<         "---------------" << endl;
        file << "Before move:\n";
        file << board_to_string(board, p1.store, p2.store);
        file << "Player 1: " << p1.store << "\nPlayer 2: " << p2.store << endl;
      }
      
      house = p1.super_strat(board, dre, description);
      move_type = move(board, house, p1);
      game_over = (p1.check_houses(board) || p2.check_houses(board));
      if (single_play) {
        file << "\nPlayer 1 took from house: " << house << endl;
        file << description << endl;
        if (move_type == 1) file << "Player 1 captured!" << endl;
        file << "\nAfter move:\n";
        file << board_to_string(board, p1.store, p2.store);
        file << "Player 1: " << p1.store << "\nPlayer 2: " << p2.store << endl;
        if(!game_over)
          file << endl <<
          ((move_type == 2) ?
           "Player 1 plays again\n" : "Player 1 does not play again\n");
      }
    } while (move_type == 2 && !game_over);
    
    if (game_over)
      break;
    
    do {
      if (single_play) {
        file << endl << "---------------";
        file << endl << "Player 2's turn" << endl;
        file <<         "---------------" << endl;
        file << "Before:\n";
        file << board_to_string(board, p1.store, p2.store);
        file << "Player 1: " << p1.store << "\nPlayer 2: " << p2.store << endl;
      }
      house = p2.simple_strat(board, description);
      move_type = move(board, house, p2);
      game_over = (p2.check_houses(board) || p1.check_houses(board));
      if (single_play) {
        file << "\nPlayer 2 took from house: " << house << endl;
        file << description << endl;
        if (move_type == 1) file << "Player 2 captured!" << endl;
        file << "\nAfter move:\n";
        file << board_to_string(board, p1.store, p2.store);
        file << "Player 1: " << p1.store << endl;
        file << "Player 2: " << p2.store << endl;
        if (!game_over)
          file << endl <<
          ((move_type == 2) ?
           "Player 2 plays again\n" : "Player 2 does not play again\n");
      }
    } while (move_type == 2 && !game_over);
    
  } while(!game_over);
  
  // move all rocks to appropriate store
  p1.clear_board(board);
  p2.clear_board(board);
  
  if (!single_play) {
    p1.scores.push_back(p1.store);
    p2.scores.push_back(p2.store);
  }
  
  if (single_play) {
    file << endl;
    file << "----------" << endl;
    file << "Game over!" << endl;
    file << endl << board_to_string(board, p1.store, p2.store);
    file << "Player 1: " << p1.store << endl;
    file << "Player 2: " << p2.store << endl;
    file << "----------" << endl;
    file << "\nPlayer " << ((p1.store > p2.store) ? 1 : 2 )<< " won!\n";
  }
  
  return (p1.store > p2.store) ? 1 : (p2.store > p1.store) ? 2 : 0;
}

vector<int> create_board(int size, int value) {
  return vector<int>(2*size,value);
}
