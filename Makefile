GCC = gcc

C_CLIENT  = client.c main.c
C_SERVER = server.c
O_CLIENT  = client.o main.o
O_SERVER = server.o

BUILD = build
CLIENT = client
SERVER = server

FLAGS = 

.PHONY: build obj clean

build: obj clean
	@ cd $(BUILD) && $(GCC) $(FLAGS) $(O_CLIENT) -o $(CLIENT)
	@ cd $(BUILD) && $(GCC) $(FLAGS) $(O_SERVER) -o $(SERVER)
	@ cd $(BUILD) && mv $(CLIENT) ../ && mv $(SERVER) ../

obj: clean
	@ mkdir $(BUILD)
	@ $(GCC) $(C_CLIENT) -c
	@ $(GCC) $(C_SERVER) -c
	@ mv $(O_CLIENT) $(BUILD)
	@ mv $(O_SERVER) $(BUILD)

clean:
	@ rm -rf $(CLIENT) $(SERVER) $(BUILD)
