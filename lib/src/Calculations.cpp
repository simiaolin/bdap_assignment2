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
//using namespace std::placeholders;
using boost::timer::cpu_timer;



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

/**
 * Given data, to find the best split.
 * @param rows the data
 * @param meta the feature information
 * @return a tuple of best gain and best question
 */
tuple<const double, const Question> Calculations::find_best_split(const Data &rows, const MetaData &meta) {
    double best_gain = 0.0;  // keep track of the best information gain
    auto best_question = Question();  //keep track of the feature / value that produced it
    std::string best_question_value;
    int best_column;
    double current_gain;

    //iterate through all the features, and find their best split, correspondingly.
    for (int col = 0; col < meta.labels.size() - 1; col++) {
        ClassCounterWithSize sum;    //record the ClassCounter and size for all data
        tuple<std::string, double> thres_and_loss =
                (meta.labelTypes.at(col) > 0) ?
                determine_best_threshold_numeric(rows, col) :
                determine_best_threshold_cat(rows, col, sum);


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

/**
 * Given data and a specific feature, to determine the threshold that generates the best gain.
 * Before all, to sort the data based on given column, no matter the corresponding feature values are numeric or category.
 * Later iterate through the sorted data,
 *      record the ClassCounter and the size for each feature value, as well as the ClassCounter and size of all data.
 * Finally, to get the best threshold based on its corresponding ClassCounter, and the overall ClassCounter.
 * @param data  the data
 * @param col   the column index of the feature
 * @param sum   the overall class counter with size
 * @return    a tuple of best threshold and best gain, correspondingly.
 */
tuple<std::string, double>
Calculations::determine_best_threshold_numeric(const Data &data, int col) {
    cpu_timer cpuTimer;
    Data sortData = sort_numeric_data(data, col);
    std::cout<<"------------numeric data with size " <<data.size() <<" begin-------------" << std::endl;
    std::cout<<"sort " <<data.size() <<" use " << cpuTimer.format()<<std::endl;
    int begin_index =0;
    int end_index = 0;
    string current_feature_value = sortData.front().at(0);
    NumericClassCounterVec numericClassCounterVec;      //record the ClassCounter and the size for each feature value
    for (std::vector<std::string> row : sortData) {
        if (row.at(0) == current_feature_value) {
            end_index++;
        } else {
            add_to_class_counter_vec(sortData, begin_index, end_index, numericClassCounterVec, current_feature_value);
            begin_index = end_index;
            end_index++;
            current_feature_value = row.at(0);
        }
    }
    add_to_class_counter_vec(sortData, begin_index, end_index, numericClassCounterVec, current_feature_value);
//    std::cout<<"get the numeric class coutner use " <<cpuTimer.format() << std::endl;
    std::tuple<std::string, double> res = get_best_threshold_from_numeric_class_counter_vec(numericClassCounterVec);
//    std::cout<<"get the threshold use " << cpuTimer.format() << std::endl;
    std::cout<<"------------numeric data with size " <<data.size() <<" end-------------" <<std::endl<<std::endl;
    return res;
}

tuple<std::string, double>
        Calculations::determine_best_threshold_cat(const Data& data, int col, ClassCounterWithSize& sum) {
    CategoryClassCounterMap single;      //record the ClassCounter and the size for each feature value
    string current_feature_value;
    string current_decision;
    std::cout<<"-------------cat with data of size " << data.size() << " begin----------" << std::endl;
    cpu_timer cpuTimer;
    for (std::vector<std::string> row : data) {
        current_feature_value = row.at(col);
        current_decision = row.back();
        add_to_class_counter(single[current_feature_value], current_decision);
        add_to_class_counter(sum, current_decision);
    }

//    std::cout<<"cat get class coutner use " <<  cpuTimer.format() <<std::endl;
    std::tuple<std::string, double> res = get_best_threshold_from_category_class_counter_vecs(single, sum);
    std::cout<<"cat get threshold use " <<cpuTimer.format() << std::endl;
    std::cout<<"-------------cat with data of size " << data.size() << " end----------" << std::endl<<std::endl;

    return res;
}

/**
 * Given a specific feature value, and its range in the sorted data,
 * record the ClassCounter and the size for it, and update the ClassCounter and size of all data.
 * @param data   the sorted data
 * @param begin_index   begin index of sorted data having the specific feature value
 * @param end_index     end index of sorted data having the specific feature value
 * @param classCounterWithSizeVec        records the size and the class counter for each feature value.
 *
 *                      for the feature of type CATEGORY, each element in 「classCounterWithSizeVec」 keeps its size and class counter.
 *                      for the feature of type NUMERIC, each element in 「classCounterWithSizeVec」 keeps the accumulated size and class counter.
 *                      For instance, for CATEGORY feature value 「Yellow」, we partition via v == Yellow,
 *                      so we only have to care about the size and the class counter for it.
 *                      However, for NUMERIC feature value 「15」, actually we partition via v >= 15,
 *                      so all the size and class counter for feature value greater than 15 should be accumulated.
 * @param sum           records the size and the class counter for all data
 * @param current_feature_value    the specific feature value
 * @param isNumeric     whether the current specific feature value is numeric or not
 * @param col           the column index of the feature
 */
void Calculations::add_to_class_counter_vec(const Data &data, int begin_index, int end_index,
                                            NumericClassCounterVec &classCounterWithSizeVec,
                                            const string &current_feature_value) {
    ClassCounterWithSize class_counter_with_size;
    if (classCounterWithSizeVec.size() > 0) {
        //for NUMERIC feature,
        //initialize class_counter_with_size with the last accumulated class_counter_with_size in the vector 「classCounterWithSizeVec」.
        class_counter_with_size = forward_as_tuple(
                std::get<0>(std::get<1>(classCounterWithSizeVec.back())),
                        std::get<1>(std::get<1>(classCounterWithSizeVec.back())));
    }
    for (int i = begin_index; i < end_index; i++) {
        const string decision = data.at(i).back();
        add_to_class_counter(class_counter_with_size, decision);
    }
    classCounterWithSizeVec.push_back(forward_as_tuple(current_feature_value, class_counter_with_size));
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

struct RowComparator {
    explicit RowComparator(int col_): col(col_) {}

    bool operator ()(std::vector<string> row1, std::vector<string> row2) const {
        return row1.at(col) > row2.at(col);
    }
    int col;
};

//bool Calculations::row_sorter(std::vector<string> row1, std::vector<string> row2, int col) {
//    return row1.at(col) > row2.at(col);
//}

/**
 * To sort data reversely given column index.
 * For instance, to sort the data below on column 1,
 *
 * Green, 22, Apple
 * Yellow, 34, Banana
 * Purple, 25, Grape
 *
 * would lead to a result
 * Yellow, 34, Banana
 * Purple, 25, Grape
 *  Green, 22, Apple
 * @param data  data
 * @param col  the column to sort on
 */
const Data Calculations::sort_numeric_data(const Data &data, int col) {

    Data sorted_data;
    for (VecS row : data) {
        const VecS new_row{row.at(col), row.back()};
        sorted_data.emplace_back(std::move(new_row));
    }
    Data *temp = (Data *) &sorted_data;
    sort(temp->begin(), temp->end(), sorter);
    return sorted_data;
}

bool Calculations::sorter(VecS &row1, VecS &row2) {
    return std::stoi(row1.front()) > std::stoi(row2.front());
}
/**
 * To get the best threshold.
 * @param classCounterWithSizeVec        records the size and the class counter for each feature value.
 * @param sum                            records the size and the class counter for all data
 * @return  the best threshold and the best gain
 */
std::tuple<std::string, double>
const Calculations::get_best_threshold_from_numeric_class_counter_vec(
        const NumericClassCounterVec &classCounterWithSizeVec) {
    double best_loss = std::numeric_limits<float>::infinity();
    ClassCounterWithSize sum = std::get<1>(classCounterWithSizeVec.back());
    std::string best_thresh;
    for (int index = 0; index < classCounterWithSizeVec.size(); index++) {
        ClassCounterWithFeatureValue true_feature_value_and_class_counter = classCounterWithSizeVec.at(index);
        const string feature_value = std::get<0>(true_feature_value_and_class_counter);
        const ClassCounterWithSize true_class_counter_with_size = std::get<1>(true_feature_value_and_class_counter);
        bool has_get_best_loss = get_best_loss(feature_value, true_class_counter_with_size, sum, best_loss,
                                               best_thresh);
        if (has_get_best_loss)
            break;
    }
    const double overall_gini = gini(std::get<1>(sum), std::get<0>(sum));
    return forward_as_tuple(best_thresh,  overall_gini -  best_loss);
}

std::tuple<std::string, double>
const Calculations::get_best_threshold_from_category_class_counter_vecs(const CategoryClassCounterMap &categoryClassCounterMap,
                                                                       ClassCounterWithSize &sum) {
    double best_loss = std::numeric_limits<float>::infinity();
    std::string best_thresh;
    for (auto catClassCounter = categoryClassCounterMap.begin(); catClassCounter != categoryClassCounterMap.end(); catClassCounter++) {
        const string feature_value =  catClassCounter->first;
        const ClassCounterWithSize true_class_counter_with_size = catClassCounter->second;

        bool has_get_best_loss = get_best_loss(feature_value, true_class_counter_with_size, sum, best_loss,
                                               best_thresh);
        if (has_get_best_loss)
            break;
    }
    const double overall_gini = gini(std::get<1>(sum), std::get<0>(sum));
    return forward_as_tuple(best_thresh,  overall_gini -  best_loss);
}

bool
Calculations::get_best_loss(const std::string &feature_value, const ClassCounterWithSize &true_class_counter_with_size,
                            const ClassCounterWithSize &sum, double &best_loss, std::string &best_thresh) {

    const ClassCounterWithSize false_class_counter_with_size = get_false_class_counter(true_class_counter_with_size,
                                                                                       sum);
    const int true_size = std::get<0>(true_class_counter_with_size);
    const int false_size = std::get<0>(false_class_counter_with_size);
    const double true_gini = gini(std::get<1>(true_class_counter_with_size), true_size);
    const double false_gini = gini(std::get<1>(false_class_counter_with_size), false_size);
    const double current_gini =  (true_gini * true_size + false_gini * false_size) / (double) std::get<0>(sum);

    if (current_gini < best_loss) {
        best_loss = current_gini;
        best_thresh = feature_value;
        if (IsAlmostEqual(best_loss, 0.0))
            return true;
    }
    return false;
}

/**
 * To get false class counter with size.
 * @param trueClassCounterWithSize   records the size and the class counter for true data
 * @param sum                        records the size and the class counter for overall data
 * @return the size and the class counter for false data
 */
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
