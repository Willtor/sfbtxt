
TARGET = sfbtxt
TARGET_BENCH = sfbtxt_bench

GCC = gcc

SRC = font.c main.c
OBJ = $(SRC:.c=.o)

SRC2 = font.c main_bench.c
OBJ2 = $(SRC2:.c=.o)

LIBS = -lpng

FONTS = fonts/description.txt

all: $(TARGET) $(TARGET_BENCH)

$(TARGET): $(OBJ)
	$(GCC) -O3 -o $@ $(OBJ) $(LIBS)

$(TARGET_BENCH): $(OBJ2)
	$(GCC) -O3 -o $@ $(OBJ2) $(LIBS)

%.o: %.c
	$(GCC) -c -O3  $<

font.c: $(FONTS)
	@echo "#include \"font.h\"" > $@
	perl -w buildfont.pl fonts/description.txt >> $@

clean:
	rm -f *.o font.c $(TARGET) $(TARGET_BENCH)
