/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include "Leaf.hpp"

Leaf::Leaf(const ClassCounter pred) : predictions_(std::move(pred)) {}
