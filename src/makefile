CPPFLAGS      += -DNDEBUG -I ../include
CFLAGS        += -O3 -Wall -Wextra -std=c11
CXXFLAGS      += -O3 -Wall -Wextra -std=c++17

AMARU_DOUBLE_H := ../include/amaru_double.h ../generated/ieee64.h ../generated/ieee64_impl.h
AMARU_FLOAT_H  := ../include/amaru_float.h  ../generated/ieee32.h ../generated/ieee32_impl.h

all            : test benchmark

generator.o    : generator.cpp ../include/common.h

generator      : generator.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

../generated/% : generator
	./generator

amaru_double.o : amaru_double.c ../include/amaru_impl.h ../include/common.h $(AMARU_DOUBLE_H)
amaru_float.o  : amaru_float.c  ../include/amaru_impl.h ../include/common.h $(AMARU_FLOAT_H)
dragonbox.o    : dragonbox.cpp dragonbox.hpp

test.o         : test.cpp ../include/amaru_impl.h ../include/common.h $(AMARU_DOUBLE_H) $(AMARU_FLOAT_H)

test           : test.o amaru_double.o amaru_float.o dragonbox.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ -lgtest -lgtest_main

benchmark.o    : benchmark.cpp ../include/amaru_impl.h ../include/common.h $(AMARU_DOUBLE_H) $(AMARU_FLOAT_H)

benchmark      : benchmark.o amaru_double.o amaru_float.o dragonbox.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o generator test benchmark
