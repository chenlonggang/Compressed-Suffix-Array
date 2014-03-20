CC=g++
CFLAGS= -g -O9 -Wall
my_csa:main.o csa.a 
	g++ main.o csa.a -o my_csa -lpthread

csa.a:savekit.o loadkit.o InArray.o Phi.o ABS_Phi.o GAM_Phi.o GRL_Phi.o CSA.o  ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/deep2.o ds_ssort/blind2.o
	ar rc csa.a CSA.o savekit.o loadkit.o InArray.o Phi.o   ABS_Phi.o GAM_Phi.o GRL_Phi.o  ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/deep2.o ds_ssort/blind2.o

#InArray.o:InArray.h InArray.cpp 
#	g++ -c -O3 InArray.cpp

#Dictionary.o:Dictionary.h Dictionary.cpp InArray.h
#	g++ -c -O3 Dictionary.cpp

#BitSequence.o:BitSequence.h BitSequence.cpp InArray.h
#	g++ -c -O3 BitSequence.cpp

#GapCoding.o:GapCoding.h GapCoding.cpp BitSequence.h
#	g++ -c -O3 GapCoding.cpp

#Phi.o:Phi.h Phi.cpp GapCoding.h 
#	g++ -c -O3 Phi.cpp 

#CSA.o:CSA.h CSA.cpp InArray.h Dictionary.h Phi.h
#	g++ -c -O3 CSA.cpp
%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
main.o:main.cpp  CSA.h
	g++ -c main.cpp 
clean:
	rm *.a  *.o ds_ssort/*.a ds_ssort/*.o ds_ssort/ds ds_ssort/bwt ds_ssort/unbwt ds_ssort/testlcp;
ds_ssortr:
	make -C ./ds_ssort/; cp ./ds_ssort/ds_ssort.a .

