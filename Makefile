GCC = gcc
C_ARP  = main.c arp.c misc.c
O_ARP  = main.o arp.o misc.o
BUILD = build
ARP = spoof
FLAGS = 

.PHONY: build obj clean

build: obj clean
	@ cd $(BUILD) && $(GCC) $(FLAGS) $(O_ARP) -o $(ARP)
	@ cd $(BUILD) && mv $(ARP) ../

obj: clean
	@ mkdir $(BUILD)
	@ $(GCC) $(C_ARP) -c
	@ mv $(O_ARP) $(BUILD)

clean:
	@ rm -rf $(ARP) $(BUILD)
