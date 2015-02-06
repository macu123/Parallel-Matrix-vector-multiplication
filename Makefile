CC = mpicc
CFLAGS = -Wall -o2
TARGET = matvec
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c
clean:
	$(RM) $(TARGET)
