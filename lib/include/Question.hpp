/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_QUESTION_HPP
#define DECISIONTREE_QUESTION_HPP

#include <string>
#include <vector>

using VecS = std::vector<std::string>;

/**
 * Representation of a "test" on an attritbute.
 *
 * NOTE: This class can be modified.
 */
class Question {
  public:
    Question();
    Question(const int column, const std::string value);

    const bool solve(VecS example) const;
    const bool isNumeric(std::string value) const;
    const bool isNumeric(void) const;
    const std::string toString(const VecS& labels) const;

    int column_;
    std::string value_;
};

#endif //DECISIONTREE_QUESTION_HPP
