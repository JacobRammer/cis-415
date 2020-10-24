cc = gcc

sources = main.c command.c
objects = $(addsuffix .o, $(basename $(sources)))

flags = -g

target = pseudu-shell

all: $(target) 

$(target) : $(objects)
	$(cc) $(flags) -o $(target) $(objects)

%.o : %.c
	$(cc) -c $(flags) $< -o $@

clean:
	rm -rf $(target) $(objects) 
