SRC_DIR = .
TOKEN_DIR = $(SRC_DIR)/Tokens
EXPRESSION_DIR = $(SRC_DIR)/ParseExpressions
BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/objects
TEMPLATE_DIR = $(SRC_DIR)/Templates
TEMPLATE_RESULT_DIR = $(SRC_DIR)/TemplateResult
TEST_DIR = $(SRC_DIR)/Test

TOKEN_OBJECTS = TokenComment.o TokenGlobalIdentifier.o LibraryTokens.o TokenKeyword.o TokenOperator.o TokenString.o \
				TokenParenthesis.o TokenAttributeID.o TokenCurlyBrace.o TokenBaseIdentifier.o TokenLocalIdentifier.o \
				TokenNamedMetadata.o HelperFunctionsTokens.o TokenNumberLiteral.o TokenMetadataNodeStart.o \
				TokenMetadataString.o TokenComma.o TokenBaseUnnamedIdentifier.o TokenLocalUnnamedIdentifier.o \
				TokenGlobalUnnamedIdentifier.o TokenTypeInteger.o TokenTypeReservedWord.o
EXPRESSION_OBJECTS = 
OBJECTS = Tokenizer.o BasicCharArray.o main.o ParseResultSourceFile.o EnumRegistry.o Parser.o AttributeIDProcessor.o
TEST_LL_FILES = mainOnly.ll

TEMPLATE_TOKEN_H_FILE := $(TEMPLATE_DIR)/TokenTEMPLATE.h
TEMPLATE_TOKEN_CPP_FILE := $(TEMPLATE_DIR)/TokenTEMPLATE.cpp
TEMPLATE_EXPRESSION_H_FILE := $(TEMPLATE_DIR)/ExpressionTEMPLATE.h
TEMPLATE_INSTRUCTION_YIELDS_VALUE_H_FILE := $(TEMPLATE_DIR)/InstructionYieldsValueTEMPLATE.h
TEMPLATE_INSTRUCTION_YIELDS_VOID_H_FILE := $(TEMPLATE_DIR)/InstructionYieldsVoidTEMPLATE.h
TEMPLATE_GENERIC_H_FILE := $(TEMPLATE_DIR)/HeaderTEMPLATE.h

prepareDirectories:
	- mkdir $(subst /,\\,$(OBJECT_DIR))
	- mkdir $(subst /,\\,$(TEMPLATE_RESULT_DIR))

clean:
	-del $(subst /,\\,$(OBJECT_DIR))\\*.o

tokenizer: prepareDirectories $(patsubst %,$(SRC_DIR)/%,$(OBJECTS)) $(patsubst %,$(TOKEN_DIR)/%,$(TOKEN_OBJECTS)) \
		   $(patsubst %,$(EXPRESSION_DIR)/%,$(EXPRESSION_OBJECTS))
	$(CXX) $(CFLAGS) $(patsubst %,$(OBJECT_DIR)/%,$(OBJECTS) $(TOKEN_OBJECTS) $(EXPRESSION_OBJECTS)) \
	$(LDFLAGS) -o $(BUILD_DIR)/$@
	./$(BUILD_DIR)/$@

NewToken%: h_data := $(file <$(subst /,\\,$(TEMPLATE_TOKEN_H_FILE)))
NewToken%: cpp_data := $(file <$(subst /,\\,$(TEMPLATE_TOKEN_CPP_FILE)))
NewToken%: prepareDirectories
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewToken,Token,$@).h),$(subst TEMPLATE,$(subst NewToken,,$@),$(h_data)))
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewToken,Token,$@).cpp),$(subst TEMPLATE,$(subst NewToken,,$@),$(cpp_data)))

NewExpression%: h_data := $(file <$(subst /,\\,$(TEMPLATE_EXPRESSION_H_FILE)))
NewExpression%: prepareDirectories
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewExpression,Expression,$@).h),$(subst TEMPLATE,$(subst NewExpression,,$@),$(h_data)))

NewInstructionYieldsValue%: h_data := $(file <$(subst /,\\,$(TEMPLATE_INSTRUCTION_YIELDS_VALUE_H_FILE)))
NewInstructionYieldsValue%: prepareDirectories
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewInstructionYieldsValue,Instruction,$@).h),$(subst TEMPLATE,$(subst NewInstructionYieldsValue,,$@),$(h_data)))

NewInstructionYieldsVoid%: h_data := $(file <$(subst /,\\,$(TEMPLATE_INSTRUCTION_YIELDS_VOID_H_FILE)))
NewInstructionYieldsVoid%: prepareDirectories
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewInstructionYieldsVoid,Instruction,$@).h),$(subst TEMPLATE,$(subst NewInstructionYieldsVoid,,$@),$(h_data)))

NewHeader%: h_data := $(file <$(subst /,\\,$(TEMPLATE_GENERIC_H_FILE)))
NewHeader%: prepareDirectories
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewHeader,,$@).h),$(subst TEMPLATE,$(subst NewHeader,,$@),$(h_data)))

%.ll : %.cpp ; $(CXX) $(CFLAGS) -c $< -o $(TEST_DIR)/$(subst .cpp,.ll,$(<F)) -S -emit-llvm

prepareTests: $(patsubst %,$(TEST_DIR)/%,$(TEST_LL_FILES))

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $(OBJECT_DIR)/$(subst .cpp,.o,$(<F))