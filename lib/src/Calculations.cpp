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


void Calculations::generateSampleData(const Data &rows, const std::vector<size_t> &samples, Data &sampleData) {
    for (auto index : samples) {
        sampleData.emplace_back(std::move(rows.at(index)));
    }
}


void
Calculations::partition(const Data &data, const Question &q, Data &trueData, Data &falseData) {
    for (const auto &row: data) {
        if (q.solve(row))
            trueData.emplace_back(std::move(row));
        else
            falseData.emplace_back(std::move(row));
    }

}

/**
 * Given data, to find the best split.
 * @param rows the data
 * @param meta the feature information
 * @return a tuple of best gain and best question
 */
tuple<const double, const Question, int, int> Calculations::find_best_split(const Data &rows, const MetaData &meta) {
    std::string best_question_value;
    int col_size = meta.labels.size() - 1;
    AllCatMap allCatMap;
    AllNumericMap allNumericMap;
    for (auto row: rows) {
        for (int col = 0; col < col_size; col++) {
            if (meta.labelTypes.at(col) == 0) {
                //cat
                add_to_all_catmap(allCatMap, col, row);
            } else {
                //numeric
                add_to_all_numericmap(allNumericMap, col, row);
            }
        }
    }

    return get_best_split_from_all_map(allCatMap, allNumericMap, rows.size());
}

void Calculations::add_to_all_catmap(AllCatMap &allCatMap, int &col, std::vector<std::string> &row) {
    std::get<0>(allCatMap[col][row.at(col)])++;
    std::get<1>(allCatMap[col][row.at(col)])[row.back()]++;
}

void Calculations::add_to_all_numericmap(AllNumericMap &allNumericMap, int &col, std::vector<std::string> &row) {
    std::get<0>(allNumericMap[col][std::stoi(row.at(col))])++;
    std::get<1>(allNumericMap[col][std::stoi(row.at(col))])[row.back()]++;
}

std::tuple<const double, const Question, int, int>
Calculations::get_best_split_from_all_map(AllCatMap &allCatMap, AllNumericMap &allNumericMap, const int &allDataSize) {
    //todo:select the smallest one to cal the overall
    ClassCounterWithSize overallClasscounterWithSize =
            (allCatMap.size() > 0) ?
            forward_as_tuple(allDataSize, get_overall_classcounter_cat(allCatMap.begin()->second)) :
            forward_as_tuple(allDataSize, get_overall_classcounter_numeric(allNumericMap.begin()->second));

    double best_gain = 0.0;  // keep track of the best information gain
    std::string best_question_value;
    int best_column;
    int best_true_size;
    int best_false_size;

    for (auto current_cat_map : allCatMap) {
        tuple<std::string, double, int, int> thres_and_loss_and_splitted_size =
                get_best_threshold_from_category_class_counter_vecs(current_cat_map.second,
                                                                    overallClasscounterWithSize);;
        update_best(thres_and_loss_and_splitted_size, current_cat_map.first,
                    best_question_value, best_column, best_gain, best_true_size, best_false_size);
    }
    for (auto current_numeric_map : allNumericMap) {
        tuple<std::string, double, int, int> thres_and_loss_and_splitted_size =
                get_best_threshold_from_numeric_class_counter_map(current_numeric_map.second,
                                                                  overallClasscounterWithSize);
        update_best(thres_and_loss_and_splitted_size, current_numeric_map.first,
                    best_question_value, best_column, best_gain, best_true_size, best_false_size);
    }

    Question best_question(best_column, best_question_value);
    return forward_as_tuple(best_gain, best_question, best_true_size, best_false_size);


}

void Calculations::update_best(tuple<string, double, int, int> &thres_and_loss_and_splitted_size, int col,
                               std::string &best_question_value, int &best_column, double &best_gain,
                               int &best_true_size, int &best_false_size) {
    if (std::get<1>(thres_and_loss_and_splitted_size) > best_gain) {

        best_question_value = std::get<0>(thres_and_loss_and_splitted_size);
        best_column = col;
        best_gain = std::get<1>(thres_and_loss_and_splitted_size);
        best_true_size = std::get<2>(thres_and_loss_and_splitted_size);
        best_false_size = std::get<3>(thres_and_loss_and_splitted_size);
    }
}

