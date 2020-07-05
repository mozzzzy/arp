ARTIFUCT = ./main

CXX = g++

CXXFLAGS = \
  -g

CPPFLAGS = \
  -I/opt/cppunit_latest/include \
  -I../src

LDFLAGS = \
  -lcppunit

LDLIBS = \
  -L/opt/cppunit_latest/lib

CXXSRC = \
  ../src/arp.c \
  main.cc

$(ARTIFUCT): $(CXXSRC)

test: $(ARTIFUCT)
	$(ARTIFUCT)

clean:
	rm -f  $(ARTIFUCT)
