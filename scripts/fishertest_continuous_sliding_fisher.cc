
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
// #include <boost/tokenizer.hpp>
#include <filesystem>
#include <stack>
#include <ctime>
// #include <boost/math/distributions/hypergeometric.hpp>
#include <algorithm>
#include <Rcpp.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
// #include <boost/filesystem.hpp>
#include <sys/stat.h>

using namespace Rcpp;

using namespace std;
// using namespace boost;
// using namespace boost::math;


// fuzz
#define SMALLISH_EPSILON 0.00000000003
#define SMALL_EPSILON 0.0000000000001

// This helps us avoid premature floating point overflow.
#define EXACT_TEST_BIAS 0.00000000000000000000000010339757656912845935892608650874535669572651386260986328125

double fisher22(uint32_t m11, uint32_t m12, uint32_t m21, uint32_t m22, uint32_t midp) {
  // Basic 2x2 Fisher exact test p-value calculation.
  double tprob = (1 - SMALL_EPSILON) * EXACT_TEST_BIAS;
  double cur_prob = tprob;
  double cprob = 0;
  int32_t tie_ct = 1;
  uint32_t uii;
  double cur11;
  double cur12;
  double cur21;
  double cur22;
  double preaddp;
  // Ensure we are left of the distribution center, m11 <= m22, and m12 <= m21.
  if (m12 > m21) {
    uii = m12;
    m12 = m21;
    m21 = uii;
  }
  if (m11 > m22) {
    uii = m11;
    m11 = m22;
    m22 = uii;
  }
  if ((((uint64_t)m11) * m22) > (((uint64_t)m12) * m21)) {
    uii = m11;
    m11 = m12;
    m12 = uii;
    uii = m21;
    m21 = m22;
    m22 = uii;
  }
  cur11 = m11;
  cur12 = m12;
  cur21 = m21;
  cur22 = m22;
  while (cur12 > 0.5) {
    cur11 += 1;
    cur22 += 1;
    cur_prob *= (cur12 * cur21) / (cur11 * cur22);
    cur12 -= 1;
    cur21 -= 1;
    if (cur_prob < EXACT_TEST_BIAS) {
      if (cur_prob > (1 - 2 * SMALL_EPSILON) * EXACT_TEST_BIAS) {
        tie_ct++;
      }
      tprob += cur_prob;
      break;
    }
    cprob += cur_prob;
    if (cprob == INFINITY) {
      return 0;
    }
  }
  if ((cprob == 0) && (!midp)) {
    return 1;
  }
  if (cur12 > 0.5) {
    do {
      cur11 += 1;
      cur22 += 1;
      cur_prob *= (cur12 * cur21) / (cur11 * cur22);
      cur12 -= 1;
      cur21 -= 1;
      preaddp = tprob;
      tprob += cur_prob;
      if (tprob <= preaddp) {
	break;
      }
    } while (cur12 > 0.5);
  }
  if (m11) {
    cur11 = m11;
    cur12 = m12;
    cur21 = m21;
    cur22 = m22;
    cur_prob = (1 - SMALL_EPSILON) * EXACT_TEST_BIAS;
    do {
      cur12 += 1;
      cur21 += 1;
      cur_prob *= (cur11 * cur22) / (cur12 * cur21);
      cur11 -= 1;
      cur22 -= 1;
      preaddp = tprob;
      tprob += cur_prob;
      if (tprob <= preaddp) {
        if (!midp) {
	  return preaddp / (cprob + preaddp);
        } else {
          return (preaddp - ((1 - SMALL_EPSILON) * EXACT_TEST_BIAS * 0.5) * tie_ct) / (cprob + preaddp);
        }
      }
    } while (cur11 > 0.5);
  }
  if (!midp) {
    return tprob / (cprob + tprob);
  } else {
    return (tprob - ((1 - SMALL_EPSILON) * EXACT_TEST_BIAS * 0.5) * tie_ct) / (cprob + tprob);
  }
}

double fisher22_1sided(uint32_t m11, uint32_t m12, uint32_t m21, uint32_t m22, uint32_t m11_is_greater_alt, uint32_t midp) {
  double cur_prob = EXACT_TEST_BIAS;
  double left_prob = cur_prob;
  double right_prob = 0;
  uint32_t uii;
  double cur11;
  double cur12;
  double cur21;
  double cur22;
  double preaddp;
  // Ensure m11 <= m22 and m12 <= m21.
  if (m12 > m21) {
    uii = m12;
    m12 = m21;
    m21 = uii;
  }
  if (m11 > m22) {
    uii = m11;
    m11 = m22;
    m22 = uii;
  }
  // Flipping m11<->m12 and m21<->m22 also flips the direction of the
  // alternative hypothesis.  So we flip on m11-is-greater alternative
  // hypothesis here to allow the rest of the code to assume m11-is-less.
  if (m11_is_greater_alt) {
    uii = m11;
    m11 = m12;
    m12 = uii;
    uii = m21;
    m21 = m22;
    m22 = uii;
  }
  cur11 = m11;
  cur12 = m12;
  cur21 = m21;
  cur22 = m22;
  if ((((uint64_t)m11) * m22) >= (((uint64_t)m12) * m21)) {
    // starting right of (or at) center, p > 0.5
    // 1. left_prob = sum leftward to precision limit
    // 2. total_prob := left_prob
    // 3. total_prob += sum rightward to total_prob precision limit
    // return left_prob / total_prob
    while (cur11 > 0.5) {
      cur12 += 1;
      cur21 += 1;
      cur_prob *= (cur11 * cur22) / (cur12 * cur21);
      cur11 -= 1;
      cur22 -= 1;
      preaddp = left_prob;
      left_prob += cur_prob;
      if (left_prob <= preaddp) {
	    break;
      }
      if (left_prob >= 1.0) {
        // Probability mass of our starting table was represented as 2^{-83},
        // so this would mean the left probability mass partial sum is greater
        // than 2^83 times that.  In which case the final p-value will
            // be indistinguishable from 1 at 53-bit precision if our input just
        // had 32-bit integers.  (Yes, the constant can be reduced.)
        return 1;
      }
    }
    cur11 = m11;
    cur12 = m12;
    cur21 = m21;
    cur22 = m22;
    cur_prob = EXACT_TEST_BIAS;
    right_prob = left_prob; // actually total_prob
    while (cur12 > 0.5) {
      cur11 += 1;
      cur22 += 1;
      cur_prob *= (cur12 * cur21) / (cur11 * cur22);
      cur12 -= 1;
      cur21 -= 1;
      preaddp = right_prob;
      right_prob += cur_prob;
      if (right_prob <= preaddp) {
	    break;
      }
    }
    if (!midp) {
      return left_prob / right_prob;
    } else {
      return (left_prob - EXACT_TEST_BIAS * 0.5) / right_prob;
    }
  } else {
    // starting left of center, p could be small
    // 1. right_prob = sum rightward to precision limit
    // 2. left_prob = sum leftward to left_prob precision limit
    // return left_prob / (left_prob + right_prob)
    while (cur12 > 0.5) {
      cur11 += 1;
      cur22 += 1;
      cur_prob *= (cur12 * cur21) / (cur11 * cur22);
      cur12 -= 1;
      cur21 -= 1;
      preaddp = right_prob;
      right_prob += cur_prob;
      if (right_prob == INFINITY) {
	    return 0;
      }
      if (right_prob <= preaddp) {
	    break;
      }
    }
    cur11 = m11;
    cur12 = m12;
    cur21 = m21;
    cur22 = m22;
    cur_prob = EXACT_TEST_BIAS;
    while (cur11 > 0.5) {
      cur12 += 1;
      cur21 += 1;
      cur_prob *= (cur11 * cur22) / (cur12 * cur21);
      cur11 -= 1;
      cur22 -= 1;
      preaddp = left_prob;
      left_prob += cur_prob;
      if (left_prob <= preaddp) {
	    break;
      }
    }
    if (!midp) {
      return left_prob / (left_prob + right_prob);
    } else {
      return (left_prob - EXACT_TEST_BIAS * 0.5) / (left_prob + right_prob);
    }
  }
}

