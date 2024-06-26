MAKE = make 
EXAMPLESPATH = $(realpath ./) 

all:
	@echo $(EXAMPLESPATH)
	$(MAKE) -C Queue all
	$(MAKE) -C QueueRandom all
	$(MAKE) -C QueueBatch all
	$(MAKE) -C Queue3P all
	$(MAKE) -C QueueES all
	$(MAKE) -C QueuePI all
	$(MAKE) -C Queue_PI2 all	
	$(MAKE) -C Random all
	$(MAKE) -C AbleBaker_3P all
	$(MAKE) -C AbleBaker_ES all
	$(MAKE) -C AbleBaker_PI all
	$(MAKE) -C Philosopher_3P all
	$(MAKE) -C Philosopher_ES all
	$(MAKE) -C Philosopher_PI all
	$(MAKE) -C DumpTruck_ES all	
	$(MAKE) -C DumpTruck_3P all
	$(MAKE) -C DumpTruck_PI all
	@echo All done!
	
clean:
	$(MAKE) -C Queue clean
	$(MAKE) -C QueueRandom clean
	$(MAKE) -C QueueBatch clean
	$(MAKE) -C Queue3P clean
	$(MAKE) -C QueueES clean
	$(MAKE) -C QueuePI clean
	$(MAKE) -C Queue_PI2 clean
	$(MAKE) -C Random clean
	$(MAKE) -C AbleBaker_3P clean
	$(MAKE) -C AbleBaker_ES clean
	$(MAKE) -C AbleBaker_PI clean
	$(MAKE) -C Philosopher_3P clean
	$(MAKE) -C Philosopher_ES clean
	$(MAKE) -C Philosopher_PI clean
	$(MAKE) -C DumpTruck_ES clean	
	$(MAKE) -C DumpTruck_3P clean
	$(MAKE) -C DumpTruck_PI clean
