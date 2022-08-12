#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#define Vin(V,i) ((bool)((static_cast<unsigned __int64>(1)<<(i-1))&V))
#define Vpop(V,i) (V&(~(static_cast<unsigned __int64>(1)<<(i-1))))
#define Vput(V,i) (V|(static_cast<unsigned __int64>(1)<<(i-1)))
using namespace std;
namespace py = pybind11;
double** x, ** dp;
int n, ** route;

template<class T>
ostream& operator<<(ostream& os, const vector<T>& x) {
	os << "[";
	for (auto& i : x) {
		os << i << ", ";
	}
	os << "]";
	return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& x) {
	os << "{";
	for (auto& i : x) {
		os << i << ", ";
	}
	os << "}";
	return os;
}
template<class T>
ostream& operator<<(ostream& os, const unordered_set<T>& x) {
	os << "{";
	for (auto& i : x) {
		os << i << ", ";
	}
	os << "}";
	return os;
}
template<class T1, class T2>
ostream& operator<<(ostream& os, const unordered_map<T1, T2>& x) {
	os << "{";
	for (auto& i : x) {
		os << i.first << ": " << i.second << ", ";
	}
	os << "}";
	return os;
}
template<class Tuple, size_t N>
struct PrintTuple
{
	static void Printf(const Tuple& Value, ostream& os)
	{
		PrintTuple<Tuple, N - 1>::Printf(Value, os);
		os << get<N - 1>(Value) << ", ";
	}
};
template<class Tuple>
struct PrintTuple<Tuple, 1>
{
	static void Printf(const Tuple& Value, ostream& os)
	{
		os << get<0>(Value) << ", ";
	}
};
template<class... Args>
void PrintfMyTuple(const tuple<Args...>& vlaue, ostream& os) {
	PrintTuple<decltype(vlaue), sizeof...(Args)>::Printf(vlaue, os);
}
template<class ...T1>
ostream& operator<<(ostream& os, const tuple<T1...>& x) {
	os << "(";
	PrintfMyTuple(x, os);
	os << ")";
	return os;
}
double d(int i, long long int V) {
	if (!V) {
		dp[i][V] = x[i][0];
		route[i][V] = 0;
		return x[i][0];
	}
	if (dp[i][V] >= 0) {
		return dp[i][V];
	}
	double recent_min = DBL_MAX, tmp;
	int chosen_k = -1;
	for (int k = 0; k < n; ++k) {
		if (Vin(V, k)) {
			tmp = x[i][k] + d(k, Vpop(V, k));
			if (tmp < recent_min) {
				recent_min = tmp;
				chosen_k = k;
			}
		}
	}
	dp[i][V] = recent_min;
	route[i][V] = chosen_k;
	return recent_min;
}
vector<int> solve(vector<vector<double>> _x) {
	n = _x.size();
	if (n > 64)throw py::value_error("Error: num of points too large causing long long int overflow.");
	x = new double* [n];
	dp = new double* [n];
	route = new int* [n];
	long long int tmp = static_cast < unsigned __int64>(1) << (n - 1);
	for (int i = 0; i < n; ++i) {
		x[i] = new double[n];
		for (int j = 0; j < n; ++j) {
			x[i][j] = _x[i][j];
		}
		dp[i] = new double[tmp];
		route[i] = new int[tmp];
		for (long long int j = 0; j < tmp; ++j) {
			dp[i][j] = -1;
			route[i][j] = -1;
		}
	}
	_x.clear();
	int start = 0;
	long long V = (static_cast<unsigned __int64>(1) << (n - 1)) - 1;
	double dret = d(start, V);
	vector<int> ret;
	ret.push_back(start); start = route[start][V]; V = Vpop(V, start);
	while (start) {
		ret.push_back(start); start = route[start][V]; V = Vpop(V, start);
	}

	for (int i = 0; i < n; ++i) {
		delete[] x[i];
		delete[] dp[i];
		delete[] route[i];
	}
	delete[] x;
	delete[] dp;
	delete[] route;

	return ret;
}

PYBIND11_MODULE(dp_tsp, m) {
	m.def("solve", &solve, py::arg("distance_matrix"));
}