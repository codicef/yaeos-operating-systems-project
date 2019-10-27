# yaeos-operating-systems-project
Yet another educational operating system



# Compile and test
- execure "make clean"
- execute "make"
- test the file bin/init.e in UARM


# Structure
/yaeos
|
|____/headers   <- headers fils folders
|    |
|    |___ pcb.h
|    |___ alberi.h 
|    |___ coda.h 
|    |___ hash.h
|    |___ init.h 
|    |___ scheduler.h 
|    |___ interrupt.h
|    |___ systemcalls.h 
|    |___ traphandlers.h 
|   
|____/modules   
|    |			
|    |____/pcb <- phase1
|    |	  |
|    |	  |____/src    	
|    |	  |	|___ alberi.c 
|    |	  |	|___ coda.c 
|    |	  |	|___ hash.c
|    |	  |
|    |	  |___/build
|    |	  |
|    |	  |___makefile
|    |
|    |	
|    |____/kernel <- phase2
|    |	  |
|    |	  |____/src    	
|    |	  |	|___ scheduler.c 
|    |	  |	|___ traphandlers.c 
|    |	  |	|___ systemcalls.c
|    |	  |     |___ interrupt.c
|    |    |     |___ p2test.c
|    |	  |     |___ init.c
|    |	  |
|    |	  |___/build
|    |	  |
|    |	  |___makefile
|    |
|    |
|    |____/bin <- executables files
|
|	
|__ makefile   <- makefile
|
|__ AUTHORS    <- authors file
|