double fisher22_oddsratio(uint32_t m11, uint32_t m12, uint32_t m21, uint32_t m22) {
    return ((uint64_t)m11) * m22 / 1.0 / (((uint64_t) m12) * m21);
}

std::vector<int> seqLen(int start, int end) {
    std::vector<int> result;

    if (start == end) {
        result.resize(end + 1);
        std::iota(result.begin(), result.end(), 1);
    } else if (start < end) {
        result.resize(end - start + 1);
        std::iota(result.begin(), result.end(), start);
    } else {
        result.resize(start - end + 1);
        std::iota(result.rbegin(), result.rend(), end);
    }

    return result;
}

std::vector<int> order(const std::vector<double>& arr, bool decreasing) {
    std::vector<int> idx(arr.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::function<bool(int, int)> cmp;
    if (decreasing) {
        cmp = [&arr](int a, int b) { return arr[b] < arr[a]; };
    } else {
        cmp = [&arr](int a, int b) { return arr[a] < arr[b]; };
    }

    std::stable_sort(idx.begin(), idx.end(), cmp);
    return idx;
}

std::vector<double> cummin(const std::vector<double>& arr) {
    if (arr.empty()) throw std::runtime_error("cummin requries at least one element");
    std::vector<double> output(arr.size());
    double cumulativeMin = arr[0];
    std::transform(arr.cbegin(), arr.cend(), output.begin(), [&cumulativeMin](double a) {
        if (a < cumulativeMin) cumulativeMin = a;
        return cumulativeMin;
    });
    return output;
}

std::vector<double> cummax(const std::vector<double>& arr) {
    if (arr.empty()) throw std::runtime_error("cummax requries at least one element");
    std::vector<double> output(arr.size());
    double cumulativeMax = arr[0];
    std::transform(arr.cbegin(), arr.cend(), output.begin(), [&cumulativeMax](double a) {
        if (cumulativeMax < a) cumulativeMax = a;
        return cumulativeMax;
    });
    return output;
}

std::vector<double> pminx(const std::vector<double>& arr, double x) {
    if (arr.empty()) throw std::runtime_error("pmin requries at least one element");
    std::vector<double> result(arr.size());
    std::transform(arr.cbegin(), arr.cend(), result.begin(), [&x](double a) {
        if (a < x) return a;
        return x;
    });
    return result;
}

/*
void doubleSay(const std::vector<double>& arr) {
    printf("[ 1] %.10f", arr[0]);
    for (size_t i = 1; i < arr.size(); ++i) {
        printf(" %.10f", arr[i]);
        if ((i + 1) % 5 == 0) printf("\n[%2d]", i + 1);
    }
}
*/

std::stack<clock_t> tictoc_stack;

void tic() {
    tictoc_stack.push(clock());
}

void toc(const string& str) {
    std::cout << str << "("
              << ((double)(clock() - tictoc_stack.top())) / CLOCKS_PER_SEC
              << ")" << std::endl;
    tictoc_stack.pop();
}


std::vector<double> pAdjust(const std::vector<double>& pvalues, const std::string& str) {
    if (pvalues.empty()) throw std::runtime_error("pAdjust requires at least one element");
    size_t size = pvalues.size();

    int type;
    if ("bh" == str || "fdr" == str) {
        type = 0;
    } else if ("by" == str) {
        type = 1;
    } else if ("bonferroni" == str) {
        type = 2;
    } else if ("hochberg" == str) {
        type = 3;
    } else if ("holm" == str) {
        type = 4;
    } else if ("hommel" == str) {
        type = 5;
    } else {
        throw std::runtime_error(str + " doesn't match any accepted FDR types");
    }

    // Bonferroni method
    if (2 == type) {
        std::vector<double> result(size);
        for (size_t i = 0; i < size; ++i) {
            double b = pvalues[i] * size;
            if (b >= 1) {
                result[i] = 1;
            } else if (0 <= b && b < 1) {
                result[i] = b;
            } else {
                throw std::runtime_error("a value is outside [0, 1)");
            }
        }
        return result;
    }
    // Holm method
    else if (4 == type) {
        auto o = order(pvalues, false);
        std::vector<double> o2Double(o.begin(), o.end());
        std::vector<double> cummaxInput(size);
        for (size_t i = 0; i < size; ++i) {
            cummaxInput[i] = (size - i) * pvalues[o[i]];
        }
        auto ro = order(o2Double, false);
        auto cummaxOutput = cummax(cummaxInput);
        auto pmin = pminx(cummaxOutput, 1.0);
        std::vector<double> result(size);
        std::transform(ro.cbegin(), ro.cend(), result.begin(), [&pmin](int a) { return pmin[a]; });
        return result;
    }
    // Hommel
    else if (5 == type) {
        auto indices = seqLen(size, size);
        auto o = order(pvalues, false);
        std::vector<double> p(size);
        std::transform(o.cbegin(), o.cend(), p.begin(), [&pvalues](int a) { return pvalues[a]; });
        std::vector<double> o2Double(o.begin(), o.end());
        auto ro = order(o2Double, false);
        std::vector<double> q(size);
        std::vector<double> pa(size);
        std::vector<double> npi(size);
        for (size_t i = 0; i < size; ++i) {
            npi[i] = p[i] * size / indices[i];
        }
        double min = *std::min_element(npi.begin(), npi.end());
        std::fill(q.begin(), q.end(), min);
        std::fill(pa.begin(), pa.end(), min);
        for (int j = size; j >= 2; --j) {
            auto ij = seqLen(1, size - j + 1);
            std::transform(ij.cbegin(), ij.cend(), ij.begin(), [](int a) { return a - 1; });
            int i2Length = j - 1;
            std::vector<int> i2(i2Length);
            for (int i = 0; i < i2Length; ++i) {
                i2[i] = size - j + 2 + i - 1;
            }
            double q1 = j * p[i2[0]] / 2.0;
            for (int i = 1; i < i2Length; ++i) {
                double temp_q1 = p[i2[i]] * j / (2.0 + i);
                if (temp_q1 < q1) q1 = temp_q1;
            }
            for (size_t i = 0; i < size - j + 1; ++i) {
                q[ij[i]] = std::min(p[ij[i]] * j, q1);
            }
            for (int i = 0; i < i2Length; ++i) {
                q[i2[i]] = q[size - j];
            }
            for (size_t i = 0; i < size; ++i) {
                if (pa[i] < q[i]) {
                    pa[i] = q[i];
                }
            }
        }
        std::transform(ro.cbegin(), ro.cend(), q.begin(), [&pa](int a) { return pa[a]; });
        return q;
    }

    std::vector<double> ni(size);
    std::vector<int> o = order(pvalues, true);
    std::vector<double> od(o.begin(), o.end());
    for (size_t i = 0; i < size; ++i) {
        if (pvalues[i] < 0 || pvalues[i]>1) {
            throw std::runtime_error("a value is outside [0, 1]");
        }
        ni[i] = (double)size / (size - i);
    }
    auto ro = order(od, false);
    std::vector<double> cumminInput(size);
    if (0 == type) {        // BH method
        for (size_t i = 0; i < size; ++i) {
            cumminInput[i] = ni[i] * pvalues[o[i]];
        }
    } else if (1 == type) { // BY method
        double q = 0;
        for (size_t i = 1; i < size + 1; ++i) {
            q += 1.0 / i;
        }
        for (size_t i = 0; i < size; ++i) {
            cumminInput[i] = q * ni[i] * pvalues[o[i]];
        }
    } else if (3 == type) { // Hochberg method
        for (size_t i = 0; i < size; ++i) {
            cumminInput[i] = (i + 1) * pvalues[o[i]];
        }
    }
    auto cumminArray = cummin(cumminInput);
    auto pmin = pminx(cumminArray, 1.0);
    std::vector<double> result(size);
    for (size_t i = 0; i < size; ++i) {
        result[i] = pmin[ro[i]];
    }
    return result;
}

template<typename T, typename K>
ostream& operator<<(ostream& os, const pair<T, K>& v) {
    return os << "(" << v.first << " " << v.second << ")";
}

template<typename K, typename V>
ostream& operator<<(ostream& os, const map<K, V>& v) {
    os << "[";
    for (const auto& vv : v)
        os << vv.first << ":" << vv.second << " ";
    return os << "]";
}

template<size_t I, typename... TS>
struct tuple_printer {
    static void print(ostream& out, const std::tuple<TS...> & t) {
        tuple_printer<I-1, TS...>::print(out, t);
        if (I < sizeof...(TS))
            out << ",";
        out << std::get<I>(t);
    }
};
template<typename... TS>
struct tuple_printer<0, TS...> {
    static void print(ostream& out, const std::tuple<TS...> & t) {
        out << std::get<0>(t);
    }
};
template<typename... TS>
std::ostream& operator<<(ostream& out, const std::tuple<TS...> & t) {
    out << "(";
    tuple_printer<sizeof...(TS) - 1, TS...>::print(out, t);
    return out << ")";
}

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "[";
    for (const auto& vv : v)
        os << vv << " ";
    return os << "]";
}

