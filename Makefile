# Flags to detect 32-bit or 64-bit OS platform
OS_SIZE = $(shell uname -m | sed -e "s/i.86/32/" -e "s/x86_64/64/")
OS_ARCH = $(shell uname -m | sed -e "s/i386/i686/")

# These flags will override any settings
ifeq ($(i386),1)
	OS_SIZE = 32
	OS_ARCH = i686
endif

ifeq ($(x86_64),1)
	OS_SIZE = 64
	OS_ARCH = x86_64
endif


# OS-specific build flags
ifeq ($(OS_SIZE),32)
	CCFLAGS   := -m32
else
	CCFLAGS   := -m64
endif

CPPINCLUDES     ?=$(CCFLAGS)
CURRPATH	?= $(abspath .)
CXX		?= g++ 
CXXDBGFLAGS     ?= -g -ggdb
CXXOPTFLAGS     ?= -O2 -march=native 
######################################################################################
#
# Boost related libraries only required for building the tests
# for more information http://www.boost.org/
#
ifdef BOOST_INC
        CPPINCLUDES += -I$(BOOST_INC)
endif

ifdef BOOST_LIB
        CPPINCLUDES += -L$(BOOST_LIB)
	BOOSTPRESENT=$(BOOST_INC)
else
        BOOSTPRESENT=$(shell whereis libboost_unit_test_framework.so|cut -d " " -f2)
endif

TESTINCLUDES    ?= -lboost_unit_test_framework 


######################################################################################
#
# Target rules
#
SRCFILES	 ?= $(wildcard *.cpp)
MPISRCFILES	 ?= $(wildcard mpi*.cpp)
TBBSRCFILES	 ?= $(wildcard tbb*.cpp)
SRCFILESWOTBB     = $(filter-out $(TBBSRCFILES),$(SRCFILES))
SRCFILESWOMPI     = $(filter-out $(MPISRCFILES),$(SRCFILES))
NONPARALLELFILES ?= $(filter-out $(TBBSRCFILES),$(filter-out $(MPISRCFILES),$(SRCFILES)))
CPPFILES          = $(NONPARALLELFILES) #start out with files w/o any dependency
CPPINCLUDES      ?= 


FILESNOTBUILT    = $(filter-out $(CPPFILES),$(SRCFILES))

TESTFILES	?= $(wildcard *.tpp)

TESTEXECUTABLES	?= $(patsubst %.tpp,%.texe,$(TESTFILES))
CPPEXECUTABLES	?= $(patsubst %.cpp,%.exe,$(CPPFILES))
CPPOPTEXECUTABLES	?= $(patsubst %.cpp,%.exeO,$(CPPFILES))



######################################################################################
#
# Target rules
#

all : $(CPPEXECUTABLES) 
	$(if $(FILESNOTBUILT),@echo -e "\nunable to build \n\t>> $(FILESNOTBUILT)\ndue to incomplete environment setup")


opt : $(CPPOPTEXECUTABLES) 
	$(if $(FILESNOTBUILT),@echo -e "\nunable to build \n\t$(FILESNOTBUILT)\ndue to incomplete environment setup")

tests : $(TESTEXECUTABLES)

%.texe : %.tpp boost_configured
	@echo "Building test $(BINDIR)/$@ from $<!"
	$(CXX) -x c++ -ggdb $< $(TESTINCLUDES) $(CPPINCLUDES) -o $(CURRPATH)/$@
	@echo "executing $@\n"
	$(CURRPATH)/$@

%.exe : %.cpp 
	@echo "Building dbg target $@ from $<!"
	$(CXX)  $(CXXDBGFLAGS) $< $(CPPINCLUDES) -o $(CURRPATH)/$@

%.exeO : %.cpp 
	@echo "Building opt target for $@ from $<!"
	$(CXX) -x c++ $(CXXOPTFLAGS) $< $(CPPINCLUDES) -o $(CURRPATH)/$@

clean:
	@echo "removing all binaries"	
	rm -fv *.?exe *.exe? *.exe

boost_configured :
	@if [ -z $(BOOSTPRESENT)  ] ; then echo -e "\n   !!!! ERROR: BOOST not set up properly!\n >> http://www.boost.org/\n"; exit 1; fi

print_vars :
	@echo -e "CPPFILES:	 " $(CPPFILES)
	@echo -e "CPPEXECUTABLES:" $(CPPEXECUTABLES)
	@echo -e "FILESNOTBUILT: " $(FILESNOTBUILT)
	@echo -e "TBBPRESENT:	 " $(TBBPRESENT)
	@echo -e "MPIPRESENT:    " $(MPIPRESENT)
	@echo -e "SRCFILESWOTBB:    " $(SRCFILESWOTBB)
	@echo -e "SRCFILESWOMPI:    " $(SRCFILESWOMPI)
	@echo -e "NONPARALLELFILES:    " $(NONPARALLELFILES)
