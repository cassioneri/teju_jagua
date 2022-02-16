CPPFLAGS         += -DNDEBUG -I ../include
CFLAGS           += -O3 -Wall -Wextra -std=c11
CXXFLAGS         += -O3 -Wall -Wextra -std=c++17

GENERATED        := ieee32 ieee64

GENERATED_H      := $(patsubst %,../generated/%.h,$(GENERATED))
GENERATED_C      := $(patsubst %,../generated/%.c,$(GENERATED))
GENERATED_O      := $(patsubst %,%.c.o,$(GENERATED))

DERIVED_H        := $(patsubst %,../inlude/%_conv.h,$(GENERATED))
DERIVED_C        := $(patsubst %,%_conv.c,$(GENERATED))
DERIVED_O        := $(patsubst %,%_conv.c.o,$(GENERATED))

all              : amaru.a benchmark test

generator.cpp.o  : generator.cpp ../include/common.h

generator        : generator.cpp.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

../generated/%.c : generator
	./generator

$(GENERATED_O)   : %.c.o : ../generated/%.c ../include/amaru.h ../include/common.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

$(DERIVED_O)     : %_conv.c.o : %_conv.c ../include/%_conv.h ../generated/%.h ../include/common.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

%.cpp.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

other.cpp.o      : other.cpp other.hpp
benchmark.o      : benchmark.cpp ../include/common.h $(DERIVED_H)
test.cpp.o       : test.cpp ../include/amaru.h ../include/common.h $(GENERATED_H)

amaru.a          : $(GENERATED_O) $(DERIVED_O)
	$(AR) r $@ $^

benchmark        : benchmark.cpp.o other.cpp.o amaru.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

test             : test.cpp.o other.cpp.o amaru.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ -lgtest -lgtest_main

.PHONY: clean
clean:
	rm -f $(GENERATED_O) $(DERIVED_O) amaru.a \
		generator generator.cpp.o \
		other.cpp.o \
		benchmark benchmark.cpp.o \
		test test.cpp.o
