MAKE = make 
DEMOSPATH = $(realpath ./) 

all:
	@echo $(DEMOSPATH)
	$(MAKE) -C Inventory all
	$(MAKE) -C FMS all
	$(MAKE) -C Philosopher all
	$(MAKE) -C Assembly all
	$(MAKE) -C QueueReplication all	
	@echo All done!
	
clean:
	$(MAKE) -C Inventory clean
	$(MAKE) -C FMS clean
	$(MAKE) -C Philosopher clean
	$(MAKE) -C Assembly clean
	$(MAKE) -C QueueReplication clean	
