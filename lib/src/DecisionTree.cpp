/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include "DecisionTree.hpp"

using std::make_shared;
using std::shared_ptr;
using std::string;
using boost::timer::cpu_timer;
using std::tuple;


DecisionTree::DecisionTree(const DataReader& dr) : root_(Node()), dr_(dr) {
  std::cout << "Start building tree." << std::endl; cpu_timer timer;
  root_ = buildTree(dr_.trainData(), dr_.metaData());
  std::cout << "Done. " << timer.format() << std::endl;
}

DecisionTree::DecisionTree(const DataReader& dr, const std::vector<size_t>& samples): root_(Node()), dr_(dr){
    std::cout << "Start building tree in bagging." << std::endl; cpu_timer timer;

    Data sample_data;
    Calculations::generateSampleData(dr_.trainData(), samples, sample_data);
    root_ = buildTree(sample_data, dr_.metaData());
    std::cout << "Done with building tree in bagging. " << timer.format() << std::endl;
}

const Node DecisionTree::buildTree(const Data &rows, const MetaData& meta) {
    bool showSplittingMsg = true;
    cpu_timer cpuTimer1;
  tuple<const double, const Question> gain_question = Calculations::find_best_split(rows, meta);
    if (showSplittingMsg) {
        std::cout<<"----find best split with size " <<rows.size() << " use " <<cpuTimer1.format() << "----"<< std::endl;
    }

    double gain = std::get<0>(gain_question);
  Question question = std::get<1> (gain_question);
  if (IsAlmostEqual(gain, 0.0)) {
      ClassCounter classCounter = Calculations::classCounts(rows);
      Leaf leaf(classCounter);
      Node leafNode(leaf);
      return leafNode;
  } else {
//      cpu_timer cpuTimer;
       Data true_data;
       Data false_data;
      Calculations::partition(rows, question, true_data,false_data);
//      std::cout<<"split size " << rows.size() << " using " << cpuTimer.format() << std::endl;
      // In case there is empty branch
      Node trueBranch = buildTree(true_data, meta);
      Node falseBranch = buildTree(false_data, meta);
      return Node(trueBranch, falseBranch, question);
  }
}

void DecisionTree::print() const {
  print(make_shared<Node>(root_));
}

void DecisionTree::print(const shared_ptr<Node> root, string spacing) const {
  if (bool is_leaf = root->leaf() != nullptr; is_leaf) {
    const auto &leaf = root->leaf();
    std::cout << spacing + "Predict: "; Utils::print::print_map(leaf->predictions());
    return;
  }
  std::cout << spacing << root->question().toString(dr_.metaData().labels) << "\n";

  std::cout << spacing << "--> True: " << "\n";
  print(root->trueBranch(), spacing + "   ");

  std::cout << spacing << "--> False: " << "\n";
  print(root->falseBranch(), spacing + "   ");
}

void DecisionTree::test() const {
  TreeTest t(dr_.testData(), dr_.metaData(), root_);
}
