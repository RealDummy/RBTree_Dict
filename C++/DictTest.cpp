#include "Dictionary.h"
#include <string>
#include <array>
#include <vector>

class base {
public:
	int a;
	int b;
};

class sub : public base{
	int c;
};

struct S {
	int val;
	S() {
		val = -1;
	};
	S(int i) : val{ i } {}
	S(const S& o) {
		val = o.val;
	}
	S(S&& o) {
		val = o.val;
		o.val = -1;
	}
	S& operator=(const S& rhs) {
		val = rhs.val;
		return *this;
	}
	
	friend bool operator<(const S& lhs, const S& rhs) {
		return lhs.val < rhs.val;
	}
	friend std::ostream&  operator<<(std::ostream& o, const S& s) {
		return o << s.val;
	}
};

enum tests {
	INDEX_TEST,
	CONATAINS_TEST,
	LOOKUP_TEST,
	CLEAR_TEST,
	SIZE_TEST,
	ERASE_TEST,
	ITERATOR_TEST,
	CONSTRUCTOR_TEST,
	NUM_TESTS,
};
typedef S valtype;
typedef Dictionary<int, valtype> Dict;

int run_test(int test, Dict D, const std::vector<valtype> vals) {
	switch (test) {
	case INDEX_TEST:
	{
		int i = 0;
		for (auto& v : vals) {
			if (v.val != D[i].val) return 1;
			++i;
		}
		if (D[vals.size()].val >= 0) return 2;
		D[vals.size()].val += 5;
		if ((D[vals.size()].val < 0)) return 3;

		return 0;
	}
	case CONATAINS_TEST:
	{
		for (size_t i = 0; i < vals.size(); ++i) {
			if (!D.contains(i)) return 1;
		}
		if (D.contains(vals.size())) return 2;
		return 0;
	}
	case LOOKUP_TEST:
	{
		int i = 0;
		for (auto& v : vals) {
			if (D.lookup(i).val != v.val) return 1;
			++i;
		}
		return 0;
	}
	case CLEAR_TEST:
	{
		D.clear();
		if (D.contains(0)) return 1;
		if (D.size() != 0) return 2;
		return 0;
	case SIZE_TEST:
		if (D.size() != vals.size()) return 1;
		D.insert(vals.size(), valtype(99));
		if (D.size() != vals.size() + 1) return 2;
		D[100] = valtype(50);
		if (D.size() != vals.size() + 2) return 3;
		D.erase(100);
		if (D.size() != vals.size() + 1) return 4;
		return 0;
	case ERASE_TEST:
		D.erase(0);
		if (D.contains(0)) return 1;
		return 0;
	}
	case ITERATOR_TEST:
	{
		auto it = begin(vals);
		int i = 0;
		for (auto& v : D) {
			if (v.second.val != (*it).val) return 1;
			if (v.first != i) return 2;
			++it;
			++i;
		}
		const auto& cD = D;
		it = begin(vals);
		i = 0;
		for (auto& v : cD) {
			if (v.second.val != (*it).val) return 3;
			if (v.first != i) return 4;
			++it;
			++i;
		}
		return 0;
	}
	case CONSTRUCTOR_TEST:
	{
		Dict D2(D);
		for (auto d = D.begin(), d2 = D2.begin(); d != D.end() && d2 != D2.end(); ++d, ++d2) {
			if (d->first != d2->first || d->second.val != d2->second.val) return 1;
		}

		Dict D3 = {
			{0,1},
			{1,2},
			{2,3},
			{3,10},
			{4,20},
			{5,30},
		};
		if (D3[3].val != 10) return 2;
		return 0;
	}
	default:
		std::cerr << "not a valid case number\n";
		return 0;
	}
}

int main() {
	{	
		std::array<std::string,NUM_TESTS> testNames = { 
			"INDEX_TEST",
			"CONATAINS_TEST",
			"LOOKUP_TEST",
			"CLEAR_TEST",
			"SIZE_TEST",
			"ERASE_TEST",
			"ITERATOR_TEST",
			"CONSTRUCTOR_TEST",
		};
		Dict D;
		std::vector<S> vals = { 1,2,3,10,20,30 };
		for (size_t i = 0; i < vals.size(); ++i) {
			D[i] = vals[i];
		}
		for (int i = 0; i < NUM_TESTS; ++i) {
			auto res = run_test(i, D, vals);
			std::cout << testNames[i] << ": ";
			if (res != 0) {
				std::cout << "Failed test " << res << "\n";
			}
			else {
				std::cout << "Passed\n";
			}
		}

	}
}