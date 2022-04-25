include ../Makefile.defs

demo/pipes: setup demos/pipes.c
	$(CC) $(CCFLAGS) demos/pipes.c -o $(BIN)/pipes

# Targets for the examples provided in exercises PDF
q1/original: setup q1/original.c
	$(CC) $(CCFLAGS) q1/original.c -o $(BIN)/q1-original

q2/original: setup q2/original.c
	$(CC) $(CCFLAGS) q2/original.c -o $(BIN)/q2-original

q4/original: setup q4/original.c
	$(CC) $(CCFLAGS) q4/original.c -o $(BIN)/q4-original

q5/original: setup q5/original.c
	$(CC) $(CCFLAGS) q5/original.c -o $(BIN)/q5-original

q6/original: setup q6/original.c
	$(CC) $(CCFLAGS) q6/original.c -o $(BIN)/q6-original

# Targets for exercise solutions
q1: setup q1/sol.c
	$(CC) $(CCFLAGS) q1/sol.c -o $(BIN)/q1

q2: setup q2/sol.c
	$(CC) $(CCFLAGS) q2/sol.c -o $(BIN)/q2

q3: setup q3/sol.c
	$(CC) $(CCFLAGS) q3/sol.c -o $(BIN)/q3

q4: setup q4/sol.c
	$(CC) $(CCFLAGS) q4/sol.c -o $(BIN)/q4

q7: setup q7/sol.c
	$(CC) $(CCFLAGS) q7/sol.c -o $(BIN)/q7

# No default target for this makefile
.DEFAULT_GOAL:=
