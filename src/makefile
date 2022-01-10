CPPFLAGS += -DNDEBUG -I ../../ryu/ryu -I ../../dragonbox/include/dragonbox
CFLAGS   += -O3 -Wall -Wextra -std=c11
CXXFLAGS += -O3 -Wall -Wextra -std=c++17

all         : test benchmark

generator.o : generator.cpp ../include/common.h

generator   : generator.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

../generated/%.c : generator ../include/amaru_impl.h
	./generator

generated_%.o : ../generated/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

dragonbox.o : dragonbox.cpp dragonbox.hpp

test.o      : test.cpp ../include/common.h ../include/ieee.h \
	../generated/ieee32.c ../generated/ieee64.c

test        : test.o generated_ieee32.o generated_ieee64.o dragonbox.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^  ../../ryu/ryu/libryu.a -lgtest -lgtest_main

benchmark.o : benchmark.cpp ../include/common.h ../include/ieee.h

benchmark   : benchmark.o generated_ieee32.o generated_ieee64.o dragonbox.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ ../../ryu/ryu/libryu.a

.PHONY: clean
clean:
	rm -f *.o generator test benchmark
