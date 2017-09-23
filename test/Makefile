# 'make'        build executable file 'k2_test'
# 'make all'	same as 'make'
# 'make clean'  removes all .o and executable files

# Unit unit test framework
	SEATEST	=	../thirdparty/seatest/src
	K2_SRC	=	../src


	CC	=	gcc

# NOTE: setting the inline limit values to be obscenely high since the size of
#		the binary isn't a concern
#	CFLAGS	=	-O3 -std=iso9899:2011 \
#				-flto \
#				-march=native \
#				-mtune=native \
#				-lmcheck \-static 
#				-fsanitize=address \
#				-fomit-frame-pointer \
#				--param large-function-growth=20000 \
#				--param max-inline-insns-single=90000 \
#				-Wpedantic \
#				-Wformat=2 \
#				-Wall \
#				-Wstrict-prototypes \
#				-Wmissing-prototypes \
#				-Werror \
#				-Wlogical-op \
#				-Wextra \
#				-Wconversion \
#				-Wsign-conversion \
#				-Wshadow \
#				-Wcast-qual \
#				-Wcast-align \
#				-Wmissing-include-dirs \
#				-Wpointer-arith


# flags for profiling code
#	CFLAGS	=	-O3 -pg -std=iso9899:2011


# more lax compiler options
	CFLAGS	=	-Wall -g -std=iso9899:2011 -lubsan

# define any directories containing header files other than /usr/include
#
	INCLUDES =	-I. -I$(SEATEST) -I$(K2_SRC)/board

# define the C source files

	SRCS	=	$(SEATEST)/seatest.c \
				$(K2_SRC)/board/piece.c \
				test_launcher.c \
				test_piece.c

# define the C object files
#
	OBJS	=	$(SRCS:.c=.o)


# define the executable files
	MAIN	=	k2_test

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY:	depend	clean

all:	$(MAIN)
		@echo  'k2_test' build complete

$(MAIN):		$(OBJS)
				$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS) 


.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend:	$(SRCS)
		makedepend $(INCLUDES) $^
# DO NOT DELETE THIS LINE -- make depend depends on it.
