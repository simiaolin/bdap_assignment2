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
using ClassCounterWithSize = std::tuple<int, ClassCounter>;
using ClassCounterWithFeatureValue = std::tuple<std::string, ClassCounterWithSize>;  // feature_value, overall_count, class_counter
using ClassCounterVec = std::vector<ClassCounterWithFeatureValue>;

namespace Calculations {

std::tuple<const Data, const Data> partition(const Data &data, const Question &q);

const double gini(const ClassCounter& counts, double N);

std::tuple<const double, const Question> find_best_split(const Data &rows, const MetaData &meta);

std::tuple<std::string, double> determine_best_threshold_numeric(const Data &data, int col);

std::tuple<std::string, double> determine_best_threshold_cat(const Data &data, int col,  const MetaData &meta);


const ClassCounter classCounts(const Data &data);


    double get_gini_with_data(const Data &data);

    void sortNumeric(const Data &data, int col);

    std::tuple<std::string, double>
    const get_best_threshold_from_class_counter_vecs(const ClassCounterVec& single,
                                                     const ClassCounterWithSize &sum);


    void add_to_class_counter_vecs(const Data &data, int begin_index, int end_index,
                                   ClassCounterVec& single,
                                   ClassCounterWithSize &sum, int current_feature_value);


    void add_to_class_counter(ClassCounterWithSize &classCounterWithSize, const std::string &decision);

    const ClassCounterWithSize get_false_class_counter(const ClassCounterWithSize &trueClassCounterWithSize, const ClassCounterWithSize &sum);

    bool row_sorter(std::vector<std::string> row1, std::vector<std::string> row2, int col);
} // namespace Calculations

#endif //DECISIONTREE_CALCULATIONS_HPP
