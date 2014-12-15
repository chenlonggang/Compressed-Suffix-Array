CC=gcc 
CFLAGS=  -O3 -Wall  -DHAVE_CONFIG_H -DBUILD_DIVSUFSORT64
libdivsufsort.a:divsufsort.o sssort.o trsort.o utils.o
	ar rc libdivsufsort64.a divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm *.a *.o
