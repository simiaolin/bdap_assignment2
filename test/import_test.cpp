/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include <DecisionTree/DecisionTree.hpp>

int main(void) {
  Dataset d;
  d.train.filename = "../data/play_tennis.csv";
  d.test.filename = "../data/play_tennis_test.csv";

  DecisionTree dt(d);
  return 0;
}
