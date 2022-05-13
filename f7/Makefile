include ../Makefile.defs

# prepend the -pthread flag
CCFLAGS:=-pthread $(CCFLAGS)

# Targets for the examples provided in exercises PDF
q1/original: setup q1/original.c
	$(CC) $(CCFLAGS) q1/original.c -o $(BIN)/q1-original

q2/original: setup q2/original.c
	$(CC) $(CCFLAGS) q2/original.c -o $(BIN)/q2-original

q2/naive: setup q2/naive.c
	$(CC) $(CCFLAGS) q2/naive.c -o $(BIN)/q2-naive

q2/reduction: setup q2/reduction.c
	$(CC) $(CCFLAGS) q2/reduction.c -o $(BIN)/q2-reduction

q2/inc_dec: setup q2/inc_dec.c
	$(CC) $(CCFLAGS) q2/inc_dec.c -o $(BIN)/q2-inc_dec

q3/mutex: setup q3/mutex.c
	$(CC) $(CCFLAGS) q3/mutex.c -o $(BIN)/q3-mutex

q3/original: setup q3/original.c
	$(CC) $(CCFLAGS) q3/original.c -o $(BIN)/q3-original

# No default target for this makefile
.DEFAULT_GOAL:=
