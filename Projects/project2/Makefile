cc = gcc

sources = part1.c part2.c part3.c part4.c
objects = $(addsuffix .o, $(basename $(sources)))

flags = -g

target = part1 part2 part3 part4

all: $(target) 

$(target) : $(objects)
	$(cc) $(flags) -o $(target) $(objects)

%.o : %.c
	$(cc) -c $(flags) $< -o $@

clean:
	rm -rf $(target) $(objects) 