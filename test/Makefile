all: dag_vector_test comp_vector_test dag_select_test dag_vector_performance_test
dag_vector_test: dag_vector_test.cpp ../lib/dag_vector.hpp
	g++ -O2 -W -Wall -g -o dag_vector_test dag_vector_test.cpp
comp_vector_test: comp_vector_test.cpp ../lib/comp_vector.hpp
	g++ -O2 -W -Wall -g -o comp_vector_test comp_vector_test.cpp
dag_select_test: dag_select_test.cpp ../lib/dag_select.hpp
	g++ -O2 -W -Wall -g -o dag_select_test dag_select_test.cpp
dag_vector_performance_test: dag_vector_performance_test.cpp ../lib/dag_vector.hpp
	g++ -O2 -W -Wall -g -o dag_vector_performance_test dag_vector_performance_test.cpp
