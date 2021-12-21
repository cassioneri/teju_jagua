CPPFLAGS  = -DNDEBUG -I../include -I. -I.. -I ~/ryu/cassio/ryu
CFLAGS    = -O3 -Wall -Wextra -std=c11
CXXFLAGS  = -O3 -Wall -Wextra -std=c++11

all         : test

generator.o : generator.cpp ../include/common.h
generator   : generator.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

../generated/ieee32.c : generator amaru.h
	./generator

ieee32.o : ../generated/ieee32.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

../generated/ieee64.c : generator amaru.h
	./generator

ieee64.o : ../generated/ieee64.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

test.o      : test.cpp ../include/common.h ../include/ieee.h

test        : test.o ieee32.o ieee64.o ~/ryu/cassio/ryu/libryu.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $? -lgtest -lgtest_main

.PHONY: clean
clean:
	rm -f *.o generator test
