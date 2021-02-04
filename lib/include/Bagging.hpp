/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_BAGGING_HPP
#define DECISIONTREE_BAGGING_HPP

#include <random>
#include <boost/chrono.hpp>
#include "Dataset.hpp"
#include "DecisionTree.hpp"
#include "Calculations.hpp"
#include "DataReader.hpp"
#include "TreeTest.hpp"

class Bagging {
  public:
    Bagging() = delete;
    explicit Bagging(const DataReader& dr, const int ensembleSize, uint seed = 1234);

    void test() const;

    inline Data testData() { return dr_.testData(); }

  private:
    DataReader dr_;
    int ensembleSize_;
    std::vector<DecisionTree> learners_;
    std::mt19937_64 random_number_generator;

    void buildBag();
};

#endif //DECISIONTREE_BAGGING_HPP
