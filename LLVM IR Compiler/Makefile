SRC_DIR = .
TOKEN_DIR = $(SRC_DIR)/Tokens
BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/objects

TOKEN_OBJECTS = TokenComment.o TokenGlobalIdentifier.o TokenLibrary.o TokenKeyword.o TokenOperator.o TokenString.o
OBJECTS = Tokenizer.o BasicCharArray.o main.o

prepareDirectories:
	- mkdir $(subst /,\\,$(OBJECT_DIR))

clean:
	-del $(subst /,\\,$(OBJECT_DIR))\\*.o

tokenizer: prepareDirectories $(patsubst %, $(SRC_DIR)/%, $(OBJECTS)) $(patsubst %, $(TOKEN_DIR)/%, $(TOKEN_OBJECTS))
	$(CXX) $(CFLAGS) $(patsubst %, $(OBJECT_DIR)/%, $(OBJECTS) $(TOKEN_OBJECTS)) \
	$(LDFLAGS) -o $(BUILD_DIR)/$@
	./$(BUILD_DIR)/$@

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $(OBJECT_DIR)/$(subst .cpp,.o,$(<F))