const double Calculations::gini(const ClassCounter &counts, const double N) {
    double impurity = 1.0;
    for (auto const &element : counts) {
        impurity -= std::pow(element.second / N, 2);
    }
    return impurity;
}

std::tuple<std::string, double, int, int>
Calculations::determine_best_threshold_numeric_new(const Data &data, int col) {
    cpu_timer cpuTimer;
    NumericClassCounterMap numericClassCounterMap;      //record the ClassCounter and the size for each feature value
    for (std::vector<std::string> row : data) {
        std::get<0>(numericClassCounterMap[std::stoi(row.at(col))])++;
        std::get<1>(numericClassCounterMap[std::stoi(row.at(col))])[row.back()]++;
    }
    ClassCounterWithSize overall = forward_as_tuple(data.size(),
                                                    get_overall_classcounter_numeric(numericClassCounterMap));

    std::tuple<std::string, double, int, int> res = get_best_threshold_from_numeric_class_counter_map(
            numericClassCounterMap, overall);
    bool showNumericSpitting = false;
    if (showNumericSpitting) {

        std::cout << "------------numeric data with size " << data.size() << " use " << cpuTimer.format()
                  << " end-------------" << std::endl << std::endl;
    }
    return res;
}


std::tuple<std::string, double, int, int>
Calculations::determine_best_threshold_cat(const Data &data, int col) {
    CategoryClassCounterMap category_classcounter_map;      //record the ClassCounter and the size for each feature value
//    std::cout<<"-------------cat with data of size " << data.size() << " begin----------" << std::endl;
//    cpu_timer cpuTimer;
    for (std::vector<std::string> row : data) {
        add_to_class_counter(category_classcounter_map[row.at(col)], row.back());
    }
    const ClassCounter overall_classcounter = get_overall_classcounter_cat(category_classcounter_map);
    const ClassCounterWithSize overall_classcounter_with_size = forward_as_tuple(data.size(),
                                                                                 overall_classcounter);    //record the ClassCounter and size for all data

//    std::cout<<"cat get class coutner use " <<  cpuTimer.format() <<std::endl;
    std::tuple<std::string, double, int, int> res = get_best_threshold_from_category_class_counter_vecs(
            category_classcounter_map, overall_classcounter_with_size);
//    std::cout<<"cat get threshold use " <<cpuTimer.format() << std::endl;
//    std::cout<<"-------------cat with data of size " << data.size() << " end----------" << std::endl<<std::endl;

    return res;
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


const std::tuple<std::string, double, int, int>
Calculations::get_best_threshold_from_numeric_class_counter_map(NumericClassCounterMap &numericClassCounterMap,
                                                                const ClassCounterWithSize &overall) {
    double best_loss = std::numeric_limits<float>::infinity();
    std::string best_thresh;
    int best_true_size;
    int best_false_size;
    ClassCounterWithSize true_classcounter_with_size;
    std::get<1>(true_classcounter_with_size).reserve(std::get<1>(overall).size());
    for (auto current = numericClassCounterMap.rbegin(); current != numericClassCounterMap.rend(); current++) {
        accumulate_to_numeric_classcounter(true_classcounter_with_size, current->second);
        const string feature_value = std::to_string(current->first);
        bool has_get_best_loss = get_best_loss(feature_value, true_classcounter_with_size, overall,
                                               best_loss,
                                               best_thresh, best_true_size, best_false_size);
        if (has_get_best_loss)
            break;
    }
    const double overall_gini = gini(std::get<1>(overall), std::get<0>(overall));
    return forward_as_tuple(best_thresh, overall_gini - best_loss, best_true_size, best_false_size);
}


void Calculations::accumulate_to_numeric_classcounter(ClassCounterWithSize &toBeAccumulated,
                                                      ClassCounterWithSize &accumalated) {
    std::get<0>(toBeAccumulated) += std::get<0>(accumalated);
    for (auto class_counter : std::get<1>(accumalated)) {
        if (std::get<1>(toBeAccumulated).find(class_counter.first) != std::get<1>(toBeAccumulated).end()) {
            std::get<1>(toBeAccumulated)[class_counter.first] += class_counter.second;
        } else {
            std::get<1>(toBeAccumulated)[class_counter.first] = class_counter.second;
        }
    }
}


std::tuple<std::string, double, int, int>
const Calculations::get_best_threshold_from_category_class_counter_vecs(
        const CategoryClassCounterMap &categoryClassCounterMap, const ClassCounterWithSize &sum) {
    double best_loss = std::numeric_limits<float>::infinity();
    std::string best_thresh;
    int best_true_size;
    int best_false_size;
    for (auto catClassCounter = categoryClassCounterMap.begin();
         catClassCounter != categoryClassCounterMap.end(); catClassCounter++) {
        bool has_get_best_loss = get_best_loss(catClassCounter->first, catClassCounter->second, sum, best_loss,
                                               best_thresh, best_true_size, best_false_size);
        if (has_get_best_loss)
            break;
    }
    const double overall_gini = gini(std::get<1>(sum), std::get<0>(sum));
    return forward_as_tuple(best_thresh, overall_gini - best_loss, best_true_size, best_false_size);
}

bool
Calculations::get_best_loss(const std::string &feature_value, const ClassCounterWithSize &true_class_counter_with_size,
                            const ClassCounterWithSize &sum, double &best_loss, std::string &best_thresh,
                            int &best_true_size,
                            int &best_false_size) {

    const ClassCounterWithSize false_class_counter_with_size = get_false_class_counter(true_class_counter_with_size,
                                                                                       sum);
    const int true_size = std::get<0>(true_class_counter_with_size);
    const int false_size = std::get<0>(false_class_counter_with_size);
    const double true_gini = gini(std::get<1>(true_class_counter_with_size), true_size);
    const double false_gini = gini(std::get<1>(false_class_counter_with_size), false_size);
    const double current_gini = (true_gini * true_size + false_gini * false_size) / (double) std::get<0>(sum);

    if (current_gini < best_loss) {
        best_loss = current_gini;
        best_thresh = feature_value;
        best_true_size = true_size;
        best_false_size = false_size;
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
    false_class_counter.reserve(std::get<1>(sum).size());
    const ClassCounter sum_counter = std::get<1>(sum);
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

const ClassCounter Calculations::get_overall_classcounter_cat(const CategoryClassCounterMap &categoryClassCounterMap) {
    ClassCounter overall_classcounter;
    for (auto featurevalue_and_classcounter_with_size : categoryClassCounterMap) {
        //todo: map inside or outsize
        const ClassCounter current_classcounter = std::get<1>(featurevalue_and_classcounter_with_size.second);
        for (auto decision_and_count :  current_classcounter) {
            overall_classcounter[decision_and_count.first] += decision_and_count.second;
        }
    }
    return overall_classcounter;
}

const ClassCounter
Calculations::get_overall_classcounter_numeric(const NumericClassCounterMap &numericClassCounterMap) {
    ClassCounter overall_classcounter;
    for (auto featurevalue_and_classcounter_with_size : numericClassCounterMap) {
        //todo: map inside or outsize
        const ClassCounter current_classcounter = std::get<1>(featurevalue_and_classcounter_with_size.second);
        for (auto decision_and_count :  current_classcounter) {
            overall_classcounter[decision_and_count.first] += decision_and_count.second;
        }
    }
    return overall_classcounter;
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
tuple<std::string, double> Calculations::determine_best_threshold_numeric(const Data &data, int col) {
//    cpu_timer cpuTimer;
    Data sortData = sort_numeric_data(data, col);
//    std::cout<<"------------numeric data with size " <<data.size() <<" begin-------------" << std::endl;
//    std::cout<<"sort " <<data.size() <<" use " << cpuTimer.format()<<std::endl;
    int begin_index = 0;
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
//    std::cout<<"------------numeric data with size " <<data.size() <<" end-------------" <<std::endl<<std::endl;
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
        add_to_class_counter(class_counter_with_size, data.at(i).back());
    }
    classCounterWithSizeVec.emplace_back(std::move(forward_as_tuple(current_feature_value, class_counter_with_size)));
}


struct RowComparator {
    explicit RowComparator(int col_) : col(col_) {}

    bool operator()(std::vector<string> row1, std::vector<string> row2) const {
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
std::tuple<std::string, double> const Calculations::get_best_threshold_from_numeric_class_counter_vec(
        const NumericClassCounterVec &classCounterWithSizeVec) {
    double best_loss = std::numeric_limits<float>::infinity();
    ClassCounterWithSize overall_class_counter_with_size = std::get<1>(classCounterWithSizeVec.back());
    std::string best_thresh;
    int best_true_size;
    int best_false_size;
    for (int index = 0; index < classCounterWithSizeVec.size(); index++) {
        //todo map before of after
        const string feature_value = std::get<0>(classCounterWithSizeVec.at(index));
        const ClassCounterWithSize true_classcounter_with_size = std::get<1>(classCounterWithSizeVec.at(index));
        bool has_get_best_loss = get_best_loss(feature_value, true_classcounter_with_size,
                                               overall_class_counter_with_size, best_loss,
                                               best_thresh, best_true_size, best_false_size);
        if (has_get_best_loss)
            break;
    }
    const double overall_gini = gini(std::get<1>(overall_class_counter_with_size),
                                     std::get<0>(overall_class_counter_with_size));
    return forward_as_tuple(best_thresh, overall_gini - best_loss);
}

//tuple<const double, const Question, int, int> Calculations::find_best_split(const Data &rows, const MetaData &meta) {
//    double best_gain = 0.0;  // keep track of the best information gain
//    std::string best_question_value;
//    int best_column;
//    double current_gain;
//    int best_true_size;
//    int best_false_size;
//
////    int i = std::thread::hardware_concurrency();    //获取当前CPU核心数量
//    vector<tuple<int, tuple<std::string, double, int, int>>>* thres_and_loss_and_splitted_size_vec =
//            new vector<tuple<int, tuple<std::string, double, int, int>>>(meta.labels.size() - 1);
//    int thread_num = 2;
//    std::vector<std::tuple<int, int>> colIndicesEachThreadVec = getColumnCntEachThread(thread_num, meta.labels.size() - 1);
//    for (auto colIndice : colIndicesEachThreadVec) {
//        std::thread t([&rows, &meta, thres_and_loss_and_splitted_size_vec, &colIndice] () {
//            for (int col = std::get<0>(colIndice); col < std::get<1>(colIndice); col++) {
//                tuple<std::string, double, int, int> thres_and_loss_and_splitted_size =
//                        (meta.labelTypes.at(col) > 0) ?
//                        determine_best_threshold_numeric_new(rows, col) :
//                        determine_best_threshold_cat(rows, col);
//                thres_and_loss_and_splitted_size_vec->push_back(forward_as_tuple(col, thres_and_loss_and_splitted_size));
//
//            }
//        });
//        t.join();
//    }
//
//    for (auto current_best : *thres_and_loss_and_splitted_size_vec) {
//        int col = std::get<0>(current_best);
//        tuple<std::string, double, int, int> thres_and_loss_and_splitted_size = std::get<1>(current_best);
//        current_gain = std::get<1>(thres_and_loss_and_splitted_size);
//        if (current_gain > best_gain) {
//            best_question_value = std::get<0>(thres_and_loss_and_splitted_size);
//            best_column = col;
//            best_gain = current_gain;
//            best_true_size = std::get<2>(thres_and_loss_and_splitted_size);
//            best_false_size = std::get<3>(thres_and_loss_and_splitted_size);
//        }
//    }
//    delete thres_and_loss_and_splitted_size_vec;
//    Question best_question(best_column, best_question_value);
//    return forward_as_tuple(best_gain, best_question, best_true_size, best_false_size);
//}

//std::vector<std::tuple<int, int>> Calculations::getColumnCntEachThread(int threadNum, int columnNum) {
//    int min_avg_each_thread = columnNum / threadNum;
//    int left_over_col_cnt = columnNum - min_avg_each_thread * threadNum;
//    std::vector<std::tuple<int, int>> res;
//    res.reserve(threadNum);
//    int begin_index = 0;
//    int end_index = 0;
//    for (int i = 0 ; i < threadNum; i++) {
//        if (left_over_col_cnt > 0) {
//            end_index = begin_index + min_avg_each_thread + 1;
//            left_over_col_cnt--;
//        } else {
//            end_index = begin_index + min_avg_each_thread;
//        }
//        res.emplace_back(forward_as_tuple(begin_index, end_index));
//        begin_index = end_index;
//    }
//    return res;
//}

/**
 * Given data, to find the best split.
 * @param rows the data
 * @param meta the feature information
 * @return a tuple of best gain and best question
 */
//tuple<const double, const Question, int, int> Calculations::find_best_split(const Data &rows, const MetaData &meta) {
//    std::string best_question_value;
//    int col_size = meta.labels.size() - 1;
//
//
//    AllCatMap*  allCatMap1 = new AllCatMap;
//    AllNumericMap*  allNumericMap1 = new AllNumericMap;
//    AllCatMap*  allCatMap2 = new AllCatMap ;
//    AllNumericMap*  allNumericMap2 = new AllNumericMap ;
//    int mid = rows.size() / 2;
//    int size = rows.size();
//    std::thread t1([&rows, &meta, &mid, &col_size, allCatMap1, allNumericMap1] ( ) {
//        get_maps(0, mid, col_size, meta, rows, *allCatMap1, *allNumericMap1);
//    });
//
//    std::thread t2([&rows, &meta, &mid, &size,&col_size, allCatMap2, allNumericMap2] ( ) {
//        get_maps(mid, size, col_size, meta, rows, *allCatMap2, *allNumericMap2);
//    });
//
//    t1.join();
//    t2.join();
//
//    merge_all_catmap(*allCatMap1, *allCatMap2);
//    merge_all_numericmap(*allNumericMap1, *allNumericMap2);
//
//    std::tuple<const double, const Question, int, int> res = get_best_split_from_all_map(*allCatMap1, *allNumericMap1, rows.size());
//    delete allNumericMap1;
//    delete allCatMap1;
//    delete allNumericMap2;
//    delete allCatMap2;
//    return res;
//}
//
//void
//Calculations::get_maps(int begin, int end, int col_size, const MetaData &meta, const Data &rows,
//                       AllCatMap &allCatMap,
//                       AllNumericMap &allNumericMap) {
//    for (int row_index = begin; row_index < end; row_index++) {
//        for (int col = 0; col < col_size; col++) {
//            if (meta.labelTypes.at(col) == 0) {
//                add_to_all_catmap(allCatMap, col, rows.at(row_index));
//            } else {
//                add_to_all_numericmap(allNumericMap, col, rows.at(row_index));
//            }
//        }
//    }
//}
//
//void Calculations::merge_all_catmap(AllCatMap &toBeAccumulatedAllCatMap, AllCatMap &accumulatedAllCatMap) {
//    for (auto col_with_catmap : accumulatedAllCatMap) {
//        for (auto feature_with_class_counter : col_with_catmap.second) {
//            accumulate_to_numeric_classcounter(
//                    toBeAccumulatedAllCatMap[col_with_catmap.first][feature_with_class_counter.first],
//                    accumulatedAllCatMap[col_with_catmap.first][feature_with_class_counter.first]);
//        }
//    }
//}
//
//void
//Calculations::merge_all_numericmap(AllNumericMap &toBeAccumulatedNumericMap, AllNumericMap &accumulatedAllNumericMap) {
//    for (auto col_with_catmap : accumulatedAllNumericMap) {
//        for (auto feature_with_class_counter : col_with_catmap.second) {
//            accumulate_to_numeric_classcounter(
//                    toBeAccumulatedNumericMap[col_with_catmap.first][feature_with_class_counter.first],
//                    accumulatedAllNumericMap[col_with_catmap.first][feature_with_class_counter.first]);
//        }
//    }
//}