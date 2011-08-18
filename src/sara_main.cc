#include <climits>
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/irange.hpp>
#include <elog/benchmark.h>
#include "sara.h"
using namespace std;

bool sufcomp(const string& str, size_t i, size_t j) {
  auto begin = reinterpret_cast<const unsigned char*>(&str[0]),
      end = reinterpret_cast<const unsigned char*>(&str.back() + 1);
  return lexicographical_compare(begin + i, end, begin + j, end);
}

void check(const string& str, size_t i, size_t j) {
  if (!sufcomp(str, i, j)) {
    cerr << i << ',' << flush;
  }
}

vector<ptrdiff_t> naive_make(const string& s) {
  vector<ptrdiff_t> sa(s.size());
  boost::copy(boost::irange(0UL, s.size()), sa.begin());

  std::sort(sa.begin(), sa.end(),
            [&s](ptrdiff_t i, ptrdiff_t j) { return sufcomp(s, i, j); });

  return sa;
}

int main() {
  string input;
  for (string line; getline(cin, line); ) {
    input += line;
    input += '\n';
  }

  vector<sara::index_t> sa;

  BENCHMARK(sara_all) {
    sa = sara::make(input, UCHAR_MAX);
  }

  BENCHMARK(naive_all) {
    (void)naive_make(input);
  }

  for (auto suffix : sa) {
    cout << suffix << endl;
  }

  // check
  cerr << "error: ";
  for (size_t i = 1; i < sa.size(); ++i) {
    check(input, sa[i-1], sa[i]);
  }
  cerr << endl;
  sort(sa.begin(), sa.end());
  auto it = unique(sa.begin(), sa.end());
  if (it != sa.end()) {
    cerr << "recall down!" << endl;
  }
}
