/*
 * Copyright (c) DTAI - KU Leuven – All rights reserved.
 * Proprietary, do not copy or distribute without permission. 
 * Written by Pieter Robberechts, 2019
 */

#ifndef DECISIONTREE_UTILS_HPP
#define DECISIONTREE_UTILS_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/timer/timer.hpp>


// You can change these data type aliases
using VecS = std::vector<std::string>;
using Data = std::vector<std::vector<std::string>>;
struct MetaData {
  VecS labels;
  // Here you can store additional meta data
};


namespace Utils::iterators {

  struct RetrieveKey {
    template<typename T>
      typename T::first_type operator()(T keyValuePair) const {
        return keyValuePair.first;
      }
  };

  struct AddMapValue {
    template<typename S, typename T>
      int operator()(const S previous, const T map) const {
        return previous + map.second;
      }
  };

  template<class InputIt, class T = typename std::iterator_traits<InputIt>::value_type>
    T mostCommon(InputIt begin, InputIt end)
    {
      std::map<T, int> counts;
      for (InputIt it = begin; it != end; ++it) {
        if (counts.find(*it) != counts.end()) {
          ++counts[*it];
        }
        else {
          counts[*it] = 1;
        }
      }
      return std::max_element(counts.begin(), counts.end(),
          [] (const std::pair<T, int>& pair1, const std::pair<T, int>& pair2) {
          return pair1.second < pair2.second;})->first;
    }

  template <typename It>
    auto average(It begin, It end) {
      assert(begin != end);
      auto s = std::accumulate(begin, end, 0.0);
      return s / std::distance(begin, end);
    };
}

namespace Utils::tree {

  template <typename T>
    int mapValueSum(std::unordered_map<T, int> counts) {
      return std::accumulate(begin(counts), std::end(counts), 0, iterators::AddMapValue());
    }

  template<typename T> 
    T getMax(std::unordered_map<T, int> counts ) {
      using pairtype = std::pair<T, int>; 
      std::pair<T, int> max = *std::max_element(counts.begin(), counts.end(), [] (const pairtype & p1, const pairtype & p2) {
          return p1.second < p2.second;
          }); 
      return max.first;
    }
}

namespace Utils::print {
  template<typename T>
    void print_vector(const std::vector<T> &vec) {
      if (vec.empty())
        return;

      std::cout << "{ ";
      std::copy(begin(vec), std::end(vec), std::ostream_iterator<T>(std::cout, " "));
      std::cout << "}\n";
    }

  template<typename K, typename V>
    void print_map(const std::unordered_map<K, V> &counter) {
      if (counter.empty())
        return;

      std::cout << "{ ";
      for (const auto& [key, val]: counter) {
        std::cout << key << ": " << val << " ";
      }
      std::cout << "}" << "\n";
    }
}

#endif //DECISIONTREE_UTILS_HPP
