#HDFINC=-I/usr/include/hdf5/serial/
#HDFLIB=-L/usr/lib/x86_64-linux-gnu/hdf5/serial/ -lhdf5_hl -lhdf5
#or set CPATH and LIBRARY_PATH variable locally 
HDFINC=
HDFLIB=-lhdf5_hl -lhdf5 
#HDFINC+=-Wno-literal-suffix #to disable the annoying literal-suffix warning with the hdf header

###comment out the following lines if you do not have GSL (for eigenvalue decomposition of inertial tensors)
#GSLINC=
#GSLINC+=-DHAS_GSL 
#GSLLIB=-lgsl -lgslcblas
###comment up to here to disable GSL.

#CC=mpicc
CXX=mpiCC
CXXFLAGS+=-std=c++0x $(HDFINC) $(GSLINC) $(OMPFLAG) -O3 -Wno-deprecated -g 

###for Dark Matter only simulations
#CXXFLAGS+=-DDM_ONLY
###

##enable these if you want to obtain (thermal) binding energy for each particle
###save binding energy:
#CXXFLAGS+=-DSAVE_BINDING_ENERGY
###
###load thermal energy (also set SaveSubParticleProperties to 1 in config file if you want to save thermal energy)
#CXXFLAGS+=-DHAS_THERMAL_ENERGY
###

###for thermal unbinding:
#CXXFLAGS+=-DUNBIND_WITH_THERMAL_ENERGY
### alternatively, you can save both the non-thermal binding energy and the thermal energy and do it yourself. see above.

#CXXFLAGS+=-DMPICH_IGNORE_CXX_SEEK -DMPICH_SKIP_MPICXX  -idirafter /usr/include/linux #to work around intel compiler bugs
#CXXFLAGS+=-xHost #to get it runnable on cosma-e 

LDLIBS+=$(HDFLIB) $(OMPFLAG) $(GSLLIB)
LDFLAGS+=-g
LINK.o=$(CXX) $(LDFLAGS) $(TARGET_ARCH)

ifneq (,$(filter $(CXX),mpic++ mpicxx mpiCC))
BASECXX=$(shell $(CXX) -show|cut -c1-3)
else
BASECXX=$(CXX)
endif
  
ifeq ($(BASECXX), g++)
#OMPFLAG=-fopenmp
else
#OMPFLAG=-qopenmp
CXXFLAGS+=-diag-disable 3180  #to disable omp warnings by icc when not using omp; comment this out if you enabled OMPFLAG
endif
  
%.test.o: %.cpp
	$(CXX) $^ -DTEST_$(basename $<) $(CXXFLAGS) -c -o $@
%.test: %.test.o
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $@
$(TEST): CXXFLAGS+=-g -gdwarf-2 -O0

SUBDIRS:=$(wildcard */.)
SUBDIRSCLEAN=$(addsuffix clean,$(SUBDIRS))

clean: clean_curdir $(SUBDIRSCLEAN)

clean_curdir:
	rm -rf *.o *~ core a.out $(EXE) $(OUTPUTS) *.test

%clean: %
	$(MAKE) -C $< clean

depend_curdir:
	makedepend --$(CXXFLAGS)-- -Y $(SRC)
