/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include <cmath>
#include <algorithm>
#include <iterator>
#include "Calculations.hpp"
#include "Question.hpp"
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
  std::string best_question_value;
  int best_column;
  double current_gain;
    for (int i = 0; i < meta.labels.size() - 1; i++) {
      //todo: check the real feature
      tuple<std::string, double> thres_and_loss = (std::get<0>(meta.featureMap.at(i)) > 0)?
              determine_best_threshold_numeric(rows, i, meta) : determine_best_threshold_cat(rows, i, meta);

        current_gain = 1 - std::get<1>(thres_and_loss);
      //todo: check the > < =
      if (current_gain > best_gain) {
          best_question_value = std::get<0> (thres_and_loss);
          best_column = i;
          best_gain = current_gain;
      }
  }
  best_question = Question(best_column, best_question_value);
  return forward_as_tuple(best_gain, best_question);
}

const double Calculations::gini(const ClassCounter& counts, double N) {
  double impurity = 1.0;
  for (auto const& element : counts) {
      impurity -= std::pow(element.second  / N, 2);
  }
  return impurity;
}

tuple<std::string, double> Calculations::determine_best_threshold_numeric(const Data& data, int col,  const MetaData &meta) {
  double best_loss = std::numeric_limits<float>::infinity();
  std::string best_thresh;

  //TODO: find the best split value for a discrete ordinal feature
    tuple<Data, std::vector<int>>  sorted_data_with_numeric_possible_values = sortNumeric(data, col);
    for (int feature_value : std::get<1>(sorted_data_with_numeric_possible_values)) {

    }
  return forward_as_tuple(best_thresh, best_loss);
}

tuple<Data, std::vector<int>> Calculations::sortNumeric(const Data& data, int col) {

}

tuple<std::string, double> Calculations::determine_best_threshold_cat(const Data& data, int col,  const MetaData &meta) {
  double best_loss = std::numeric_limits<float>::infinity();
  std::string best_thresh;
  double current_gini;
  //todo: if there are two value, no need to calculate both of them.
  for (std::string featureValue : std::get<1>(meta.featureMap.at(col))) {
      Question q(col, featureValue);
      tuple<Data, Data> true_false_data = partition(data, q);
      Data true_data = std::get<0>(true_false_data);
      Data false_data = std::get<1>(true_false_data);
      double size_of_overall = data.size();
      current_gini = (get_gini_with_data(true_data) + get_gini_with_data(false_data) ) / size_of_overall;

      if (current_gini < best_loss) {
          best_loss = current_gini;
          best_thresh = featureValue;
      }
  }
  return forward_as_tuple(best_thresh, best_loss);
}

double Calculations::get_gini_with_data(const Data& data) {
    double data_size = data.size();
    ClassCounter classCounter = classCounts(data);
    double gini_value = gini(classCounter, data_size);
    return gini_value * data_size;
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
