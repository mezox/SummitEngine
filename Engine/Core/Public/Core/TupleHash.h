#pragma once

/**
* \file TupleHash.h
*
* std::hash specialization for std::tuple from stackoverflow.com and ideone.com
* How to: #include <PAL/System/TupleHashImpl.h>
*/

#include <tuple>
#include <numeric>
#include <vector>
#include <functional>
#include <array>

namespace
{
	size_t hash_combiner(size_t left, size_t right)
	{
		return right + 0x9e3779b9 + (left << 6) + (left >> 2);
	}

	template<int index, class...types>
	struct hash_impl {
		size_t operator()(size_t a, const std::tuple<types...>& t) const {
			typedef typename std::tuple_element<index, std::tuple<types...>>::type nexttype;
			hash_impl<index - 1, types...> next;
			size_t b = std::hash<typename std::decay<nexttype>::type>()(std::get<index>(t));
			return next(hash_combiner(a, b), t);
		}
	};
	template<class...types>
	struct hash_impl<0, types...> {
		size_t operator()(size_t a, const std::tuple<types...>& t) const {
			typedef typename std::tuple_element<0, std::tuple<types...>>::type nexttype;
			size_t b = std::hash<typename std::decay<nexttype>::type>()(std::get<0>(t));
			return hash_combiner(a, b);
		}
	};
}

// specialization for std::hash<std::tuple<...>>
namespace std {
	template<class...types>
	struct hash<std::tuple<types...>> {
		size_t operator()(const std::tuple<types...>& t) const {
			const size_t begin = std::tuple_size<std::tuple<types...>>::value - 1;
			return hash_impl<begin, types...>()(0, t);
		}
	};

	template<class First, class Second>
	struct hash<std::pair<First, Second>> {
		size_t operator()(const std::pair<First, Second>& t) const {
			return hash_impl<1, First, Second>()(0, t);
		}
	};

	template<class T>
	struct hash<std::vector<T>> {
		size_t operator()(const std::vector<T>& v) const {
			size_t input = 0x9e3779b9;
			return std::accumulate<>(v.begin(), v.end(), input, [](const size_t& a, const T& b) {
				return hash_combiner(a, std::hash<T>{}(b));
			});
		}
	};

	template<class T, std::size_t N>
	struct hash<std::array<T, N>> {
		size_t operator()(const std::array<T, N>& v) const {
			size_t input = 0x9e3779b9;
			return std::accumulate<>(v.begin(), v.end(), input, [](const size_t& a, const T& b) {
				return hash_combiner(a, std::hash<T>{}(b));
			});
		}
	};
}
