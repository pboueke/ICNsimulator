CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS = BaseNode.o ClientNode.o RouterBaseNode.o ServerNode.o DistUtility.o ICNSimulator.o Logger.o OperationManager.o SimConfiguration.o StringUtility.o TestUnits.o TimeHandler.o VectorUtility.o

LIBS =

TARGET =	ICNSimulator

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
