// Copyright (c) 2011 Seiya Tokui <beam.web@gmail.com>. All Rights Reserved.
// This source code is distributed under MIT License in LICENSE file.

#include <climits>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "sara.h"
using namespace std;

namespace sara {

class sara_string_test
    : public ::testing::TestWithParam<pair<string, vector<ptrdiff_t>>> {
};

TEST_P(sara_string_test, string) {
  auto params = GetParam();

  auto sa = make(params.first, UCHAR_MAX);
  EXPECT_EQ(params.second, sa);
}

TEST_P(sara_string_test, pstr) {
  auto params = GetParam();

  auto size = params.first.size();
  vector<ptrdiff_t> sa(size);
  make(params.first.c_str(), sa.begin(), size, UCHAR_MAX);
  EXPECT_EQ(params.second, sa);
}

TEST_P(sara_string_test, vector_char) {
  auto params = GetParam();
  auto str = params.first;

  vector<char> vchar(str.begin(), str.end());
  auto sa = make(vchar, UCHAR_MAX);
  EXPECT_EQ(params.second, sa);
}

pair<string, vector<ptrdiff_t>> sara_string_test_data[] = {
  { "mmiissiissiippii",
    { 15, 14, 10, 6, 2, 11, 7, 3, 1, 0, 13, 12, 9, 5, 8, 4 } },
  { "mmiissiippiissii",
    { 15, 14, 6, 10, 2, 7, 11, 3, 1, 0, 9, 8, 13, 5, 12, 4 } },
  { "abracadabra", { 10, 7, 0, 3, 5, 8, 1, 4, 6, 9, 2 } },
  { "aeadacab", { 6, 4, 2, 0, 7, 5, 3, 1 } },
  { "abracadabra0AbRa4Cad14abra",
    { 11, 20, 16, 21, 12, 17, 14, 25, 10, 15, 22, 7, 0, 3, 18, 5, 13, 23, 8, 1,
      4, 19, 6, 24, 9, 2} }
};

INSTANTIATE_TEST_CASE_P(sample1, sara_string_test,
                        ::testing::ValuesIn(sara_string_test_data));

TEST(sara_test, integer_array) {
  int query[] = { 1, 1, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 2, 2, 0, 0 };
  vector<ptrdiff_t> answer =
      { 15, 14, 10, 6, 2, 11, 7, 3, 1, 0, 13, 12, 9, 5, 8, 4 };

  auto sa = make(query, 4);
  EXPECT_EQ(answer, sa);
}

TEST(sara_test, char_array) {
  auto sa = make("mmiissiissiippii", UCHAR_MAX);
  vector<ptrdiff_t> answer =
      { 16, 15, 14, 10, 6, 2, 11, 7, 3, 1, 0, 13, 12, 9, 5, 8, 4 };

  EXPECT_EQ(answer, sa);
}

}  // namespace sara
