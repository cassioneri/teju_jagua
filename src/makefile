CPPFLAGS  = -DNDEBUG -I../include -I. -I.. -I ~/ryu/cassio/ryu
CFLAGS    = -g -O3 -Wall -Wextra -std=c11
CXXFLAGS  = -g -O3 -Wall -Wextra -std=c++11

all         : test benchmark

generator.o : generator.cpp ../include/common.h
generator   : generator.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

../generated/%.c : generator amaru.h
	./generator

generated_%.o : ../generated/%.c generator
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

test.o      : test.cpp ../include/common.h ../include/ieee.h \
	../generated/ieee32.c ../generated/ieee64.c
test        : test.o generated_ieee32.o generated_ieee64.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^  ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main

benchmark.o : benchmark.cpp ../include/common.h ../include/ieee.h
benchmark   : benchmark.o generated_ieee32.o generated_ieee64.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ ~/ryu/cassio/ryu/libryu.a

.PHONY: clean
clean:
	rm -f *.o generator test
