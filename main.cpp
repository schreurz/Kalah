//
//  main.cpp
//  Kalah
//
//  Created by Zack Schreur on 12/11/16.
//  Copyright © 2016 Zack Schreur. All rights reserved.
//

#include <vector>
using std::vector;
#include <random>
using std::default_random_engine; using std::uniform_int_distribution;
#include <string>
using std::string;
#include <iostream>
using std::cout; using std::cin; using std::endl;
#include <fstream>
using std::ofstream;

#include "player.h"
#include "gameplay.h"

int main() {
  
  int boardSize, startValue;
  cout << "Pits: ";
  cin >> boardSize;
  cout << "Stones: ";
  cin >> startValue;
  
  // Create board: {4,4,4,4,4,4
  //                4,4,4,4,4,4}
  vector<int> board = create_board(boardSize, startValue);
  
  // Setup random
  int seed, loops;
  cout << "Random seed: ";
  cin >> seed;
  cout << "Multi-Play loops: ";
  cin >> loops;
  
  default_random_engine dre(seed);
  
  int p1_low = 0;
  int p1_high = boardSize-1;
  int p2_low = boardSize;
  int p2_high = (2*boardSize)-1;
  
  Player p1(p1_low, p1_high);
  Player p2(p2_low, p2_high);
  
  
  int winner;
  ofstream single_play_file;
  single_play_file.open("./single_play.txt");
  single_play_file << "Game start\nPlayer 1 plays the top row\n"
  "Player 2 plays the bottom row\n"
  "Game moves counter clockwise\n"
  "Top right house is index 0\n"
  "The two stores are the single numbers on the edge\n";
  for (int i=0; i<loops+1; i++) { // loop includes +1 for single play
    
    winner = play_game(board, p1, p2, dre, !i, single_play_file);
    if (i) {
      if (winner == 1) {
        p1.wins++;
      }
      else if (winner == 2){
        p2.wins++;
      }
      else {
        ++p1.ties;
        ++p2.ties;
      }
    }
    p1.store = 0;
    p2.store = 0;
    
    if (!i) {// After single play change seed to number of loops
      dre.seed(loops);
      p1.wins=0;
      p2.wins=0;
      p1.ties=0;
      p2.ties=0;
      p1.goes_first=0;
      p2.goes_first=0;
      p1.scores.clear();
      p2.scores.clear();
    }
  }
  single_play_file.close();
  
  // write data to file
  ofstream multi_play_file;
  multi_play_file.open("./multiple_play.txt");
  multi_play_file << "Multi-Play data\n";
  multi_play_file << "Rounds played: " << loops << endl;
  multi_play_file << "Game rules: (" << boardSize << ", " << startValue << ")\n";
  multi_play_file << "\nPlayer 1 (Amazing Strategy)\n";
  multi_play_file << "Goes first: " << p1.goes_first << endl;
  multi_play_file << "Wins: " << p1.wins << endl;
  multi_play_file << "Losses: " << loops - p1.wins - p1.ties << endl;
  multi_play_file << "Ties: " << p1.ties << endl;
  multi_play_file << "Win %: " << 100*(double)p1.wins/loops << endl;
  multi_play_file << "Average score: " << p1.get_average_score() << endl;
  multi_play_file << "Median score: " << p1.get_median_score() << endl;
  multi_play_file << "Highest score " << p1.get_highest_score() << endl;
  multi_play_file << "Lowest score " << p1.get_lowest_score() << endl;
  multi_play_file << "\nPlayer 2 (Simple Strategy)\n";
  multi_play_file << "Goes first: " << p2.goes_first << endl;
  multi_play_file << "Wins: " << p2.wins << endl;
  multi_play_file << "Losses: " << loops - p2.wins - p2.ties << endl;
  multi_play_file << "Ties: " << p2.ties << endl;
  multi_play_file << "Win %: " << 100*(double)p2.wins/loops << endl;
  multi_play_file << "Average score: " << p2.get_average_score() << endl;
  multi_play_file << "Median score: " << p2.get_median_score() << endl;
  multi_play_file << "Highest score: " << p2.get_highest_score() << endl;
  multi_play_file << "Lowest score: " <<p2.get_lowest_score() << endl;
  multi_play_file.close();
  
  return 0;
}
