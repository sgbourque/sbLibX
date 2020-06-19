//
// Note : Les sommes apparaissent ici "inversées" en commençant par le n le plus grand vers 0.
// Ceci est pour aller chercher le maximum de précision (les termes avec n plus élevés sont les termes les plus petits)
// dès le départ (voir exemple du low_precision).
//
#include <tuple>
#include <algorithm>
#include <chrono>

	template< typename _TYPE_>
struct real_topology_point
{
	using type_t = _TYPE_;
	type_t value;
	type_t precision;
};

////
	template< typename _TYPE_>
constexpr real_topology_point<_TYPE_> ln2_slow_series_nth_term(const int64_t n)
{
	using type_t = _TYPE_;
	const auto sign = type_t((n & 1) == 0 ? -1 : 1);
	const auto term_value = sign / type_t(n);
	return real_topology_point<type_t>{
		.value = term_value,
		.precision = type_t(1) / type_t(n + 1) + type_t(1) / type_t(n + 2),
	};
}

// These are the straightforward ln(2) series expension of ln(1+x) at x -> 1.
// high_precision calculates smallest terms first while low_precision starts with the largest one.
	template< typename _TYPE_>
constexpr real_topology_point<_TYPE_> ln2_slow_series_high_precision( const int64_t termsCount = 0 )
{
	// starting from smallest numbers to largest will allow to achieve best floating point precision
	using type_t = _TYPE_;
	auto result = real_topology_point<type_t>{
		.value     = type_t(0),
		.precision = std::numeric_limits<type_t>::infinity(),
	};
	for (int64_t n = termsCount; n > 0; --n)
	{
		const auto next_smallest_term = ln2_slow_series_nth_term<type_t>(n);
		result.precision = std::max((result.value + std::min(next_smallest_term.precision, result.precision)) - result.value, std::numeric_limits<type_t>::denorm_min());
		result.value += next_smallest_term.value;
	}
	return result;
}
	template< typename _TYPE_>
constexpr real_topology_point<_TYPE_> ln2_slow_series_low_precision( const int64_t termsCount = 0 )
{
	// starting from largest numbers to smallest will loose some precision : the converging value is off because of the floating point nature
	// (and because this series converges too slowly...)
	using type_t = _TYPE_;
	auto result = real_topology_point<type_t>{
		.value     = type_t(0),
		.precision = std::numeric_limits<type_t>::infinity(),
	};
	for (int64_t n = 1; n < termsCount + 1; ++n)
	{
		const auto next_biggest_term = ln2_slow_series_nth_term<type_t>(n);
		result.precision = std::max((result.value + std::min(next_biggest_term.precision, result.precision)) - result.value, std::numeric_limits<type_t>::denorm_min());
		result.value += next_biggest_term.value;
	}
	return result;
}

////
// This simulates as if the ln2_slow_series_low_precision was rearranged such that two threads,
// running at different frequency, are accumulating partial sums for positive and negative terms.
// At every loop, the value from each thread are read and the partial sum is returned once
// we reach at least termsCount.
	template< typename _TYPE_>
constexpr real_topology_point<_TYPE_> rearranged_slow_series(int64_t termsCount = 0, int64_t positives = 1, int64_t negatives = 1)
{
	// align up termsCount for positive & negatives
	const int64_t termsPerIteration = (positives + negatives);
	const int64_t loopCount = termsPerIteration > 0 ? ((termsCount + (termsPerIteration - 1)) / termsPerIteration) : 0;
	int64_t remainingCount = termsPerIteration * loopCount;

	using type_t = _TYPE_;
	auto result = real_topology_point<type_t>{
		.value     = type_t(0),
		.precision = std::numeric_limits<type_t>::quiet_NaN(),
	};
	int64_t positiveCount = positives * loopCount;
	int64_t negativeCount = negatives * loopCount;
	while (remainingCount != 0)
	{
		const auto current_precision = (positives / type_t(2 * positiveCount + 1) - negatives / type_t(2 * (negativeCount + negatives + 1)));
		auto positiveValue = type_t(0);
		//auto [&positiveValue, &positiveCount]()->type_t
		//{
		//}
		for (int64_t p = 0; p < positives; ++p)
		{
			auto next_term = type_t(2 * positiveCount - 1); // next largest odd value
			positiveValue += type_t( 1) / next_term; // next smallest positive term
			--positiveCount;
		}
		auto negativeValue = _TYPE_(0);
		for (int64_t n = 0; n < negatives; ++n)
		{
			auto next_term = type_t(2 * negativeCount); // next largest even value
			negativeValue += _TYPE_(-1) / next_term; // next smallest negative term
			--negativeCount;
		}

		auto next_smallest_term = (positiveValue + negativeValue);
		result.precision = std::max((result.value + std::min(current_precision, result.precision)) - result.value, std::numeric_limits<type_t>::denorm_min());
		result.value += next_smallest_term;
		remainingCount -= (positives + negatives);
	}
	return result;
}

