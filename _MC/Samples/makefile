

.PHONY: clean all

all: test_cutpoint.exe


test_cutpoint.exe: test_cutpoint.exe.cpp Cutpoint.cpp 
	$(CXX) -o $@ $^


clean:
	-rm -f *~ *.exe *.o