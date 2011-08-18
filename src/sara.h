#ifndef SARA_SARA_H_
#define SARA_SARA_H_

// Suffix array construction by induced sorting
// see: "Linear Suffix Array Construction by Almost Pure Induced-Sorting"
//      Nong, G., Zhang, S. and Chan, W. Data Compression Conference, 2009.

#include <cstddef>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>
#include <boost/range.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

namespace sara {

typedef std::ptrdiff_t index_t;

// Induced sorting
template <typename String, typename Indexes>
void induce_sa(const String& s,
               const std::vector<bool>& t,
               const std::vector<index_t>& buckets,
               Indexes& sa) {
  typedef typename std::make_unsigned<
    typename std::decay<decltype(s[0])>::type>::type ch_type;

  // sort L-type indexes
  std::vector<index_t> poss(buckets.begin(), buckets.end() - 1);

  auto back_idx = boost::size(s) - 1;
  sa[poss[ch_type(s[back_idx])]++] = back_idx;  // sentinel

  for (auto cur : sa) {
    if (--cur >= 0 && !t[cur]) {
      sa[poss[ch_type(s[cur])]++] = cur;
    }
  }

  // sort S-type indexes
  std::copy(buckets.begin() + 1, buckets.end(), poss.begin());
  for (auto cur : sa | boost::adaptors::reversed) {
    if (--cur >= 0 && t[cur]) {
      sa[--poss[ch_type(s[cur])]] = cur;
    }
  }
}

// Constructs suffix array by induced sorting.
// [s_begin, ..., s_begin + n) must not contain elements greater than |ch_end|.
template <typename CharIter, typename IndexIter>
void make(CharIter s_begin, IndexIter sa_begin, int n, int ch_max) {
  typedef typename std::make_unsigned<
    typename std::decay<decltype(*s_begin)>::type>::type ch_type;
  const int ch_end = ch_max + 1;

  auto sa = boost::make_iterator_range(sa_begin, sa_begin + n);
  auto s = boost::make_iterator_range(s_begin, s_begin + n);
  boost::fill(sa, -1);

  // Create L/S bit array |t|
  std::vector<bool> t(n);
  for (auto i = n - 1; i > 0; --i) {
    t[i - 1] = s[i - 1] == s[i] ? t[i]
                                : ch_type(s[i - 1]) < ch_type(s[i]);
  }

  auto is_not_lms = [&t](index_t i) { return !(i > 0 && t[i] && !t[i - 1]); };

  // Create buckets
  std::vector<index_t> buckets(ch_end + 1);
  for (ch_type ch : s) {
    ++buckets[ch + 1];
  }
  std::partial_sum(buckets.begin(), buckets.end(), buckets.begin());

  // Induced sorting of S-substrings
  std::vector<std::size_t> poss(buckets.begin() + 1, buckets.end());
  for (int i = 0; i < n; ++i) {
    if (!is_not_lms(i)) {
      sa[--poss[ch_type(s[i])]] = i;
    }
  }
  induce_sa(s, t, buckets, sa);

  // Remove S-substrings other than LMS-substrings
  auto lms_end = boost::remove_if(sa, is_not_lms);
  auto num_lms = lms_end - sa_begin;
  std::fill(lms_end, sa.end(), -1);

  // Get lexicographical names of all (sorted) LMS-substrings and create S1
  int cur_name = 0;
  for (auto it = sa_begin; it != lms_end; ++it) {
    if (it == sa_begin) {
      lms_end[*it / 2] = cur_name;
      continue;
    }
    auto cur = *it, prev = *(it - 1);
    bool miss = s[cur] != s[prev] || t[cur] != t[prev];
    if (!miss) do {
      ++cur, ++prev;
      miss |= cur == n ||
              prev == n ||
              s[cur] != s[prev] ||
              t[cur] != t[prev];
    } while (!miss && is_not_lms(cur) && is_not_lms(prev));

    if (miss) ++cur_name;
    lms_end[*it / 2] = cur_name;
  }
  const int num_names = cur_name + 1;
  std::remove_if(boost::rbegin(sa), boost::rbegin(sa) + (n - num_lms),
                 [](index_t idx) { return idx < 0; });

  // Recurse if LMS-substrings are not unique, otherwise generate SA1 directly
  // from S1
  auto s1 = sa | boost::adaptors::sliced(n - num_lms, n);
  if (num_names < num_lms) {
    make(s1.begin(), sa_begin, num_lms, num_names);
  } else {
    for (int i = 0; i < num_lms; ++i) {
      sa[s1[i]] = i;
    }
  }

  // Induce SA from SA1
  auto p1 = s1;
  boost::remove_copy_if(boost::irange(1, n), p1.begin(), is_not_lms);
  for (int i = 0; i < num_lms; ++i) {
    sa[i] = p1[sa[i]];
  }
  std::fill(sa.begin() + num_lms, sa.end(), -1);

  std::copy(buckets.begin() + 1, buckets.end(), poss.begin());
  for (auto i = num_lms; i-- > 0; ) {
    auto j = sa[i];
    sa[i] = -1;
    sa[--poss[ch_type(s[j])]] = j;
  }
  induce_sa(s, t, buckets, sa);
}

template <typename String>
std::vector<index_t> make(const String& s, int ch_max) {
  std::vector<index_t> sa(boost::size(s));
  make(boost::begin(s), sa.begin(), boost::size(s), ch_max);
  return sa;
}

}  // namespace sara

#endif  // SARA_SAIS_H_
