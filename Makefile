all: PowerMeter
PowerMeter:     PowerMeter.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lrt 
clean:
	$(RM) *.o
