CC=g++
CFLAGS=  -O3 -Wall
my_csa:div  main.o csa.a 
	g++ main.o csa.a -o my_csa 

csa.a:savekit.o loadkit.o InArray.o Phi.o ABS_Phi.o GAM_Phi.o CSA.o divsufsort.o sssort.o trsort.o utils.o
	ar rc csa.a CSA.o savekit.o loadkit.o InArray.o Phi.o ABS_Phi.o GAM_Phi.o divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
main.o:main.cpp  CSA.h
	g++ -c main.cpp 
clean:
	rm *.a  *.o  my_csa ./divsufsort/div32/*.o ./divsufsort/div32/*.a
div:
	make -C ./divsufsort/div32/;cp divsufsort/div32/libdivsufsort.a .;ar x libdivsufsort.a

