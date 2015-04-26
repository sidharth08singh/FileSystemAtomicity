CC=gcc
CFLAGS= -ggdb -Wall

TARGET = project

$(TARGET): WriteFile.c ReadFile.c LogRecord.c Recovery.c CreateFile.c FaultyWrite1.c FaultyWrite2.c FaultyWrite3.c JournalManager.c
	$(CC) $(CFLAGS) -o $(TARGET) WriteFile.c ReadFile.c LogRecord.c Recovery.c CreateFile.c FaultyWrite1.c FaultyWrite2.c FaultyWrite3.c JournalManager.c -lpthread -I.

clean: 
	$(RM) $(TARGET) 
