SRC_DIR = .
TOKEN_DIR = $(SRC_DIR)/Tokens
EXPRESSION_DIR = $(SRC_DIR)/ParseExpressions
CODEGEN_DIR = $(SRC_DIR)/CodeGen
SNES_INSTRUCTIONS_DIR = $(CODEGEN_DIR)/SNESInstructions

BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/objects

TEMPLATE_DIR = $(SRC_DIR)/Templates
TEMPLATE_RESULT_DIR = $(SRC_DIR)/TemplateResult
TEST_DIR = $(SRC_DIR)/Test
TEST_OUTPUT_DIR = $(SRC_DIR)/TestOutput

EXTRA_CFLAGS = -std=c++17 
EXECUTABLE_NAME = main

DIRS = $(SRC_DIR) $(TOKEN_DIR) $(EXPRESSION_DIR) $(CODEGEN_DIR) $(SNES_INSTRUCTION_OBJECTS)
SRC_FILES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)%.cpp=$(OBJECT_DIR)%.o)
DEP_FILES = $(OBJ_FILES:.o=.d)

TEST_CPP_FILES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_LL_FILES = $(TEST_CPP_FILES:%.cpp=%.ll)

TEMPLATE_TOKEN_H_FILE := $(TEMPLATE_DIR)/TokenTEMPLATE.h
TEMPLATE_TOKEN_CPP_FILE := $(TEMPLATE_DIR)/TokenTEMPLATE.cpp
TEMPLATE_EXPRESSION_H_FILE := $(TEMPLATE_DIR)/ExpressionTEMPLATE.h
TEMPLATE_LLVMIR_INSTRUCTION_NORMAL_YIELDS_VALUE_H_FILE := $(TEMPLATE_DIR)/InstructionNonTerminatorYieldsValueTEMPLATE.h
TEMPLATE_LLVMIR_INSTRUCTION_NORMAL_YIELDS_VOID_H_FILE := $(TEMPLATE_DIR)/InstructionNonTerminatorYieldsVoidTEMPLATE.h
TEMPLATE_LLVMIR_INSTRUCTION_TERMINATOR_YIELDS_VOID_H_FILE := $(TEMPLATE_DIR)/InstructionTerminatorYieldsVoidTEMPLATE.h
TEMPLATE_SNES_INSTRUCTION_H_FILE := $(TEMPLATE_DIR)/SNESInstructionTEMPLATE.h
TEMPLATE_GENERIC_H_FILE := $(TEMPLATE_DIR)/HeaderTEMPLATE.h

clean:
	-del $(subst /,\\,$(OBJECT_DIR))\\*.o
	-del $(subst /,\\,$(OBJECT_DIR))\\*.d

compiler: $(EXECUTABLE_NAME)
	./$(BUILD_DIR)/$(EXECUTABLE_NAME) -ll Test/simpleAddition.ll -asm TestOutput/simpleAddition.asm

info:
	$(info $(OBJ_FILES))

$(EXECUTABLE_NAME): $(OBJ_FILES)
	$(CXX) $(CFLAGS) $(EXTRA_CFLAGS) $(OBJ_FILES) $(LDFLAGS) -o $(BUILD_DIR)/$(EXECUTABLE_NAME)

NewToken%: h_data := $(file <$(subst /,\\,$(TEMPLATE_TOKEN_H_FILE)))
NewToken%: cpp_data := $(file <$(subst /,\\,$(TEMPLATE_TOKEN_CPP_FILE)))
NewToken%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewToken,Token,$@).h),$(subst TEMPLATE,$(subst NewToken,,$@),$(h_data)))
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewToken,Token,$@).cpp),$(subst TEMPLATE,$(subst NewToken,,$@),$(cpp_data)))

NewExpression%: h_data := $(file <$(subst /,\\,$(TEMPLATE_EXPRESSION_H_FILE)))
NewExpression%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewExpression,Expression,$@).h),$(subst TEMPLATE,$(subst NewExpression,,$@),$(h_data)))

NewLLVMIRInstructionNonTerminatorYieldsValue%: h_data := $(file <$(subst /,\\,$(TEMPLATE_LLVMIR_INSTRUCTION_NORMAL_YIELDS_VALUE_H_FILE)))
NewLLVMIRInstructionNonTerminatorYieldsValue%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewLLVMIRInstructionNonTerminatorYieldsValue,Instruction,$@).h),$(subst TEMPLATE,$(subst NewLLVMIRInstructionNonTerminatorYieldsValue,,$@),$(h_data)))

NewLLVMIRInstructionNonTerminatorYieldsVoid%: h_data := $(file <$(subst /,\\,$(TEMPLATE_LLVMIR_INSTRUCTION_NORMAL_YIELDS_VOID_H_FILE)))
NewLLVMIRInstructionNonTerminatorYieldsVoid%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewLLVMIRInstructionNonTerminatorYieldsVoid,Instruction,$@).h),$(subst TEMPLATE,$(subst NewLLVMIRInstructionNonTerminatorYieldsVoid,,$@),$(h_data)))

NewLLVMIRInstructionTerminatorYieldsVoid%: h_data := $(file <$(subst /,\\,$(TEMPLATE_LLVMIR_INSTRUCTION_TERMINATOR_YIELDS_VOID_H_FILE)))
NewLLVMIRInstructionTerminatorYieldsVoid%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewLLVMIRInstructionTerminatorYieldsVoid,Instruction,$@).h),$(subst TEMPLATE,$(subst NewLLVMIRInstructionTerminatorYieldsVoid,,$@),$(h_data)))

NewSNESInstruction%: h_data := $(file <$(subst /,\\,$(TEMPLATE_SNES_INSTRUCTION_H_FILE)))
NewSNESInstruction%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewSNESInstruction,SNESInstruction,$@).h),$(subst TEMPLATE,$(subst NewSNESInstruction,,$@),$(h_data)))

NewHeader%: h_data := $(file <$(subst /,\\,$(TEMPLATE_GENERIC_H_FILE)))
NewHeader%: | $(TEMPLATE_RESULT_DIR)/
	$(file >$(subst /,\\,$(TEMPLATE_RESULT_DIR)/$(subst NewHeader,,$@).h),$(subst TEMPLATE,$(subst NewHeader,,$@),$(h_data)))

%.ll : %.cpp ; $(CXX) $(CFLAGS) -c $< -o $(TEST_DIR)/$(subst .cpp,.ll,$(<F)) -S -emit-llvm

$(TEST_OUTPUT_DIR)/%.asm : $(TEST_OUTPUT_DIR)/%.smc ; $(TEST_OUTPUT_DIR)/wla.bat %

prepareTests: $(TEST_LL_FILES)

$(OBJECT_DIR)/%.o: %.cpp | $(@D)/
	$(CXX) -MMD -MP -I. $(EXTRA_CFLAGS) $(CFLAGS) $(LDFLAGS) -c $< -o $@

%/:
	$(info $@)
	mkdir $(subst /,\\,$@)

testing: $(OBJECT_DIR)/Tokens/HelperFunctionsTokens.o
	$(info Testing)
	$(info $(DEP_FILES))
	$(info $(DIRS))
	$(info $(TEST_CPP_FILES))
	$(info $(TEST_LL_FILES))

.PHONY: testing

-include $(DEP_FILES)