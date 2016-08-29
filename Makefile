
TARGET = sfbtxt

GCC = gcc

SRC = font.c main.c
OBJ = $(SRC:.c=.o)

FONTS = fonts/description.txt

all: $(TARGET)

$(TARGET): $(OBJ)
	$(GCC) -o $@ $(OBJ)

%.o: %.c
	$(GCC) -c $<

font.c: $(FONTS)
	@echo "#include \"font.h\"" > $@
	perl -w buildfont.pl fonts/description.txt >> $@

clean:
	rm -f *.o font.c $(TARGET)