template<typename T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "[";
    for (const auto& vv : v)
        os << vv << " ";
    return os << "]";
}



# define assert_(EX) (void)((EX) || (__assert_(#EX, __FILE__, __LINE__),0))

void __assert_(string condition, string file, int line) {
    Rcerr << "Assertion failed '" << condition << "' on " << line << "@" << file << endl;
    throw std::runtime_error("Assertion failed");
}

template<typename Eval>
// in range [lb, ub) find first that eval is true for it.
int first_match(int lb, int ub, Eval eval) {
    assert_(lb <= ub);
    if (lb == ub)
        return ub;
    if (eval(lb)) return lb;
    while (lb+1 < ub) {
        //always eval(lb) == false
        int mid = (lb + ub)/2;
        if (eval(mid))
            ub = mid;
        else
            lb = mid;
    }
    // cerr << "first_match " << lb << " " << ub << " " << eval(lb) << endl;
    return lb + 1;
}

vector<tuple<int, string, double, double, double>> sliding_fisher(const map<string, int>& lineage_index, vector<pair<int,string>>& gisaid_data, int w, int step, double freq_threshold, const set<int> voc, Function p_adjust) {
    if (gisaid_data.size() < w*2){
        Rcerr << "Not enough data for chosen window size." << endl;
        // return(data.frame(lineage = character(), date = character(), pval = numeric(), odds = numeric(), freq = numeric(), stringsAsFactors = FALSE))
        return vector<tuple<int, string, double, double, double>>();
    }

    const double p_value_threshold = 0.05;

    // cerr << "sliding_fisher: " << gisaid_data.size() << endl;
    // vector<pair<int,string>> gisaid_data_temp;
    // copy_if(gisaid_data.begin(), gisaid_data.end(), back_inserter(gisaid_data_temp), [data](const pair<int,string>& i){return i.first == data.lineage_index.find("D.3")->second;});
    // cerr << "gisaid_data:" << gisaid_data_temp << endl;
    // cerr << "lineage_index: " << data.lineage_index << endl;
    // Rcerr << "filling index_lineage" << endl;
    map<int, string> index_lineage;
    for (auto const& i : lineage_index) {
        index_lineage[i.second] = i.first;
    }

    // Rcerr << "filling all_lineages" << endl;
    vector<int> all_lineages;
    for (auto const& i : gisaid_data) {
        if (i.first != -1)
            all_lineages.push_back(i.first);
    }
    sort(all_lineages.begin(), all_lineages.end());
    all_lineages.erase(unique(all_lineages.begin(), all_lineages.end()), all_lineages.end());
    sort(all_lineages.begin(), all_lineages.end(), [index_lineage](int a, int b) {return index_lineage.find(a)->second < index_lineage.find(b)->second;});

    map<int, int> lineage_to_index;
    for (size_t ll = 0; ll < all_lineages.size(); ll++) {
        lineage_to_index[all_lineages[ll]] = ll;
    }
    
    // vector<string> all_lineages_name;
    // for (auto const& i : all_lineages) {
    //     all_lineages_name.push_back(index_lineage[i]);
    // }
    // cerr << "Lineages " << all_lineages_name << endl;

    //for a given mut_this_season, calulate lower and upper bounds 
    // for a value of mut_this_season the value mut_prev_season sould be mut_prev_season < mut_this_season_thrshold_ub[mut_this_season]
    vector<int> mut_this_season_thrshold_ub;
    for (int mut_this_season = 0; mut_this_season <= w; mut_this_season++) {
        // cerr << "mut_this_season_thrshold_bounds " << mut_this_season << endl;
        // vector<int> idx(mut_this_season+1);
        // generate(idx.begin(), idx.end(), [i = 0]() mutable { return i++; });
        // auto lb1 = lower_bound(idx.begin(), idx.end(), p_value_threshold, [w, mut_this_season](int mut_prev_season, double val) {
        //     double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, w-mut_this_season, w-mut_prev_season, true, false);
        //     return pvalue < val;
        // });
        // assert(fisher22_1sided(mut_this_season, *lb1, w-mut_this_season, w-*lb1, true, false) >= p_value_threshold);
        // assert(lb1 == idx.begin() || fisher22_1sided(mut_this_season, *lb1-1, w-mut_this_season, w-*lb1-1, true, false) < p_value_threshold);

        int lb1 = first_match(0, mut_this_season+1, [w, mut_this_season, p_value_threshold](int mut_prev_season) {
            double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, w-mut_this_season, w-mut_prev_season, true, false);
            return pvalue >= p_value_threshold;
        });
        assert_(fisher22_1sided(mut_this_season, lb1, w-mut_this_season, w-lb1, true, false) >= p_value_threshold);
        assert_(lb1 == 0 || fisher22_1sided(mut_this_season, lb1-1, w-mut_this_season, w-lb1+1, true, false) < p_value_threshold);
        mut_this_season_thrshold_ub.push_back(lb1);

        // // idx = vector<int>(w-mut_this_season+1);
        // // generate(idx.begin(), idx.end(), [i = mut_this_season]() mutable { return i++; });
        // // auto lb2 = lower_bound(idx.begin(), idx.end(), p_value_threshold, [w, mut_this_season](int mut_prev_season, double val) {
        // //     double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, w-mut_this_season, w-mut_prev_season, true, false);
        // //     return pvalue >= val;
        // // });

        // // assert(lb2 == idx.end() || fisher22_1sided(mut_this_season, *lb2, w-mut_this_season, w-*lb2, true, false) < p_value_threshold);
        // // assert(lb2 == idx.begin() || fisher22_1sided(mut_this_season, *lb2-1, w-mut_this_season, w-*lb2-1, true, false) >= p_value_threshold);

        // // mut_this_season_thrshold_bounds.push_back(make_pair(*lb1, lb2 == idx.end() ? w + 1 : *lb2));

        // int lb2 = first_match(mut_this_season, w+1, [w, mut_this_season, p_value_threshold](int mut_prev_season) {
        //     double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, w-mut_this_season, w-mut_prev_season, true, false);
        //     cerr << "lb2 mut_prev_season=" << mut_prev_season << " " << pvalue << ":" << mut_this_season << " " << mut_prev_season << " " << w-mut_this_season << " " << w-mut_prev_season << endl;
        //     return pvalue < p_value_threshold;
        // });
        // assert(fisher22_1sided(mut_this_season, lb2, w-mut_this_season, w-lb2, true, false) < p_value_threshold);
        // assert(lb2 == w+1 || fisher22_1sided(mut_this_season, lb2-1, w-mut_this_season, w-lb2+1, true, false) >= p_value_threshold);

        // mut_this_season_thrshold_bounds.push_back(make_pair(lb1, lb2));
    }
    // Rcerr << "mut_this_season_thrshold_bounds: " << mut_this_season_thrshold_ub << endl;


    // Rcerr << "Filling pval_odd_freqs" << endl; 

    // map<pair<int,int>, tuple<int,int,int,int>> debug_mut_info;

    vector<vector<tuple<double, double, double, string>>> pval_odd_freqs;
    // // for (size_t l = 0; l < lineage_index.size(); l++) {
    // for (auto l : all_lineages) {
    //     //A[a] + ... + A[b-1] = cumsum[b] - cumsum[a]
    //     vector<int> cumsum;
    //     cumsum.push_back(0);
    //     for (const auto& lin_date : gisaid_data) {
    //         cumsum.push_back(cumsum.back() + (lin_date.first == l));
    //     }
    //     // if (cumsum.back() == 0) 
    //     //     continue;

    //     pval_odd_freqs.push_back(vector<tuple<double, double, double, string>>());

    //     // if (lineage_index.find("D.3")->second != l)
    //     //     continue;

    //     // vector<int> lineage_vector_indices;
    //     // for (auto it = gisaid_data.begin(); it != gisaid_data.end(); ++it) {
    //     //     if (it->first == l)
    //     //         lineage_vector_indices.push_back(it - gisaid_data.begin());
    //     // }
    //     // cerr << "lineage_vector_indices: " << lineage_vector_indices << endl;

    //     // (gisaid_data.size() - w - w + 1) / step
    //     for (int i = w; i + w <= gisaid_data.size(); i += step) {
    //         int start = i, end = i + w,
    //             prev_start = i - w, prev_end = i;


    //         int mut_this_season = cumsum[end] - cumsum[start],
    //             mut_prev_season = cumsum[prev_end] - cumsum[prev_start],
    //             nomut_this_season = end - start - mut_this_season,
    //             nomut_prev_season = prev_end - prev_start - mut_prev_season;

    //         debug_mut_info[make_pair(l, i)] = make_tuple(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season);

    //         double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season, true, false);
    //         double oddsratio = fisher22_oddsratio(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season);

    //         // cerr << "FT: " << l << " " << find_if(lineage_index.begin(), lineage_index.end(), [l](auto const& it) {return it.second == l;})->first << " " << i << " " << mut_this_season << " " << mut_prev_season << " " << nomut_this_season << " " << nomut_prev_season << " ft=" << pvalue << endl;

    //         pval_odd_freqs.back().push_back(make_tuple(pvalue, oddsratio, mut_this_season * 1.0 / w, gisaid_data[i+w].second));
    //     }

    // }

    // size_t size_2 = (gisaid_data.size() - w - w + 1) / step + 1, size_3 = 0;
    // assert_(pval_odd_freqs[0].size() == (gisaid_data.size() - w - w + 1) / step + 1);

    pval_odd_freqs = vector<vector<tuple<double, double, double, string>>>(all_lineages.size());
    map<int, int> lineage_mut_prev_season, lineage_mut_this_season;
    set<int> active_lineages;
    for (int i=-w+1; i+w <= gisaid_data.size(); i++) {
        // Rcerr << "lineage_mut_prev_season " << i << endl;
        if (i-w-1 >= 0) {
            lineage_mut_prev_season[gisaid_data[i-w-1].first]--;
        }
        if (i-1 >= 0) {
            lineage_mut_prev_season[gisaid_data[i-1].first]++;
            lineage_mut_this_season[gisaid_data[i-1].first]--;
        }
        lineage_mut_this_season[gisaid_data[i+w-1].first]++;

        auto update_active_linages = [&gisaid_data, &active_lineages, &lineage_mut_prev_season, &lineage_mut_this_season, &mut_this_season_thrshold_ub](int loc) {
            if (loc >= 0) {
                int l = gisaid_data[loc].first;
                if (lineage_mut_prev_season[l] < mut_this_season_thrshold_ub[lineage_mut_this_season[l]]) {
                    active_lineages.insert(l);
                } else {
                    if (active_lineages.find(l) != active_lineages.end())
                        active_lineages.erase(active_lineages.find(l));
                }
            }
        };

        update_active_linages(i-w-1);
        update_active_linages(i-1);
        update_active_linages(i+w-1);

        if (i >= w && (i-w) % step == 0) {
            // size_3++;
            // assert_(debug_mut_info.find(make_pair(all_lineages[0], i)) != debug_mut_info.end());
            // for (size_t ll = 0; ll < all_lineages.size(); ll++){
            //     int l = all_lineages[ll];
            //     int start = i, end = i + w,
            //         prev_start = i - w, prev_end = i;

            //     int mut_this_season = lineage_mut_this_season[l],
            //         mut_prev_season = lineage_mut_prev_season[l],
            //         nomut_this_season = end - start - mut_this_season,
            //         nomut_prev_season = prev_end - prev_start - mut_prev_season;

            //     auto x = debug_mut_info[make_pair(l, i)];
            //     assert_(mut_this_season == get<0>(x));
            //     assert_(mut_prev_season == get<1>(x));
            //     assert_(nomut_this_season == get<2>(x));
            //     assert_(nomut_prev_season == get<3>(x));

            //     double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season, true, false);
            //     double oddsratio = fisher22_oddsratio(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season);

            //     // cerr << "FT: " << l << " " << find_if(lineage_index.begin(), lineage_index.end(), [l](auto const& it) {return it.second == l;})->first << " " << i << " " << mut_this_season << " " << mut_prev_season << " " << nomut_this_season << " " << nomut_prev_season << " ft=" << pvalue << endl;

            //     if ((pvalue < 0.05) != (active_lineages.find(l) != active_lineages.end())) {
            //         Rcerr << "pv=" << pvalue << " " << (active_lineages.find(l) != active_lineages.end()) << " l=" << l << " i=" << i << " (" << lineage_mut_this_season[l] << " " << lineage_mut_prev_season[l] << ") " << active_lineages << endl;
            //     }
            //     assert_((pvalue < 0.05) == (active_lineages.find(l) != active_lineages.end()));

            //     pval_odd_freqs[ll].push_back(make_tuple(pvalue, oddsratio, mut_this_season * 1.0 / w));
            // }

            for (auto const& l : active_lineages) {
                int start = i, end = i + w,
                    prev_start = i - w, prev_end = i;

                int mut_this_season = lineage_mut_this_season[l],
                    mut_prev_season = lineage_mut_prev_season[l],
                    nomut_this_season = end - start - mut_this_season,
                    nomut_prev_season = prev_end - prev_start - mut_prev_season;

                // auto x = debug_mut_info[make_pair(l, i)];
                // assert_(mut_this_season == get<0>(x));
                // assert_(mut_prev_season == get<1>(x));
                // assert_(nomut_this_season == get<2>(x));
                // assert_(nomut_prev_season == get<3>(x));

                double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season, true, false);
                double oddsratio = fisher22_oddsratio(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season);

                // cerr << "FT: " << l << " " << find_if(lineage_index.begin(), lineage_index.end(), [l](auto const& it) {return it.second == l;})->first << " " << i << " " << mut_this_season << " " << mut_prev_season << " " << nomut_this_season << " " << nomut_prev_season << " ft=" << pvalue << endl;

                // if ((pvalue < 0.05) != (active_lineages.find(l) != active_lineages.end())) {
                //     Rcerr << "pv=" << pvalue << " " << (active_lineages.find(l) != active_lineages.end()) << " l=" << l << " i=" << i << " (" << lineage_mut_this_season[l] << " " << lineage_mut_prev_season[l] << ") " << active_lineages << endl;
                // }
                // assert_((pvalue < 0.05) == (active_lineages.find(l) != active_lineages.end()));

                pval_odd_freqs[lineage_to_index[l]].push_back(make_tuple(pvalue, oddsratio, mut_this_season * 1.0 / w, gisaid_data[i+w-1].second));
            }
            
        } else {
            // assert_(debug_mut_info.find(make_pair(all_lineages[0], i)) == debug_mut_info.end());
        }

        // if (debug_mut_info.find(make_pair(all_lineages[0], i)) != debug_mut_info.end()) {
        //     for (auto l : all_lineages) {
        //         auto x = debug_mut_info[make_pair(l, i)];
        //         if (get<0>(x) - get<1>(x) != lineage_two_w_diff[l]) {
        //             Rcerr << "BUG! " << i << " " << l << " " << lineage_two_w_diff << " x=" << x << endl;
        //         } else {
        //             // Rcerr << "OK " << i << " " << l << endl;
        //         }
        //     }
        // }
    }

    // assert_(size_2 == size_3);

    // Rcerr << "filling pvalues" << endl; 

    // cerr << "pvalues" << endl;
    vector<double> pvalues;
    for (auto const & pv_o_fs : pval_odd_freqs) {
        for (auto const & pv_o_f : pv_o_fs) {
            pvalues.push_back(std::get<0>(pv_o_f));
            // cerr << std::get<0>(pv_o_f) << " ";
        }
        // cerr << endl;
    }
    // cerr << "pValues = " << pvalues << endl;
    // auto q = pAdjust(pvalues, "by");
    NumericVector q2 = p_adjust(pvalues, _["method"] = "BY", _["n"] = ((gisaid_data.size() - w - w + 1) / step+1) * all_lineages.size());
    // for (size_t i = 0; i< q.size(); i++) {
    //     if (abs(q[i] - q2[i]) > 1e-5) {
    //         cerr << "BAD!!! " << q[i] << " " << q2[i] << " " << i << endl;
    //     }
    // }
    // cerr << "pAdjust " << q << endl;
    // cerr << "pAdjust" << endl;
    int idx = 0;
    for (auto & pv_o_fs : pval_odd_freqs) {
        for (auto & pv_o_f : pv_o_fs) {
            // pvalues.push_back(std::get<0>(pv_o_f));
            std::get<0>(pv_o_f) = q2[idx++];
            // cerr << std::get<0>(pv_o_f) << " ";
        }
        // cerr << endl;
    }

    // Rcerr << "filling sign_results" << endl;
    vector<tuple<int, string, double, double, double>> sign_results;
    for (size_t li=0 ; li<pval_odd_freqs.size(); li++) {
        int l = all_lineages[li];
        bool isvoc = voc.find(l) != voc.end();
        for (size_t k=0; k < pval_odd_freqs[li].size(); k++) {
            if (std::get<0>(pval_odd_freqs[li][k]) < p_value_threshold && 
                (isvoc || std::get<2>(pval_odd_freqs[li][k]) > freq_threshold)) {
                sign_results.push_back(make_tuple(l, 
                    // gisaid_data[w + w + k * step - 1].second, 
                    std::get<3>(pval_odd_freqs[li][k]), 
                    std::get<0>(pval_odd_freqs[li][k]), 
                    std::get<1>(pval_odd_freqs[li][k]), 
                    std::get<2>(pval_odd_freqs[li][k])));
                break;
            }
        }
    }
  
    return sign_results;
}


