include ../Makefile.common

OBJECTS = BoundDebugExpr.o BoundDebugNodeVisitor.o Container.o ContainerSubscriptTranslator.o DebugExpr.o DebugExprBinder.o DebugExprClassMap.o \
DebugExpressionEvaluator.o DebugExprKeywords.o DebugExprLexer.o DebugExprParser.o DebugExprTokens.o DebugExprVisitor.o \
Debugger.o DebuggerCommandClassMap.o DebuggerCommandKeywords.o DebuggerCommandLexer.o DebuggerCommandParser.o \
DebuggerCommandTokens.o DebugInfo.o DebugInfoIo.o DebugInfoToXml.o DIEnumConstant.o DIType.o DIVariable.o \
Gdb.o GdbReplyClassMap.o GdbReplyKeywords.o GdbReplyLexer.o GdbReplyLineParser.o GdbReplyTokens.o \
InitDone.o TokenValueParsers.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmdebug.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmdebug.a
