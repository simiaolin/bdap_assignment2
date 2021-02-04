/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#include "TreeTest.hpp"

using std::make_shared;
using std::shared_ptr;

TreeTest::TreeTest(const Data& testData, const MetaData& meta, const Node &root) {
  test(testData, meta.labels, make_shared<Node>(root));
}

const ClassCounter TreeTest::classify(const VecS& row, shared_ptr<Node> node) const {
  if (bool is_leaf = node->leaf() != nullptr; is_leaf) {
    const auto &leaf = node->leaf();
    return leaf->predictions();
  }

  if (node->question().solve(row))
    return classify(row, node->trueBranch());
  else
    return classify(row, node->falseBranch());
}

void TreeTest::printLeaf(ClassCounter counts) const {
  const float total = static_cast<float>(Utils::tree::mapValueSum(counts));
  ClassCounterScaled scale;

  for (const auto& [key, val]: counts)
    scale[key] = std::to_string(val / total * 100) + "%";

  Utils::print::print_map(scale);
}

void TreeTest::test(const Data& testData, const VecS& labels, shared_ptr<Node> tree) const {
  float accuracy = 0;
  for (const auto& row: testData) {
    const auto& classification = classify(row, tree);
    static size_t last = row.size() - 1;
    // Comment out this line to print the predicion of each example
    // std::cout << "Actual: " << row[last] << "\tPrediction: "; printLeaf(classification);
    if (Utils::tree::getMax(classification) == row[last])
      accuracy += 1;
  }
  std::cout << "Total accuracy: " << (accuracy / testData.size()) << std::endl;
}
