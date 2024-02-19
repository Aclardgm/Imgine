
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "imgine_jobsystem.h"



//
//TEST_CASE("Construction and deconstruction", "[thread_pool]")
//{
//    BENCHMARK("Construction and destruction")
//    {
//        ThreadPool thread_pool{ 4 };
//    };
//}
//

const int num_blocks = 50000;
const int block_size = 1 << 10;

auto				g_global_mem = n_pmr::synchronized_pool_resource({ .max_blocks_per_chunk = num_blocks, .largest_required_pool_block = block_size }, n_pmr::new_delete_resource());
auto				g_global_mem_f = n_pmr::synchronized_pool_resource({ .max_blocks_per_chunk = num_blocks, .largest_required_pool_block = block_size }, n_pmr::new_delete_resource());
thread_local auto	g_local_mem_f = n_pmr::unsynchronized_pool_resource({ .max_blocks_per_chunk = num_blocks, .largest_required_pool_block = block_size }, n_pmr::new_delete_resource());
auto				g_global_mem_c = n_pmr::synchronized_pool_resource({ .max_blocks_per_chunk = num_blocks, .largest_required_pool_block = block_size }, n_pmr::new_delete_resource());
thread_local auto	g_local_mem_c = n_pmr::unsynchronized_pool_resource({ .max_blocks_per_chunk = num_blocks, .largest_required_pool_block = block_size }, n_pmr::new_delete_resource());
thread_local auto	g_local_mem_m = n_pmr::monotonic_buffer_resource(1 << 20, n_pmr::new_delete_resource());

void func(std::atomic<int>* atomic_int, int i = 1) {
	if (i > 1) schedule([=]() { func(atomic_int, i - 1); });
	if (i > 0) (*atomic_int)++;
}

void func2(std::atomic<int>* atomic_int, int i = 1) {
	if (i > 1) continuation([=]() { func(atomic_int, i - 1); });
	if (i > 0) (*atomic_int)++;
}

#define TESTRESULT(N, S, EXPR, B, C) \
		EXPR; \
		std::cout << "Test " << std::right << std::setw(3) << N << "  " << std::left << std::setw(30) << S << " " << ( B ? "PASSED":"FAILED" ) << std::endl;\
		C;

/**
* \brief This can be called as co_await parameter. It constructs a tuple
* holding only references to the arguments. The arguments are passed into a
* function get_ref, which SFINAEs to either a lambda version or for any other parameter.
*
* \param[in] args Arguments to be put into tuple
* \returns a tuple holding references to the arguments.
*
*/
template<typename... Ts>
inline decltype(auto) parallel(Ts&&... args) {
	return std::tuple<Ts&&...>(std::forward<Ts>(args)...);
}

void start_test() {
	int number = 0;
	std::atomic<int> counter = 0;
	JobSystem js;

	auto f1 = [&]() { func(&counter); };
	auto f2 = std::function<void(void)>{ [&]() { func(&counter); } };
	std::pmr::vector<std::function<void(void)>> vf1{ [&]() { func(&counter); }, [&]() { func(&counter); } };
	std::pmr::vector<std::function<void(void)>> vf2{ [&]() { func(&counter, 10); }, [&]() { func(&counter, 10); } };

	TESTRESULT(++number, "Single function", schedule([&]() { func(&counter); }), counter.load() == 1, counter = 0);
	TESTRESULT(++number, "10 functions", schedule([&]() { func(&counter, 10); }), counter.load() == 10, counter = 0);
	TESTRESULT(++number, "Single function ref", schedule(f1), counter.load() == 1, counter = 0);
	TESTRESULT(++number, "Single function ref", schedule(f2), counter.load() == 1, counter = 0);
	
	

	TESTRESULT(++number, "Vector function", schedule(std::pmr::vector<std::function<void(void)>>{ [&]() { func(&counter); } }), counter.load() == 1, counter = 0);
	TESTRESULT(++number, "Vector Function", schedule(std::pmr::vector<FunctionWrapper>{ FunctionWrapper{ [&]() { func(&counter); } } }), counter.load() == 1, counter = 0);


	//TESTRESULT(++number, "Parallel functions", schedule( parallel([&]() { func(&counter); }, [&]() { func(&counter); })), counter.load() == 2, counter = 0);
	//TESTRESULT(++number, "Parallel functions", schedule( parallel([&]() { func(&counter, 10); }, [&]() { func(&counter, 10); })), counter.load() == 20, counter = 0);

	counter = 0;
	std::pmr::vector<std::function<void(void)>> tagvf{ [&]() { func(&counter); }, [&]() { func(&counter); } };
	std::pmr::vector<FunctionWrapper> tagvF{ FunctionWrapper{[&]() { func(&counter); }}, FunctionWrapper{[&]() { func(&counter); }} };


	schedule([&]() { func(&counter, 10); }, tag_t{ 1 });
	schedule([&]() { func(&counter, 10); }, tag_t{ 2 });
	tag_t t1{ 1 };
	tag_t t2{ 2 };
	TESTRESULT(++number, "Tagged jobs 1", js.schedule_tag(t1), counter.load() == 10, );
	TESTRESULT(++number, "Tagged jobs 2", js.schedule_tag(t2), counter.load() == 20, );




	js.terminate();
}



int main(int argc, char* argv[]) {
	int num = argc > 1 ? std::stoi(argv[1]) : 0;
	JobSystem js(thread_count_t{ num });
	std::atomic<uint32_t> i { 0 };
	schedule(start_test);
	wait_for_termination();
	return 0;
}