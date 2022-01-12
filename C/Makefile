#------------------------------------------------------------------------------
#  Makefile for CSE 101 Programming Assignment 6
#
#  make                  makes Order
#  make WordFrequency    makes WordFrequency
#  make clean            removes all binaries
#  make check1           checks WordFrequency for memory leaks on Big.txt 
#  make check2           checks Order for memory leaks on English.txt
#------------------------------------------------------------------------------

TOP              = Order
TOP_OBJ          = $(TOP).o
TOP_SRC          = $(TOP).c
ADT              = Dictionary
TEST             = WordFrequency
TESTER			 = $(ADT)Test
TESTER_OBJ       = $(TESTER).o
TESTER_SRC		 = $(TESTER).c
TEST_OBJ         = $(TEST).o
TEST_SRC         = $(TEST).c
ADT_OBJ          = $(ADT).o
ADT_SRC          = $(ADT).c
ADT_HDR          = $(ADT).h
COMPILE          = gcc -std=c11 -Wall -Wextra -c 
LINK             = gcc -std=c11 -Wall -Wextra -o
REMOVE           = rm -f
MEMCHECK         = valgrind --leak-check=full

$(TOP) : $(TOP_OBJ) $(ADT_OBJ)
	$(LINK) $(TOP) $(TOP_OBJ) $(ADT_OBJ)

$(TEST) : $(TEST_OBJ) $(ADT_OBJ)
	$(LINK) $(TEST) $(TEST_OBJ) $(ADT_OBJ)

$(TOP_OBJ) : $(TOP_SRC) $(ADT_HDR)
	$(COMPILE) $(TOP_SRC)

$(TESTER) : $(TESTER_OBJ) $(ADT_OBJ)
	$(LINK) $(TESTER) $(TESTER_OBJ) $(ADT_OBJ)

$(TESTER_OBJ) : $(TESTER_SRC) $(ADT_HDR)
	$(COMPILE) $(TESTER_SRC)

$(TEST_OBJ) : $(TEST_SRC) $(ADT_HDR)
	$(COMPILE) $(TEST_SRC)

$(ADT_OBJ) : $(ADT_SRC) $(ADT_HDR)
	$(COMPILE) $(ADT_SRC)

clean :
	$(REMOVE) $(TOP) $(TOP_OBJ) $(TEST) $(TEST_OBJ) $(ADT_OBJ) $(TESTER) $(TESTER_OBJ)

