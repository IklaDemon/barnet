headers = ./include/
i = -I $(headers)
obj = ./bin/
src = ./source/
cc = clang

barnet.exe: main.c $(obj)lib.o $(obj)list.o $(obj)ssh.o $(obj)command-executor.o
	@echo Compiling main
	@$(cc) main.c -o barnet.exe $(i) $(obj)lib.o $(obj)list.o $(obj)ssh.o $(obj)devices-db.o $(obj)command-executor.o -lws2_32 -Wall
	@echo Done!

$(obj)lib.o: $(src)lib.c $(headers)lib.h
	@echo Compiling lib
	@$(cc) -c $(src)lib.c -o $(obj)lib.o $(i) -Wall

$(obj)list.o: $(src)list.c $(headers)list.h
	@echo Compiling list
	@$(cc) -c $(src)list.c -o $(obj)list.o $(i) -Wall

$(obj)ssh.o: $(src)ssh.c $(headers)ssh.h $(obj)devices-db.o
	@echo Compiling ssh
	@$(cc) -c $(src)ssh.c -o $(obj)ssh.o $(i) -Wall

$(obj)devices-db.o: $(src)devices-db.c $(headers)devices-db.h
	@echo Compiling devices-db
	@$(cc) -c $(src)devices-db.c -o $(obj)devices-db.o $(i) -Wall

$(obj)command-executor.o: $(src)command-executor.c $(headers)command-executor.h
	@echo Compiling command-executor
	@$(cc) -c $(src)command-executor.c -o $(obj)command-executor.o $(i) -Wall

clean:
	@del bin\*.o
	@del barnet.exe
	@del /q /f /s *~
	@del /q /f /s source\*~
	@del /q /f /s include\*~
	@echo deleted all bin and vim files