// [[Rcpp::export]]
void sliding_fisher_all_c(CharacterVector gisaid_data_all_pango_lineage, CharacterVector gisaid_data_all_country_code, CharacterVector gisaid_data_all_date, CharacterVector countries, int w, int step, double freq_threshold, CharacterVector voc, string output_folder, Function p_adjust) {

    // tic();
    map<string, int> lineage_index;
    map<string, int> country_index;
    vector<vector<pair<int, string>>> country_lineage_date;

    // cerr << "compressing " << gisaid_data_all_pango_lineage.size() << endl;
    for (size_t i = 0; i < gisaid_data_all_pango_lineage.size(); i++) {
        // string lin = row[column_index_map["pango_lineage"]],
        //     country = row[column_index_map["country_code"]];
        string lin = Rcpp::as<string>(gisaid_data_all_pango_lineage[i]), 
            country = Rcpp::as<string>(gisaid_data_all_country_code[i]),
            date = Rcpp::as<string>(gisaid_data_all_date[i]);
        // cerr << "r " << lin << " " << country << endl;
        if (lineage_index.find(lin) == lineage_index.end()) {
            if (lin != "" && lin != "None")
                lineage_index[lin] = lineage_index.size();
        }
        if (country_index.find(country) == country_index.end()) {
            country_index[country] = country_index.size();
            country_lineage_date.push_back(vector<pair<int, string>>());
        }
        country_lineage_date[country_index[country]].push_back(make_pair(lineage_index.find(lin) != lineage_index.end() ? lineage_index[lin] : -1, date));
    }

    // cerr << "country_index: " << country_index << endl;
    // cerr << "lineage_index: " << lineage_index << endl;


    // vector<string> voc {"B.1.1.7", "B.1.351", "P.1", "B.1.617", "B.1.617.1", "B.1.617.2", "B.1.617.3"};
    set<int> voc_index;
    for (auto const& v : voc) {
        voc_index.insert(lineage_index[Rcpp::as<string>(v)]);
    }

    // int w = 1000, 
    //     step = 100;

    for (auto & country_: countries) {
        string country = Rcpp::as<string>(country_);
        tic();
        // cerr << "Country: " << country << endl;
        vector<tuple<int, string, double, double, double>> sign_results = 
            sliding_fisher(lineage_index, country_lineage_date[country_index[country]], 
                w, step, freq_threshold, voc_index, p_adjust);

        string output_path = output_folder + "/" + country;
        // std::experimental::filesystem::create_directories(output_path.c_str());
        // if (!boost::filesystem::is_directory(output_path.c_str()))
        //     boost::filesystem::create_directories(output_path.c_str());
        struct stat info;
        if( stat(output_path.c_str(), &info ) != 0 ) {
            mkdir(output_path.c_str(), 0770);
        }

        map<int, string> lineage_index_to_name;
        for (auto const& l : lineage_index)
            lineage_index_to_name[l.second] = l.first;

        
        ofstream fo(output_path + "/" + country + "_sign_continuous.txt");
        fo << "lineage" << "\t" << "date" << "\t" << "pval" << "\t" << "odds" << "\t" << "freq" << endl;
        for (auto const i : sign_results) {
            fo << lineage_index_to_name[get<0>(i)] << "\t" << get<1>(i) << "\t" <<setprecision(17) << get<2>(i) << "\t" << get<3>(i) << "\t" << get<4>(i) << endl;
        }
        // std::cerr << "file: " << output_path + "/" + country + "_sign_continuous.txt" << endl;

        toc(country);
           
    }
    // toc("Fisher C++ ");

}

