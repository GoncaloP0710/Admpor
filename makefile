OBJ_DIR = obj
GCC = gcc

main.o = main.h memory.h process.h synchronization.h configuration.h aptime.h apsignal.h log.h stats.h
client.o = client.h memory.h main.h aptime.h synchronization.h
enterprise.o = enterprise.h memory.h main.h aptime.h synchronization.h
intermediary.o = intermediary.h memory.h main.h aptime.h synchronization.h
memory.o = memory.h main.h
process.o = process.h memory.h main.h client.h enterprise.h intermediary.h

aptime.o = aptime.h
synchronization.o = synchronization.h
log.o = log.h aptime.h
apsignal.o = apsignal.h main.h synchronization.h memory.h
configuration.o = configuration.h
stats.o = stats.h main.h memory.h synchronization.h


I = -I include
BIN = bin

OBJ = process.o main.o memory.o intermediary.o enterprise.o client.o aptime.o apsignal.o configuration.o log.o stats.o synchronization.o

vpath %.o $(OBJ_DIR)

out: $(OBJ)
	$(GCC) $(addprefix $(OBJ_DIR)/,$(OBJ)) -o $(BIN)/admpor

%.o: src/%.c $($@)
		$(GCC) $(I) -o $(OBJ_DIR)/$@ -c $<

clean: 
	rm -rf $(OBJ_DIR)/*.o $(BIN)/admpor