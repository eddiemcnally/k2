#
# 'make depend' uses makedepend to automatically generate dependencies
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'K2'
# 'make all'	makes executable file 'K2'
# 'make clean'  removes all .o and executable file
	CC	=	clang
	
# NOTE: setting the inline limit values to be obscenely high since the size of
#		the binary isn't a concern
	CFLAGS	=	-O3 \
			-flto \
			-std=c11 \
			-march=native \
			-mtune=native \
			-Wpedantic \
			-Wformat=2 \
			-Wall \
			-Wextra \
			-Wstrict-prototypes \
			-Wmissing-prototypes \
			-Wswitch-default \
			-Wunused \
			-Wunused-macros \
			-Werror \
			-Wextra \
			-Wconversion \
			-Wsign-conversion \
			-Wshadow \
			-Wcast-qual \
			-Wcast-align \
			-Wmissing-include-dirs \
			-Wpointer-arith \
			-fstack-protector \
			-fsanitize=address \
			-fsanitize=undefined \
			-fomit-frame-pointer \
			-DENABLE_ASSERTS 


# flags for profiling code
#	CFLAGS	=	-O3 -pg -std=iso9899:2011

# more lax compiler options
#	CFLAGS	=	-Wall -g -std=iso9899:2011 -flto -march=native -mtune=native 

# define any directories containing header files other than /usr/include
#
	INCLUDES =	-I. -Iboard -Iutils

# define the C source files
	SRCS		=	main.c board/piece.c board/square.c


# define the C object files
#
	OBJS		=	$(SRCS:.c=.o)


# define the executable files
	MAIN		=	K2

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY:	depend	clean

all:	$(MAIN)
		@echo  'K2' build complete

$(MAIN):		$(OBJS)
				$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)


.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend:	$(SRCS)
		makedepend $(INCLUDES) $^
# DO NOT DELETE THIS LINE -- make depend depends on it.
