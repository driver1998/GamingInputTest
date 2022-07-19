CFLAGS=-municode
LDFLAGS=-lwindowsapp -luser32
all: GamingInputTest.exe

clean:
	rm *.exe *.o

main.o: main.c arraylist.h listview.h winrt_gamepad.h
	cc -c $(CFLAGS) -o main.o main.c

arraylist.o: arraylist.c arraylist.h
	cc -c $(CFLAGS) -o arraylist.o arraylist.c

listview.o: listview.c listview.h
	cc -c $(CFLAGS) -o listview.o listview.c

winrt_gamepad.o: winrt_gamepad.c winrt_gamepad.h
	cc -c $(CFLAGS) -o winrt_gamepad.o winrt_gamepad.c

GamingInputTest.exe: main.o listview.o arraylist.o winrt_gamepad.o
	cc $(CFLAGS) $(LDFLAGS) $^ -o GamingInputTest.exe