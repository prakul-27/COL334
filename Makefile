run:
	g++ traceroute.cpp `pkg-config python3-embed --libs --cflags` -o program