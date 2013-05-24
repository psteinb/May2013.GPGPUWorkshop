
ifdef CPPINCLUDES
	CPPINCLUDES +=  -lboost_date_time 
else
	CPPINCLUDES = -lboost_date_time 
endif

CPPINCLUDES += $(ROOTLIBS) $(ROOTCFLAGS)

ifdef BOOST_INC
	TESTINCLUDES += -I$(BOOST_INC)
	CPPINCLUDES += -I$(BOOST_INC)
	BOOSTPRESENT=$(BOOST_INC)
else
	BOOSTPRESENT=$(shell whereis libboost_unit_test_framework.so|cut -d " " -f3)	
endif

ifdef BOOST_LIB
	TESTINCLUDES += -L$(BOOST_LIB)
	CPPINCLUDES += -L$(BOOST_LIB)	
endif

ifdef TESTINCLUDES
TESTINCLUDES	+= -lboost_unit_test_framework
else
TESTINCLUDES	+= -lboost_unit_test_framework
endif

include ../Makefile
