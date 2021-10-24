# Note: run "make depend" before running "make",
# to ensure that generated source and header files are
# created properly.

C_SRCS = util.c parse.tab.c lex.yy.c grammar_symbols.c treeprint.c
C_OBJS = $(C_SRCS:%.c=%.o)

# You will probably want to add
#    symbol.cpp symtab.cpp type.cpp 
# to CXX_SRCS when you implement types and symbol tables.
CXX_SRCS = main.cpp cpputil.cpp node.cpp ast.cpp context.cpp \
	astvisitor.cpp
CXX_OBJS = $(CXX_SRCS:%.cpp=%.o)

CC = gcc
CFLAGS = -g -Wall

CXX = g++
CXXFLAGS = $(CFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

all : compiler

compiler : $(C_OBJS) $(CXX_OBJS)
	$(CXX) -o $@ $(C_OBJS) $(CXX_OBJS)

parse.tab.c : parse.y
	bison -d parse.y

lex.yy.c : lex.l
	flex lex.l

grammar_symbols.h grammar_symbols.c : parse.y scan_grammar_symbols.rb
	./scan_grammar_symbols.rb < parse.y

clean :
	rm -f compiler *.o
	rm -f parse.tab.c lex.yy.c parse.tab.h grammar_symbols.h grammar_symbols.c depend.mak

depend : grammar_symbols.h grammar_symbols.c parse.tab.c lex.yy.c
	$(CC) $(CFLAFGS) -M $(C_SRCS) > depend.mak
	$(CXX) $(CXXFLAGS) -M $(CXX_SRCS) >> depend.mak

depend.mak :
	touch $@

include depend.mak