// struct Data {

//     vector<vector<string>> data;
//     vector<int> column_index;
//     vector<string> columns{"gisaid_epi_isl", "date", "country", "division", "host", "pango_lineage"};
//     map<string, int> column_index_map;
//     vector<string> countries;

//     bool check_date(string date) {
//         //"20..-..-.."
//         // cerr << "D" << date << " " << (date.length() == 10 && date[0] == '2' && date[1] == '0' && date[4] == '-' && date[7] == '-') << endl;
//         return date.length() == 10 && date[0] == '2' && date[1] == '0' && date[4] == '-' && date[7] == '-';
//     }

//     int find_column_index(vector<string>& vec, string name) {
//         return find(vec.begin(), vec.end(), name) - vec.begin();;
//     }

//     void load(string fn) {
//         ifstream fi(fn);
//         int cnt = 0;
//         for (string line; getline(fi, line); cnt++) {
//             tokenizer<escaped_list_separator<char> > tk(line, escaped_list_separator<char>('\\', '\t', '\"'));
//             std::vector<std::string> vec (tk.begin(), tk.end());

//             // cerr << "Data line " << cnt << vec << endl;

//             if (cnt == 0) {
//                 for (const string& n : columns) {
//                     // cerr << "column_index: " << n << " " << (find(vec.begin(), vec.end(), n) - vec.begin()) << endl;
//                     column_index.push_back(find(vec.begin(), vec.end(), n) - vec.begin());
//                 }
//                 cerr << "column_index: " << column_index << " " << columns << endl;
//             } else {
//                 data.push_back(vector<string>());
//                 for (int i : column_index) {
//                     data.back().push_back(vec[i]);
//                 }
//             }
//         }
//         for (size_t i=0; i<columns.size(); i++) {
//             column_index_map[columns[i]] = i;
//         }

