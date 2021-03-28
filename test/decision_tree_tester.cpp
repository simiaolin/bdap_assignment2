/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include "../lib/include/DecisionTree.hpp"


int main(int argc, char** argv) {
  Dataset d;

  std::string dataset_name = argv[1];
  std::string path = "/Users/ary/CLionProjects/bdap_2/test/data/";
  d.train.filename =  path + dataset_name + ".arff";
  d.test.filename = path + dataset_name + "_test.arff";

  DataReader dr(d);
  DecisionTree dt(dr);
  dt.print();
  dt.test();
  return 0;
}
