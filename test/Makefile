all: dag_vector_test comp_vector_test dag_select_test all_select_performance_test all_get_bit_performance_test bit_vector_test
dag_vector_test: dag_vector_test.cpp ../lib/dag_vector.hpp
	g++ -O2 -W -Wall -g -o dag_vector_test dag_vector_test.cpp
comp_vector_test: comp_vector_test.cpp ../lib/comp_vector.hpp
	g++ -O2 -W -Wall -g -o comp_vector_test comp_vector_test.cpp
dag_select_test: dag_select_test.cpp ../lib/dag_select.hpp
	g++ -O2 -W -Wall -g -o dag_select_test dag_select_test.cpp
all_select_performance_test: all_select_performance_test.cpp ../lib/dag_vector.hpp ../lib/dag_select.hpp
	g++ -O2 -W -Wall -g -o all_select_performance_test all_select_performance_test.cpp
all_get_bit_performance_test: all_get_bit_performance_test.cpp ../lib/bit_vector.hpp ../lib/dag_select.hpp
	g++ -O2 -W -Wall -g -o all_get_bit_performance_test all_get_bit_performance_test.cpp
bit_vector_test: bit_vector_test.cpp ../lib/bit_vector.hpp
	g++ -O2 -W -Wall -g -o bit_vector_test bit_vector_test.cpp