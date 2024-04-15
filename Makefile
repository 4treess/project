G = g++-4.9
CFLAGS = -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei

#csci370A7Prog: csci370A7.o
#	$(G) $(LFLAGS) -o csci370A7Prog csci370A7.o

#csci370A7.o: csci370A7.cpp
#	$(G) -c -g $(CFLAGS) csci370A7.cpp

csci370Proj: CSCI370Project.o functions.o
	$(G) $(LFLAGS) -o csci370Proj $^

CSCI370Project.o: CSCI370Project.cpp Main.h
	$(G) -c -g -Wall -Wextra $(CFLAGS) CSCI370Project.cpp

functions.o: functions.cpp functions.h
	$(G) -c -g -Wall -Wextra $(CFLAGS) functions.cpp

clean:
	rm -f *.o CSCI370Project.o functions.o