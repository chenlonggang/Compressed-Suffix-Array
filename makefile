CC=g++
CFLAGS=  -O3 -Wall
my_csa:divcmake divmake div  main.o csa.a 
	g++ main.o csa.a -o my_csa 

csa.a:savekit.o loadkit.o InArray.o Phi.o ABS_Phi.o GAM_Phi.o CSA.o divsufsort.o sssort.o trsort.o utils.o
	ar rc csa.a CSA.o savekit.o loadkit.o InArray.o Phi.o ABS_Phi.o GAM_Phi.o divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
main.o:main.cpp  CSA.h
	g++ -c main.cpp 
clean:
	rm *.a  *.o  my_csa
divcmake:
	cd ./divsufsort; rm Makefile CMakeCache.txt; cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_DIVSUFSORT64:BOOL=ON
divmake:
	make -C ./divsufsort/;
div:
	cp divsufsort/lib/libdivsufsort64.a .;ar x libdivsufsort64.a

