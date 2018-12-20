.PHONY: all
all: tcfield.o tccpubase.o tccpu_human.o tccpu_random.o tccpu_minmax.o tccpu.exe



tcfield.o: tcfield.h tcfield.cpp
	g++ -c -o tcfield.o tcfield.cpp

tccpubase.o: tccpubase.h tccpubase.cpp
	g++ -c -o tccpubase.o tccpubase.cpp

tccpu_human.o: tccpu_human.h tccpu_human.cpp tcfield.o tccpubase.o
	g++ -c -o tccpu_human.o tccpu_human.cpp

tccpu_random.o: tccpu_random.h tccpu_random.cpp tcfield.o tccpubase.o
	g++ -c -o tccpu_random.o tccpu_random.cpp

tccpu_minmax.o: tccpu_minmax.h tccpu_minmax.cpp tcfield.o tccpubase.o
	g++ -c -o tccpu_minmax.o tccpu_minmax.cpp

tccpu.exe: tccpu.cpp tcfield.o tccpubase.o tccpu_human.o tccpu_random.o tccpu_minmax.o
	g++ -o tccpu.exe tccpu.cpp tcfield.o tccpubase.o tccpu_human.o tccpu_random.o tccpu_minmax.o



.PHONY: clean
clean:
	rm -f tcfield.o tccpubase.o tccpu_human.o tccpu_random.o tccpu_minmax.o
