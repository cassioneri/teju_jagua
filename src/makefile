CPPFLAGS += -DNDEBUG -I../include -I. -I.. -I ~/ryu/cassio/ryu
CFLAGS   += -O3 -Wall -Wextra -std=c11
CXXFLAGS += -O3 -Wall -Wextra -std=c++11

all         : test benchmark

generator.o : generator.cpp ../include/common.h math.hpp
generator   : generator.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

../generated/%.c : generator amaru.h
	./generator

generated_%.o : ../generated/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

test.o      : test.cpp math.hpp ../include/common.h ../include/ieee.h \
	../generated/ieee32.c ../generated/ieee64.c
test        : test.o generated_ieee32.o generated_ieee64.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^  ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main

benchmark.o : benchmark.cpp ../include/common.h ../include/ieee.h
benchmark   : benchmark.o generated_ieee32.o generated_ieee64.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ ~/ryu/cassio/ryu/libryu.a

.PHONY: clean
clean:
	rm -f *.o generator test benchmark
