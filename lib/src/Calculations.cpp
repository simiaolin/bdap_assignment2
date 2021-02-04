/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include <cmath>
#include <algorithm>
#include <iterator>
#include "Calculations.hpp"
#include "Utils.hpp"

using std::tuple;
using std::pair;
using std::forward_as_tuple;
using std::vector;
using std::string;
using std::unordered_map;

tuple<const Data, const Data> Calculations::partition(const Data& data, const Question& q) {
  Data true_rows;
  Data false_rows;
  
  for (const auto &row: data) {
    if (q.solve(row))
      true_rows.push_back(row);
    else
      false_rows.push_back(row);
  }

  return forward_as_tuple(true_rows, false_rows);
}

tuple<const double, const Question> Calculations::find_best_split(const Data& rows, const MetaData& meta) {
  double best_gain = 0.0;  // keep track of the best information gain
  auto best_question = Question();  //keep track of the feature / value that produced it
  //TODO: find the best split among all features and feature values
  return forward_as_tuple(best_gain, best_question);
}

const double Calculations::gini(const ClassCounter& counts, double N) {
  double impurity = 1.0;

  //TODO: compute gini index, given class counts and the dataset size
  return impurity;
}

tuple<std::string, double> Calculations::determine_best_threshold_numeric(const Data& data, int col) {
  double best_loss = std::numeric_limits<float>::infinity();
  std::string best_thresh;

  //TODO: find the best split value for a discrete ordinal feature
  return forward_as_tuple(best_thresh, best_loss);
}

tuple<std::string, double> Calculations::determine_best_threshold_cat(const Data& data, int col) {
  double best_loss = std::numeric_limits<float>::infinity();
  std::string best_thresh;

  //TODO: find the best split value for a categorical feature
  return forward_as_tuple(best_thresh, best_loss);
}


const ClassCounter Calculations::classCounts(const Data& data) {
  ClassCounter counter;
  for (const auto& rows: data) {
    const string decision = *std::rbegin(rows);
    if (counter.find(decision) != std::end(counter)) {
      counter.at(decision)++;
    } else {
      counter[decision] += 1;
    }
  }
  return counter;
}