////
// be aware that code below is not particularly
#include <iostream>
bool reset_bad_stream( std::istream& is )
{
	if (is.fail())
	{
		is.clear();
		is.ignore();
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return true;
	}
	return false;
}
void clear_screen()
{
	// no standard way of clearing the console :(
	system("cls");
}

	template<typename _TYPE_>
void testing_series_part1()
{
	using type_t = _TYPE_;
	clear_screen();
	std::cout << "-- Testing Riemann's series theorem (part 1) --\n";
	do
	{
		std::cout << "\n- Floating point precision vs convergence (" << typeid(type_t).name() << ") -" << std::endl;

		int64_t loops;
	retry_count:
		std::cout << "enter total terms count: ";
		std::cin >> loops;
		if (reset_bad_stream(std::cin))
		{
			std::cerr << "invalid input." << std::endl;
			goto retry_count;
		}
		if (loops < 0)
			break;


		auto start = std::chrono::high_resolution_clock::now();
		auto result_high = ln2_slow_series_high_precision<type_t>(loops);
		auto end = std::chrono::high_resolution_clock::now();

		auto result_low = ln2_slow_series_low_precision<type_t>(loops);
		type_t expected = log(type_t(2));

		std::cout.precision(std::numeric_limits<type_t>::max_digits10);
		std::cout << "\tseries gives ln(2) ~ " << result_high.value << " +/- " << std::fixed << result_high.precision << "\n";
		std::cout << "\tcompare to   ln(2) = " << expected          << " +/- " << std::fixed << std::numeric_limits<type_t>::denorm_min() << "\n";
		std::cout << "\tlow precision gives  " << result_low.value << " +/- " << std::fixed << result_low.precision << "\n";
		std::cout << "calculation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
	}
	while (true);
}

	template<typename _TYPE_>
void testing_series_part2()
{
	using type_t = _TYPE_;
	clear_screen();
	std::cout << "\n-- Testing Riemann's series theorem (part 2) --\n";
	do
	{
		std::cout << "\n- Rearranging terms vs convergence (" << typeid(type_t).name() << ") -" << std::endl;

		int64_t loops;
	retry_count2:
		std::cout << "enter minimum terms count: ";
		std::cin >> loops;
		if (reset_bad_stream(std::cin))
		{
			std::cerr << "invalid input." << std::endl;
			goto retry_count2;
		}
		if (loops < 0)
			break;

		int64_t p;
	retry_odd2:
		std::cout << "enter odd count: ";
		std::cin >> p;
		if (reset_bad_stream(std::cin))
		{
			std::cerr << "invalid input." << std::endl;
			goto retry_odd2;
		}
		if (p < 0)
			break;

		int64_t n;
	retry_even2:
		std::cout << "enter even count: ";
		std::cin >> n;
		if (reset_bad_stream(std::cin))
		{
			std::cerr << "invalid input." << std::endl;
			goto retry_even2;
		}
		if (n < 0)
			break;

		std::cout << "\n";

		auto result = rearranged_slow_series<type_t>(loops, p, n);
		type_t expected = log(type_t(2) * sqrt(type_t(p) / type_t(n)));
		std::cout.precision(std::numeric_limits<type_t>::max_digits10);
		std::cout << "\trearranged series ...  " << result.value << std::endl;
		std::cout << "show expected result? ";

		char ready;
		std::cin.ignore(INT_MAX, '\n');
		std::cin.get(ready);
		std::cin.ignore(INT_MAX, '\n');

		bool showExpected = ((ready | ' ') == 'y');
		if (showExpected)
		{
			std::cout << "\tln(2 sqrt(" << p << "/" << n << ")) = " << "ln(" << 2 * sqrt(type_t(p) / type_t(n)) << ") = " << expected << std::endl;
		}
		else
		{
			ready = 0;
			std::cout << "show predicted result? ";
			std::cin.get(ready);
			std::cin.ignore(INT_MAX, '\n');

			showExpected = ((ready | ' ') == 'y');
			if (showExpected)
			{
				std::cout << "\t******************** = " << expected << std::endl;
			}
		}
	}
	while (true);
}

	template<typename _TYPE_>
void run_internal( size_t part )
{
	switch (part)
	{
	case 0:
	case 1:
		testing_series_part1<_TYPE_>();
		[[fallthrough]];
	case 2:
		testing_series_part2<_TYPE_>();
		[[fallthrough]];
	default:
		break;
	}
}

////
#include <string>

#if !defined(SB_EXPORT_TYPE)
	#define SB_EXPORT_TYPE
	#define test_riemann		main
#endif
SB_EXPORT_TYPE int __stdcall test_riemann([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	std::cout << "-- Not-a-Number & The Riemann Series Theorem --\n";
	do
	{
		std::string typeName;
		std::cout << "use float or double? ('q' or '-1' to quit) ";
		std::cin >> typeName;
		if (!typeName.empty())
		{
			size_t part = static_cast<size_t>( abs(typeName[1] - '0') );
			if (part > 9 )
				part = 0;

			switch (typeName[0] | ' ')
			{
			case 'f': run_internal<float>(part); break;
			case 'd': run_internal<double>(part); break;
			case '-': [[fallthrough]];
			case 'q': return 0;
			default: break;
			}
		}
	}
	while(true);
}
