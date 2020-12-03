cc = gcc

sources = quacker.c
objects = $(addsuffix .o, $(basename $(sources)))

flags = -g -pthread

target = a.out

all: $(target) 

$(target) : $(objects)
	$(cc) $(flags) -o $(target) $(objects)

%.o : %.c
	$(cc) -c $(flags) $< -o $@

clean:
	rm -rf $(target) $(objects) 
