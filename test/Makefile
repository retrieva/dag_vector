all: dag_vector_test comp_vector_test sparse_set_test sparse_map_test all_select_performance_test all_get_bit_performance_test bit_vector_test
dag_vector_test: dag_vector_test.cpp ../lib/dag_vector.hpp
	g++ -O2 -W -Wall -g -o dag_vector_test dag_vector_test.cpp
comp_vector_test: comp_vector_test.cpp ../lib/comp_vector.hpp
	g++ -O2 -W -Wall -g -o comp_vector_test comp_vector_test.cpp
sparse_set_test: sparse_set_test.cpp ../lib/sparse_set.hpp
	g++ -O2 -W -Wall -g -o sparse_set_test sparse_set_test.cpp
sparse_map_test: sparse_map_test.cpp ../lib/sparse_map.hpp
	g++ -O2 -W -Wall -g -o sparse_map_test sparse_map_test.cpp
all_select_performance_test: all_select_performance_test.cpp ../lib/dag_vector.hpp ../lib/sparse_set.hpp
	g++ -O2 -W -Wall -g -o all_select_performance_test all_select_performance_test.cpp
all_get_bit_performance_test: all_get_bit_performance_test.cpp ../lib/bit_vector.hpp ../lib/sparse_set.hpp
	g++ -O2 -W -Wall -g -o all_get_bit_performance_test all_get_bit_performance_test.cpp
bit_vector_test: bit_vector_test.cpp ../lib/bit_vector.hpp
	g++ -O2 -W -Wall -g -o bit_vector_test bit_vector_test.cpp