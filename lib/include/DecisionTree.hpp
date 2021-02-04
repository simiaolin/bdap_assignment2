/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_DECISIONTREE_HPP
#define DECISIONTREE_DECISIONTREE_HPP

#include "Calculations.hpp"
#include "DataReader.hpp"
#include "Node.hpp"
#include "TreeTest.hpp"
#include "Utils.hpp"

class DecisionTree {
  public:
    DecisionTree() = delete;
    explicit DecisionTree(const DataReader& dr);
    explicit DecisionTree(const DataReader& dr, const std::vector<size_t>& samples);

    void print() const;
    void test() const;

    inline Data testData() { return dr_.testData(); }
    inline std::shared_ptr<Node> root() { return std::make_shared<Node>(root_); }

    Node root_;
  private:
    DataReader dr_;

    const Node buildTree(const Data& rows, const MetaData &meta);
    void print(const std::shared_ptr<Node> root, std::string spacing="") const;

};

#endif //DECISIONTREE_DECISIONTREE_HPP
