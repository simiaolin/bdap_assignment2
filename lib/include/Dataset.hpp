/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_DATASET_HPP
#define DECISIONTREE_DATASET_HPP

#include <string>

/**
 * Holds the path of file containing data that is used during the train phase.
 */
struct Train {
  std::string filename;
};

/**
 * Holds the path of file containing data that is used during the test phase.
 */
struct Test {
  std::string filename;
};

/**
 * A data set consists out of two data files: one used to train a classifier,
 * the other used to validate the learned model. Additionaly, this struct
 * stores the label of the target column.
 */
struct Dataset {
  Train train;
  Test test;
  std::string classLabel;
};

#endif //DECISIONTREE_DATASET_HPP
