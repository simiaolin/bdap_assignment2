/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include "Node.hpp"

using std::make_shared;

Node::Node() : trueBranch_(nullptr), falseBranch_(nullptr), question_({}), leaf_(nullptr) {}

Node::Node(Leaf l) : trueBranch_(nullptr), falseBranch_(nullptr), question_({}), leaf_(make_shared<Leaf>(l)) {}

Node::Node(const Node &trueBranch, const Node &falseBranch, const Question &question) :
    trueBranch_(make_shared<Node>(trueBranch)),
    falseBranch_(make_shared<Node>(falseBranch)),
    question_(question),
    leaf_(nullptr) {}