//         cerr << "Filling country codes " << endl;

//         map<string, string> country_code_map = {{"Afghanistan","AF"}, {"Albania","AL"}, 
//             {"Algeria","DZ"}, {"Andorra","AD"}, {"Angola","AO"}, 
//             {"Antigua and Barbuda","AG"}, {"Argentina","AR"}, {"Armenia","AM"}, 
//             {"Aruba","AW"}, {"Australia","AU"}, {"Austria","AT"}, {"Azerbaijan","AZ"}, 
//             {"Bahamas","BS"}, {"Bahrain","BH"}, {"Bangladesh","BD"}, {"Barbados","BB"}, {"Belarus","BY"}, 
//             {"Belgium","BE"}, {"Belize","BZ"}, {"Benin","BJ"}, {"Bermuda","BM"}, {"Bolivia","BO"}, 
//             {"Bonaire","Bonaire"}, {"Bosnia and Herzegovina","BA"}, {"Botswana","BW"}, {"Brazil","BR"}, 
//             {"Brunei","BN"}, {"Bulgaria","BG"}, {"Burkina Faso","BF"}, {"Cabo Verde","CV"}, {"Cambodia","KH"}, 
//             {"Cameroon","CM"}, {"Canada","CA"}, {"Central African Republic","CF"}, {"Chile","CL"}, {"China","CN"}, 
//             {"Colombia","CO"}, {"Costa Rica","CR"}, {"Croatia","HR"}, {"Curacao","CW"}, {"Cyprus","CY"}, 
//             {"Czech Republic","CZ"}, {"Côte d'Ivoire","CI"}, {"Democratic Republic of the Congo","CD"}, 
//             {"Denmark","DK"}, {"Djibouti","DJ"}, {"Dominica","DM"}, {"Dominican Republic","DO"}, {"Ecuador","EC"}, 
//             {"Egypt","EG"}, {"Equatorial Guinea","GQ"}, {"Estonia","EE"}, {"Eswatini","SZ"}, {"Ethiopia","ET"}, 
//             {"Fiji","FJ"}, {"Finland","FI"}, {"France","FR"}, {"French Polynesia","PF"}, {"Gabon","GA"}, 
//             {"Gambia","GM"}, {"Georgia","GE"}, {"Germany","DE"}, {"Ghana","GH"}, {"Greece","GR"}, {"Grenada","GD"}, 
//             {"Guadeloupe","GP"}, {"Guatemala","GT"}, {"Guinea","GN"}, {"Guinea-Bissau","GW"}, {"Guyana","GY"}, 
//             {"Haiti","HT"}, {"Honduras","HN"}, {"Hong Kong","HK"}, {"Hungary","HU"}, {"Iceland","IS"}, {"India","IN"}, 
//             {"Indonesia","ID"}, {"Iran","IR"}, {"Iraq","IQ"}, {"Ireland","IE"}, {"Israel","IL"}, {"Italy","IT"}, 
//             {"Jamaica","JM"}, {"Japan","JP"}, {"Jordan","JO"}, {"Kazakhstan","KZ"}, {"Kenya","KE"}, {"Kosovo","Kosovo"}, 
//             {"Kuwait","KW"}, {"Kyrgyzstan","KG"}, {"Latvia","LV"}, {"Lebanon","LB"}, {"Lesotho","LS"}, {"Libya","LY"}, 
//             {"Liechtenstein","LI"}, {"Lithuania","LT"}, {"Luxembourg","LU"}, {"Madagascar","MG"}, {"Malawi","MW"}, 
//             {"Malaysia","MY"}, {"Maldives","MV"}, {"Mali","ML"}, {"Malta","MT"}, {"Mauritius","MU"}, {"Mexico","MX"}, 
//             {"Moldova","MD"}, {"Monaco","MC"}, {"Mongolia","MN"}, {"Montenegro","ME"}, {"Morocco","MA"}, 
//             {"Mozambique","MZ"}, {"Myanmar","MM"}, {"Namibia","NA"}, {"Nepal","NP"}, {"Netherlands","NL"}, 
//             {"New Zealand","NZ"}, {"Nicaragua","NI"}, {"Niger","NE"}, {"Nigeria","NG"}, {"North Macedonia","MK"}, 
//             {"Norway","NO"}, {"Oman","OM"}, {"Pakistan","PK"}, {"Palestine","PS"}, {"Panama","PA"}, 
//             {"Papua New Guinea","PG"}, {"Paraguay","PY"}, {"Peru","PE"}, {"Philippines","PH"}, {"Poland","PL"}, 
//             {"Portugal","PT"}, {"Qatar","QA"}, {"Republic of the Congo","CG"}, {"Romania","RO"}, {"Russia","RU"}, 
//             {"Rwanda","RW"}, {"Saint Barthélemy","BL"}, {"Saint Kitts and Nevis","KN"}, {"Saint Lucia","LC"}, 
//             {"Saint Martin", "Saint Martin"}, {"Saint Vincent and the Grenadines","VC"}, {"Saudi Arabia","SA"}, 
//             {"Senegal","SN"}, {"Serbia","RS"}, {"Seychelles","SC"}, {"Sierra Leone","SL"}, {"Singapore","SG"}, 
//             {"Sint Maarten","SX"}, {"Slovakia","SK"}, {"Slovenia","SI"}, {"Solomon Islands","SB"}, {"Somalia","SO"}, 
//             {"South Africa","ZA"}, {"South Korea","KR"}, {"South Sudan","SS"}, {"Spain","ES"}, {"Sri Lanka","LK"}, 
//             {"Sudan","SD"}, {"Suriname","SR"}, {"Sweden","SE"}, {"Switzerland","CH"}, {"Taiwan","TW"}, 
//             {"Tanzania","TZ"}, {"Thailand","TH"}, {"Timor-Leste","TL"}, {"Togo","TG"}, {"Trinidad and Tobago","TT"}, 
//             {"Tunisia","TN"}, {"Turkey","TR"}, {"USA","US"}, {"Uganda","UG"}, {"Ukraine","UA"}, 
//             {"Union of the Comoros","KM"}, {"United Arab Emirates","AE"}, {"United Kingdom","GB"}, {"Uruguay","UY"}, 
//             {"Uzbekistan","UZ"}, {"Venezuela","VE"}, {"Vietnam","VN"}, {"Zambia","ZM"}, {"Zimbabwe","ZW"}};

