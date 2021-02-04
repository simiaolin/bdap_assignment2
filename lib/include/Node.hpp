/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_NODE_HPP
#define DECISIONTREE_NODE_HPP

#include <memory>
#include "Leaf.hpp"
#include "Question.hpp"

/**
 * Representation of a decision tree node.
 *
 * A node is either a leaf, or has two other nodes (corresponding to the 'true'
 * branch and 'false' branch) as children and an associated question. The
 * question decides whether a specific example if forwarded to the true or
 * false branch of the node.
 *
 * NOTE: This class should not be altered!
 */
class Node {
  public:
    Node();
    explicit Node(Leaf l);
    Node(const Node& trueBranch, const Node& falseBranch, const Question &question);
    virtual ~Node() = default;

    const std::shared_ptr<Node> trueBranch() const { return trueBranch_; }
    const std::shared_ptr<Node> falseBranch() const { return falseBranch_; }
    const Question& question() const { return question_; }
    const std::shared_ptr<Leaf> leaf() const { return leaf_; }

  private:
    std::shared_ptr<Node> trueBranch_;
    std::shared_ptr<Node> falseBranch_;
    Question question_;
    std::shared_ptr<Leaf> leaf_;
};

#endif //DECISIONTREE_NODE_HPP
