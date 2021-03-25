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
using namespace std::placeholders;


tuple<const Data, const Data> Calculations::partition(const Data &data, const Question &q) {
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

tuple<const double, const Question> Calculations::find_best_split(const Data &rows, const MetaData &meta) {
    double best_gain = 0.0;  // keep track of the best information gain
    auto best_question = Question();  //keep track of the feature / value that produced it
    std::string best_question_value;
    int best_column;
    double current_gain;
    for (int col = 0; col < meta.labels.size() - 1; col++) {
        //here I do not take into account the real value
        tuple<std::string, double> thres_and_loss =
                determine_best_threshold(rows, col, std::get<0>(meta.featureMap.at(col)) > 0);

        current_gain = std::get<1>(thres_and_loss);
        if (current_gain > best_gain) {
            best_question_value = std::get<0>(thres_and_loss);
            best_column = col;
            best_gain = current_gain;
        }
    }
    best_question = Question(best_column, best_question_value);
    return forward_as_tuple(best_gain, best_question);
}

const double Calculations::gini(const ClassCounter &counts, double N) {
    double impurity = 1.0;
    for (auto const &element : counts) {
        impurity -= std::pow(element.second / N, 2);
    }
    return impurity;
}

//todo: two values make it simpler
tuple<std::string, double> Calculations::determine_best_threshold(const Data &data, int col, bool isNumeric) {
    sort_data(data, col);
    int begin_index =0;
    int end_index = 0;
    string current_feature_value = data.front().at(0);
    ClassCounterVec single;
    ClassCounterWithSize sum;
    for (std::vector<std::string> row : data) {
        if (row.at(col) == current_feature_value) {
            end_index++;
        } else {
            //keep track of the classcounter and size of current feature value.
            add_to_class_counter_vecs(data, begin_index, end_index, single, sum, current_feature_value, isNumeric);
            //reset begin_index and current_feature_value
            begin_index = end_index;
            end_index++;
            current_feature_value = row.at(col);
        }
    }
    add_to_class_counter_vecs(data, begin_index, end_index, single, sum, current_feature_value, isNumeric);
    if (isNumeric) {
        sum = std::get<1>(single.back());
    }
    return get_best_threshold_from_class_counter_vecs(single, sum);
}



void Calculations::add_to_class_counter_vecs(const Data &data, int begin_index, int end_index,
                                             ClassCounterVec &single, ClassCounterWithSize &sum,
                                             std::string current_feature_value, bool isNumeric) {
    ClassCounterWithSize classCounter;
    if (isNumeric && single.size() > 0) {
        //initialize as the accumulated class counter of feature value greater then current feature value.
        //todo: check this.
        classCounter = forward_as_tuple(std::get<0>(std::get<1>(single.back())), std::get<1>(std::get<1>(single.back())));
    }
    for (int i = begin_index; i < end_index; i++) {
        const string decision = data.at(i).back();
        add_to_class_counter(classCounter, decision);
        if (!isNumeric) {
            //there is no need to calculated the sum class counter for numeric feature,
            // because the last one is exactly the sum class counter.
            add_to_class_counter(sum, decision);
        }
    }
    single.push_back(forward_as_tuple(current_feature_value, classCounter));
}

void Calculations::add_to_class_counter(ClassCounterWithSize &classCounterWithSize, const string &decision) {
    ClassCounter &class_counter = std::get<1>(classCounterWithSize);
    if (class_counter.find(decision) != std::end(class_counter)) {
        class_counter.at(decision)++;
    } else {
        class_counter[decision] += 1;
    }
    std::get<0>(classCounterWithSize)++;
}


bool Calculations::row_sorter(std::vector<string> row1, std::vector<string> row2, int col) {
    return row1.at(col) > row2.at(col);
}

void Calculations::sort_data(const Data &data, int col) {
    Data *temp = (Data *) &data;
    sort(temp->begin(), temp->end(), std::bind(row_sorter, _1, _2, col));
}


const tuple<std::string, double> Calculations::get_best_threshold_from_class_counter_vecs(
        const ClassCounterVec &single, const ClassCounterWithSize &sum) {
    double best_loss = std::numeric_limits<float>::infinity();
    std::string best_thresh;
    int overall_size = std::get<0>(sum);
    double current_gini;
    for (int index = 0; index < single.size(); index++) {
        ClassCounterWithFeatureValue true_feature_value_and_class_counter = single.at(index);
        const string feature_value = std::get<0>(true_feature_value_and_class_counter);
        const ClassCounterWithSize true_class_counter_with_size = std::get<1>(true_feature_value_and_class_counter);
        const ClassCounterWithSize false_class_counter_with_size = get_false_class_counter(true_class_counter_with_size,
                                                                                           sum);
        const int true_size = std::get<0>(true_class_counter_with_size);
        const int false_size = std::get<0>(false_class_counter_with_size);
        const double true_gini = gini(std::get<1>(true_class_counter_with_size), true_size);
        const double false_gini = gini(std::get<1>(false_class_counter_with_size), false_size);
        current_gini =  (true_gini * true_size + false_gini * false_size) / (double)overall_size;

        if (current_gini < best_loss) {
            best_loss = current_gini;
            best_thresh = feature_value;
            if (IsAlmostEqual(best_loss, 0.0))
                break;
        }
    }
    const double overall_gini = gini(std::get<1>(sum), std::get<0>(sum));
    return forward_as_tuple(best_thresh,  overall_gini -  best_loss);
}


const ClassCounterWithSize Calculations::get_false_class_counter(
        const ClassCounterWithSize &trueClassCounterWithSize, const ClassCounterWithSize &sum) {
    ClassCounter false_class_counter;
    const ClassCounter sum_counter =  std::get<1>(sum);
    const ClassCounter true_counter = std::get<1>(trueClassCounterWithSize);
    for (auto counter = sum_counter.begin(); counter != sum_counter.end(); counter++) {
        if (true_counter.find(counter->first) != std::end(true_counter)) {
            false_class_counter[counter->first] = counter->second - true_counter.at(counter->first);
        } else {
            false_class_counter[counter->first] = counter->second;
        }
    }
    int false_size = std::get<0>(sum) - std::get<0>(trueClassCounterWithSize);
    return forward_as_tuple(false_size, false_class_counter);
}


const ClassCounter Calculations::classCounts(const Data &data) {
    ClassCounter counter;
    for (const auto &rows: data) {
        const string decision = *std::rbegin(rows);
        if (counter.find(decision) != std::end(counter)) {
            counter.at(decision)++;
        } else {
            counter[decision] += 1;
        }
    }
    return counter;
}

//deprecated
//void Calculations::add_to_sum_counter_vec(const ClassCounterVec& single,
//                                          ClassCounterVec& sum) {
//    assert(single.size() == sum.size() + 1);
//    const tuple<std::string, ClassCounter> last_of_single = single.back();
//    const string feature_value = std::get<0>(last_of_single);
//    const ClassCounter last_class_counter_of_single = std::get<1>(last_of_single);
//    const ClassCounter last_class_counter_of_sum = std::get<1>(sum.back());
//    ClassCounter new_class_counter;
//    for (auto counter = last_class_counter_of_single.begin(); counter != last_class_counter_of_single.end(); counter++) {
//        new_class_counter[counter->first] = counter->second;
//    }
//
//    for (auto counter = last_class_counter_of_sum.begin(); counter != last_class_counter_of_sum.end(); counter++) {
//        if (new_class_counter.find(counter->first) != std::end(new_class_counter)) {
//            new_class_counter.at(counter-> first) += counter->second;
//        } else{
//            new_class_counter[counter->first] = counter->second;
//        }
//    }
//
//    sum.push_back(forward_as_tuple(feature_value, new_class_counter));
//
//}