//         columns.push_back("country_code");

//         column_index_map[columns.back()] = column_index_map.size();

//         for (auto & row : data) {
//             string code = (country_code_map.find(row[column_index_map["country"]]) != country_code_map.end()) ? 
//                 country_code_map[row[column_index_map["country"]]] : "";
//             row.push_back(code);
//         }

//         cerr << "Filtering data " << column_index_map << endl;

//         data.erase(
//             remove_if(data.begin(), data.end(), 
//                 [this](const vector<string>& row) {
//                     return row[column_index_map["host"]] != "Human" || 
//                         row[column_index_map["country_code"]] == "" || 
//                         row[column_index_map["pango_lineage"]].find('/') != string::npos;
//                 }
//             ), data.end()
//         );

//         cerr << "Counting countries " << endl;

//         map<string, int> country_count;
//         for (auto const & row: data) {
//             // cerr << row[column_index_map["country_code"]] << " ";
//             country_count[row[column_index_map["country_code"]]]++;
//         }

//         cerr << "Filter countries " << endl;
//         for (auto& cc: country_count) {
//             if (cc.second > 2000)
//                 countries.push_back(cc.first);
//         }

//         cerr << "Selected countries " << countries << endl;

//         data.erase(
//             remove_if(data.begin(), data.end(), 
//                 [this](const vector<string>& row) {
//                     return !check_date(row[column_index_map["date"]]);
//                 }
//             ), data.end()
//         );

//         cerr << "Sorting data " << data.size() << endl;

//         sort(data.begin(), data.end(), [this](const vector<string>& a, const vector<string>& b) {
//             return a.at(column_index_map["date"]) < b.at(column_index_map["date"]); 
//         });

//         cerr << "Sorting done " << data.size() << endl;
//     }

