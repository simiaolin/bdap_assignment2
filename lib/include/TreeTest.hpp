/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_TREETEST_HPP
#define DECISIONTREE_TREETEST_HPP

#include "Node.hpp"
#include "Utils.hpp"

using ClassCounterScaled = std::unordered_map<std::string, std::string>;

class TreeTest {
  public:
    TreeTest() = default;
    TreeTest(const Data& testData, const MetaData& meta, const Node &root);
    ~TreeTest() = default;

    const ClassCounter classify(const VecS& row, std::shared_ptr<Node> node) const;

  private:
    void printLeaf(ClassCounter counts) const;
    void test(const Data& testing_data, const VecS& labels, std::shared_ptr<Node> tree) const;
};

#endif //DECISIONTREE_TREETEST_HPP
