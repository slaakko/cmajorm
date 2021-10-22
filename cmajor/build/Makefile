include Makefile.common

OBJECTS = Action.o Build.o BuildClient.o BuildLangClassMap.o BuildLangClientParser.o BuildLangKeywords.o \
BuildLangLexer.o BuildLangOptionParser.o BuildLangServerParser.o BuildLangTokens.o BuildOption.o \
BuildServer.o BuildServerMessage.o ClientCommand.o ClientExecutionContext.o Connection.o ExecutionContext.o \
FiberConnection.o FiberExecutionContext.o Host.o InitDone.o LocalExecutionContext.o Log.o Message.o MessageBody.o \
MessageHandler.o ServerCommand.o ServerConfig.o ServerExecutionContext.o SocketClient.o SocketConnection.o SocketServer.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmbuild.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmbuild.a