//     map<string, int> lineage_index;
//     map<string, int> country_index;
//     vector<vector<pair<int, string>>> country_lineage_date;
//     void compress() {
//         cerr << "compressing " << data.size() << endl;
//         for (auto const& row: data) {
//             string lin = row[column_index_map["pango_lineage"]],
//                 country = row[column_index_map["country_code"]];
//             // cerr << "r " << lin << " " << country << endl;
//             if (lineage_index.find(lin) == lineage_index.end()) {
//                 if (lin != "" && lin != "None")
//                     lineage_index[lin] = lineage_index.size();
//             }
//             if (country_index.find(country) == country_index.end()) {
//                 country_index[country] = country_index.size();
//                 country_lineage_date.push_back(vector<pair<int, string>>());
//             }
//             country_lineage_date[country_index[country]].push_back(make_pair(lineage_index.find(lin) != lineage_index.end() ? lineage_index[lin] : -1, row[column_index_map["date"]]));
//         }

//         cerr << "country_index: " << country_index << endl;
//         cerr << "lineage_index: " << lineage_index << endl;
//     }

// };

// vector<tuple<int, string, double, double, double>> sliding_fisher(const Data& data, vector<pair<int,string>>& gisaid_data, int w, int step, double freq_threshold, const set<int> voc) {
//     if (gisaid_data.size() < w*2){
//         cerr << "Not enough data for chosen window size." << endl;
//         // return(data.frame(lineage = character(), date = character(), pval = numeric(), odds = numeric(), freq = numeric(), stringsAsFactors = FALSE))
//         return vector<tuple<int, string, double, double, double>>();
//     }

//     cerr << "sliding_fisher: " << gisaid_data.size() << endl;

//     vector<pair<int,string>> gisaid_data_temp;
//     copy_if(gisaid_data.begin(), gisaid_data.end(), back_inserter(gisaid_data_temp), [data](const pair<int,string>& i){return i.first == data.lineage_index.find("D.3")->second;});


//     cerr << "gisaid_data:" << gisaid_data_temp << endl;

//     // cerr << "lineage_index: " << data.lineage_index << endl;

//     vector<vector<tuple<double, double, double>>> pval_odd_freqs;
//     for (size_t l = 0; l < data.lineage_index.size(); l++) {
//         pval_odd_freqs.push_back(vector<tuple<double, double, double>>());
//         //A[a] + ... + A[b-1] = cumsum[b] - cumsum[a]
//         vector<int> cumsum;
//         cumsum.push_back(0);
//         for (const auto& lin_date : gisaid_data) {
//             cumsum.push_back(cumsum.back() + (lin_date.first == l));
//         }
//         if (cumsum.back() == 0) 
//             continue;

//         if (data.lineage_index.find("D.3")->second != l)
//             continue;

//         vector<int> lineage_vector_indices;
//         for (auto it = gisaid_data.begin(); it != gisaid_data.end(); ++it) {
//             if (it->first == l)
//                 lineage_vector_indices.push_back(it - gisaid_data.begin());
//         }
//         cerr << "lineage_vector_indices: " << lineage_vector_indices << endl;

//         for (int i = w; i + w <= gisaid_data.size(); i += step) {
//             int start = i, end = i + w,
//                 prev_start = i - w, prev_end = i;


//             int mut_this_season = cumsum[end] - cumsum[start],
//                 mut_prev_season = cumsum[prev_end] - cumsum[prev_start],
//                 nomut_this_season = end - start - mut_this_season,
//                 nomut_prev_season = prev_end - prev_start - mut_prev_season;

//             double pvalue = fisher22_1sided(mut_this_season, mut_prev_season, nomut_this_season, nomut_prev_season, true, false);

//             cerr << "FT: " << l << " " << find_if(data.lineage_index.begin(), data.lineage_index.end(), [l](auto const& it) {return it.second == l;})->first << " " << i << " " << mut_this_season << " " << mut_prev_season << " " << nomut_this_season << " " << nomut_prev_season << " ft=" << pvalue << endl;

//             pval_odd_freqs.back().push_back(make_tuple(pvalue, 1.0, mut_this_season * 1.0 / w));
//         }

//     }

//     vector<double> pvalues;
//     for (auto const & pv_o_fs : pval_odd_freqs) {
//         for (auto const & pv_o_f : pv_o_fs) {
//             pvalues.push_back(std::get<0>(pv_o_f));
//         }
//     }
//     auto q = pAdjust(pvalues, "by");
//     int idx = 0;
//     for (auto & pv_o_fs : pval_odd_freqs) {
//         for (auto & pv_o_f : pv_o_fs) {
//             // pvalues.push_back(std::get<0>(pv_o_f));
//             std::get<0>(pv_o_f) = q[idx++];
//         }
//     }

//     vector<tuple<int, string, double, double, double>> sign_results;

//     for (size_t l=0 ; l< pval_odd_freqs.size(); l++) {
//         bool isvoc = voc.find(l) != voc.end();
//         for (size_t k=0; k < pval_odd_freqs[l].size(); k++) {
//             if (std::get<0>(pval_odd_freqs[l][k]) < p_value_threshold && (isvoc || std::get<2>(pval_odd_freqs[l][k]) > freq_threshold)) {
//                 sign_results.push_back(make_tuple(l, gisaid_data[w + k * step].second, std::get<0>(pval_odd_freqs[l][k]), std::get<1>(pval_odd_freqs[l][k]), std::get<2>(pval_odd_freqs[l][k])));
//             }
//         }
//     }
  
//     return sign_results;
// }

// int main(int argc, char* argv[]) {
//     string file = argv[1];
//     double freq_threshold = stod(argv[2]);
//     string output_folder = argv[3];

//     tic();

//     Data data;
//     data.load(file);
//     data.compress();

//     vector<string> voc {"B.1.1.7", "B.1.351", "P.1", "B.1.617", "B.1.617.1", "B.1.617.2", "B.1.617.3"};
//     set<int> voc_index;
//     for (auto const& v : voc) {
//         voc_index.insert(data.lineage_index[v]);
//     }
//     toc("Loading data");

//     int w = 1000, 
//         step = 100;

//     data.countries = {"AU"};
//     cerr << "country_index: " << data.country_index << endl;
//     for (auto & country: data.countries) {
//         tic();
//         // cerr << "Country: " << country << endl;
//         vector<tuple<int, string, double, double, double>> sign_results = sliding_fisher(data, data.country_lineage_date[data.country_index[country]], 
//             w, step, freq_threshold, voc_index);

//         string output_path = output_folder + "/" + country;
//         std::filesystem::create_directories(output_path.c_str());
        
//         ofstream fo(output_path + "/" + country + "_sign_continuous.txt");
//         fo << "lineage" << "\t" << "date" << "\t" << "pval" << "\t" << "odds" << "\t" << "freq" << endl;
//         for (auto const i : sign_results) {
//             fo << get<0>(i) << "\t" << get<1>(i) << "\t" << get<2>(i) << "\t" << get<3>(i) << "\t" << get<4>(i) << endl;
//         }
//         toc(country);
           
//     }


//     return 0;
// }
