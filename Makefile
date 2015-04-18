CC=gcc
CFLAGS= -ggdb -Wall

TARGET = project

$(TARGET): LogRecord.c WriteFile.c ReadFile.c CreateFile.c FaultyWrite1.c FaultyWrite2.c JournalManager.c
	$(CC) $(CFLAGS) -o $(TARGET) LogRecord.c WriteFile.c ReadFile.c CreateFile.c FaultyWrite1.c FaultyWrite2.c JournalManager.c -lpthread -I.

clean: 
	$(RM) $(TARGET) 
