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

struct NumericFeatureValueComparator {
    bool operator ()(int featureValue1, int featureValue2) const {
        return featureValue1 > featureValue2;
    }
};

using ClassCounter = std::unordered_map<std::string, int>;
using ClassCounterWithSize = std::tuple<int, ClassCounter>;  // <size, class_counter>

using CategoryClassCounterMap = std::unordered_map<std::string, ClassCounterWithSize>;  // map <feature_value, class_counter_with_size>

using NumericClassCounterMap = std::map<int, ClassCounterWithSize>;  // map <feature_value, class_counter_with_size>

using ClassCounterWithFeatureValue = std::tuple<std::string, ClassCounterWithSize>;
using NumericClassCounterVec = std::vector<ClassCounterWithFeatureValue>;     //vector <feature_value, class_counter_with_size>

namespace Calculations {

    std::tuple<const Data, const Data> partition(const Data &data, const Question &q);

    const double gini(const ClassCounter &counts, const double N);

    std::tuple<const double, const Question> find_best_split(const Data &rows, const MetaData &meta);

    std::tuple<std::string, double>
    determine_best_threshold_numeric(const Data &data, int col);

    const ClassCounter classCounts(const Data &data);


    const Data sort_numeric_data(const Data &data, int col);

    std::tuple<std::string, double>
    const get_best_threshold_from_numeric_class_counter_vec(const NumericClassCounterVec &classCounterWithSizeVec);

    std::tuple<std::string, double> const
    get_best_threshold_from_numeric_class_counter_map(NumericClassCounterMap &numericClassCounterMap,
                                                      const ClassCounterWithSize &overall);

        void add_to_class_counter_vec(const Data &data, int begin_index, int end_index,
                                  NumericClassCounterVec &classCounterWithSizeVec,
                                  const std::string &current_feature_value);


    void add_to_class_counter(ClassCounterWithSize &classCounterWithSize, const std::string &decision);

    const ClassCounterWithSize
    get_false_class_counter(const ClassCounterWithSize &trueClassCounterWithSize, const ClassCounterWithSize &sum);
    std::tuple<std::string, double> determine_best_threshold_numeric_new(const Data &data, int col);

    std::tuple<std::string, double> determine_best_threshold_cat(const Data &data, int col);

    bool get_best_loss(const std::string &feature_value, const ClassCounterWithSize &true_class_counter_with_size,
                       const ClassCounterWithSize &sum, double &best_loss, std::string &best_thresh);

    const std::tuple<std::string, double>
    get_best_threshold_from_category_class_counter_vecs(const CategoryClassCounterMap &categoryClassCounterMap,
                                                        const ClassCounterWithSize &sum);

    bool sorter(VecS &row1, VecS &row2);

    const ClassCounter get_overall_classcounter_cat(const CategoryClassCounterMap &categoryClassCounterMap);

    void accumulate_to_numeric_classcounter(ClassCounterWithSize &toBeAccumulated, ClassCounterWithSize &accumalated);

    const ClassCounter get_overall_classcounter_numeric(const NumericClassCounterMap &numericClassCounterMap);
} // namespace Calculations

#endif //DECISIONTREE_CALCULATIONS_HPP
