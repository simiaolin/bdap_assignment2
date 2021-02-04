/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_CALCULATIONS_HPP
#define DECISIONTREE_CALCULATIONS_HPP

#include <tuple>
#include <vector>
#include <string>
#include <unordered_map>
#include <boost/timer/timer.hpp>
#include "Question.hpp"
#include "Utils.hpp"

using ClassCounter = std::unordered_map<std::string, int>;

namespace Calculations {

std::tuple<const Data, const Data> partition(const Data &data, const Question &q);

const double gini(const ClassCounter& counts, double N);

std::tuple<const double, const Question> find_best_split(const Data &rows, const MetaData &meta);

std::tuple<std::string, double> determine_best_threshold_numeric(const Data &data, int col);

std::tuple<std::string, double> determine_best_threshold_cat(const Data &data, int col);


const ClassCounter classCounts(const Data &data);

} // namespace Calculations

#endif //DECISIONTREE_CALCULATIONS_HPP
