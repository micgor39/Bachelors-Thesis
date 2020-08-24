CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall -Wshadow -Wextra

run: solution.o naive.o balanced_trees.o parsings.o parsings_slow_lcp.o correctness_tester.o
	$(CXX) $(CXXFLAGS) run.cpp solution.o naive.o balanced_trees.o correctness_tester.o parsings.o parsings_slow_lcp.o -o run

solution.o:
	$(CXX) $(CXXFLAGS) -c solutions/solution.cpp

naive.o:
	$(CXX) $(CXXFLAGS) -c solutions/naive.cpp

balanced_trees.o:
	$(CXX) $(CXXFLAGS) -c solutions/balanced_trees.cpp

parsings.o:
	$(CXX) $(CXXFLAGS) -c solutions/parsings.cpp

parsings_slow_lcp.o:
	$(CXX) $(CXXFLAGS) -c solutions/parsings_slow_lcp.cpp

correctness_tester.o:
	$(CXX) $(CXXFLAGS) -c testers/correctness_tester.cpp

clean:
	rm -f *.o run