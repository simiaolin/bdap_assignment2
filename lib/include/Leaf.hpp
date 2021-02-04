/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_LEAF_HPP
#define DECISIONTREE_LEAF_HPP

#include <string>
#include <unordered_map>
#include <vector>

// You can change these data types
using Data = std::vector<std::vector<std::string>>;
using ClassCounter = std::unordered_map<std::string, int>;


/**
 * Representation of a decision tree leaf node.
 *
 * A leaf stores the number of examples of each class that ended up in the
 * leaf during training.
 *
 * NOTE: This class should not be altered!
 */
class Leaf {
  public:
    Leaf() = delete;
    explicit Leaf(const ClassCounter cc);
    virtual ~Leaf() = default;

    inline const ClassCounter predictions() const { return predictions_; }

  private:
    const ClassCounter predictions_;

};

#endif //DECISIONTREE_LEAF_HPP
