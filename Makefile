CC=gcc
WARNING_FLAGS=-Wall -Wextra
SCAN_BUILD_DIR = scan-build-out
LIBS=-lpthread

all: 537pfsim-lru 537pfsim-fifo 537pfsim-clock 537pfsim-opt

537pfsim-fifo: runner.o reader.o memoryTree.o queue.o stats.o fifo.o
	$(CC) -g -o 537pfsim-fifo runner.o reader.o memoryTree.o queue.o stats.o fifo.o $(LIBS)

537pfsim-lru: runner.o reader.o memoryTree.o queue.o stats.o lru.o
	$(CC) -g -o 537pfsim-lru runner.o reader.o memoryTree.o queue.o stats.o lru.o $(LIBS)

537pfsim-clock: runner.o reader.o memoryTree.o queue.o stats.o clock.o
	$(CC) -g -o 537pfsim-clock runner.o reader.o memoryTree.o queue.o stats.o clock.o $(LIBS)

537pfsim-opt: runner.o reader.o memoryTree.o queue.o stats.o minopt.o
	$(CC) -g -o 537pfsim-opt runner.o reader.o memoryTree.o queue.o stats.o minopt.o $(LIBS)

runner.o: runner.c reader.h queue.h stats.h memoryTree.h pageAlgo.h datatypes.h
	$(CC) -g $(WARNING_FLAGS) -c runner.c

fifo.o: fifo.c pageAlgo.h queue.h memoryTree.h
	$(CC) -g $(WARNING_FLAGS) -c fifo.c

lru.o: lru.c pageAlgo.h queue.h memoryTree.h
	$(CC) -g $(WARNING_FLAGS) -c lru.c

clock.o: clock.c pageAlgo.h queue.h memoryTree.h
	$(CC) -g $(WARNING_FLAGS) -c clock.c

minopt.o: minopt.c pageAlgo.h queue.h memoryTree.h reader.h
	$(CC) -g $(WARNING_FLAGS) -c minopt.c

reader.o: reader.c reader.h queue.h stats.h datatypes.h
	$(CC) -g $(WARNING_FLAGS) -c reader.c

queue.o: queue.c queue.h stats.h
	$(CC) -g $(WARNING_FLAGS) -c queue.c

memoryTree.o: memoryTree.c memoryTree.h
	$(CC) -g $(WARNING_FLAGS) -c memoryTree.c

stats.o: stats.c stats.h
	$(CC) -g $(WARNING_FLAGS) -c stats.c

clean:
	rm -f $(EXE) *.o
	rm -rf $(SCAN_BUILD_DIR)
	rm 537pfsim-lru 537pfsim-fifo 537pfsim-clock 537pfsim-opt

recompile: clean $(EXE)
