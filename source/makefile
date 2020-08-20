CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall -Wshadow -Wextra

run: naive.o balanced_trees.o parsings.o correctness_tester.o
	$(CXX) $(CXXFLAGS) run.cpp naive.o balanced_trees.o correctness_tester.o parsings.o -o run

naive.o:
	$(CXX) $(CXXFLAGS) -c solutions/naive.cpp

balanced_trees.o:
	$(CXX) $(CXXFLAGS) -c solutions/balanced_trees.cpp

parsings.o:
	$(CXX) $(CXXFLAGS) -c solutions/parsings.cpp

correctness_tester.o:
	$(CXX) $(CXXFLAGS) -c testers/correctness_tester.cpp

clean:
	rm -f *.o run