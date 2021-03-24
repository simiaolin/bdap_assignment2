/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include "../lib/include/DecisionTree.hpp"

int main() {
  Dataset d;
  d.train.filename = "/Users/ary/CLionProjects/bdap_2/test/data/iris_test.arff";
  d.test.filename = "/Users/ary/CLionProjects/bdap_2/test/data/iris_test.arff";

  DataReader dr(d);
  DecisionTree dt(dr);
  dt.print();
  dt.test();
  return 0;
}
