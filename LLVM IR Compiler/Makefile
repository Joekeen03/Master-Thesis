SRC_DIR = .
TOKEN_DIR = $(SRC_DIR)/Tokens
BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/objects
TEMPLATE_DIR = $(SRC_DIR)/Templates
TEMPLATE_RESULT_DIR = $(SRC_DIR)/TemplateResult

TOKEN_OBJECTS = TokenComment.o TokenGlobalIdentifier.o TokenLibrary.o TokenKeyword.o TokenOperator.o TokenString.o \
				TokenParenthesis.o TokenAttributeID.o TokenCurlyBrace.o
OBJECTS = Tokenizer.o BasicCharArray.o main.o

TEMPLATE_H_FILE := $(TEMPLATE_DIR)/TokenTEMPLATE.h
TEMPLATE_CPP_FILE := $(TEMPLATE_DIR)/TokenTEMPLATE.cpp

prepareDirectories:
	- mkdir $(subst /,\\,$(OBJECT_DIR))

clean:
	-del $(subst /,\\,$(OBJECT_DIR))\\*.o

tokenizer: prepareDirectories $(patsubst %, $(SRC_DIR)/%, $(OBJECTS)) $(patsubst %, $(TOKEN_DIR)/%, $(TOKEN_OBJECTS))
	$(CXX) $(CFLAGS) $(patsubst %, $(OBJECT_DIR)/%, $(OBJECTS) $(TOKEN_OBJECTS)) \
	$(LDFLAGS) -o $(BUILD_DIR)/$@
	./$(BUILD_DIR)/$@

NewToken%: token_dir := $(subst /,\\,$(TOKEN_DIR)/)
NewToken%: h_data := $(file <$(subst /,\\,$(TEMPLATE_H_FILE)))
NewToken%: cpp_data := $(file <$(subst /,\\,$(TEMPLATE_CPP_FILE)))
NewToken%:
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewToken,Token,$@).h),$(subst TEMPLATE,$(subst NewToken,,$@),$(h_data)))
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewToken,Token,$@).cpp),$(subst TEMPLATE,$(subst NewToken,,$@),$(cpp_data)))

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $(OBJECT_DIR)/$(subst .cpp,.o,$(<F))
