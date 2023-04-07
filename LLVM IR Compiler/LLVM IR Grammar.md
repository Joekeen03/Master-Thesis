file ::= source_file [data_layout] [target_triple] ([globalValue] | [meta])<br>
source_file ::= 'source_filename' '=' string

// Layout<br>
data_layout ::= 'target' 'datalayout' '=' string<br>
alignment ::= integer // Must be a power of two in some, but not all cases.<br>

target_triple ::= 'target' 'triple' '=' string<br>

globalValue ::= globalVariable | functionDefinition | functionDeclaration | metadataDefinition<br>

// Global Variables<br>
globalVariable ::= globalVariableDeclaration | globalVariableDefinition<br>
globalVariableDeclaration<br>
globalVariableDefinition<br>

functionDefinition ::= 'define' functionHeaderPreName
       returnTypeAttributes returnType functionName argumentList functionHeaderPostName functionCode<br>
functionDeclaration ::= // TODO<br>
<!-- functionHeaderPreName ::= [linkage] [PreemptionSpecifier] [visibility] [DLLStorageClass] [cconv] -->
returnTypeAttributes ::= returnTypeAttribute*<br>
returnTypeAttribute ::= <br>
argumentList ::= '(' [parameterDesc [',' parameterDesc]*] ')'<br>
parameterDesc ::= type parameterAttrs name<br>
parameterAttrs ::= []<br>
<!-- functionHeaderPostName ::= [(unnamed_addr|local_unnamed_addr)] [AddrSpace] [fn Attrs]
       [section "name"] [partition "name"] [comdat [($name)]] [align N]
       [gc] [prefix Constant] [prologue Constant] [personality Constant]
       (!name !N)* -->
// Function attributes<br>
fnAttrs ::= [fnAttr|attrID]* // These are likely wrong - I don't know exactly what can/can't be in an attribute group.<br>
fnAttr ::= fnAttrKeyword|fnAttrStrEquals<br>
fnAttrKeyword ::= 'mustprogress'|'noinline'|'norecurse'|'nounwind'|'optnone'|'uwtable'<br>
fnAttrStrEquals ::= framePointer|minLegalVectorWidth|noTrappingMath|stackProtectorBufferSize|targetCPU|targetFeatures|tuneCPU<br>
framePointer ::= '"' 'frame-pointer' '"' '=' '"' ('none' | 'non-leaf' | 'all') '"'<br>
minLegalVectorWidth ::= '"' 'min-legal-vector-width' '"' '=' '"' integer '"'<br>
noTrappingMath ::= '"' 'no-trapping-math' '"' '=' '"' ('true' | 'false') '"' // Not positive about accepted values for this<br>
stackProtectorBufferSize ::= '"' 'stack-protector-buffer-size' '"' '=' '"' integer '"' // Not sure about accepted values<br>
targetCPU ::= '"' 'target-cpu' '"' '=' '"' 'x86-64' '"' // Unsure about accepted values<br>
targetFeatures ::= '"' 'target-features' '"' '=' '"' '+cx8,+fxsr,+mmx,+sse,+sse2,+x87' '"' // Unsure about accepted values<br>
tuneCPU ::= '"' 'tune-cpu' '"' '=' '"' 'generic' '"' // Unsure about accepted values<br>

functionName ::= globalIdentifier // Should only be named global identifiers?<br>
functionCode ::= '{' block block* '}'<br>
block ::= [label] instructions* terminator<br>
label ::= identifier ':' // Unsure<br>

// Instructions<br>
instructions ::= yieldValInstruction | yieldVoidInstruction<br>
yieldValInstruction ::= localIdentifier '=' (alloca) // To be extended<br>
yieldVoidInstruction ::= store // To be extended<br>
terminator ::= ret<br>
ret ::= <br>
alloca ::= 'alloca' ['inalloca'] sizeType [',' <ty> <NumElements>] [',' 'align' alignment] [, addrspace(<num>)]<br>
store ::= 'store' ['volatile'] <ty> operand ',' ptrType pointerOperand [',' 'align' <alignment>]
       [, !nontemporal !<nontemp_node>][, !invariant.group !<empty_node>]<br>

// Metadata<br>
metadataDefinition ::= namedMetadataDefinition | unnamedMetadataDefinition<br>
namedMetadataDefinition ::= namedMetadata '=' '!{' unnamedMetadata [',' unnamedMetadata]* '}'<br>
namedMetadata ::= '!' ([-a-zA-Z$._] | escapeSequence)([-a-zA-Z$._0-9] | escapeSequence)* // Are escape sequences allowed for the first 'character'?<br>
unnamedMetadataDefinition ::= unnamedMetadata '=' metadataNode<br>
metadataNode ::= ['distinct'] '!{' nodeValue [',' nodeValue] '}'<br>
       // FIXME Should this differentiate between uniqued/non-uniqued metadata nodes?<br>
unnamedMetadata ::= '!' integer<br>
nodeValue ::= metadataValue | integerValue // What is a value? Just a value of any type?<br>
metadataValue ::= namedMetadata | unnamedMetadata<br>
integerValue ::= integerType integer // Should I be enforcing types in the grammar? Probably... i32 3.0 doesn't make much sense, except as an implicit cast. Using C++ as a reference, but C++ doesn't have similar syntax that I'm aware of.<br>
metadataString = '!"' identifierString '"'<br>

// Types<br>
type ::= void | valueType<br>
valueType ::= <br>
firstClassType ::= // Should be returnType, but w/o <br>
returnType ::= void | intType | // To be extended<br>
sizeType ::= intType | // To be extended<br>

functionType ::= returnType '(' parameterTypeList ')'<br>
parameterTypeList ::= parameterType parameterTypeListRest | ''<br>
parameterTypeList ::= ',' parameterType | ''<br>
parameterType ::= <br>

// Specific type keywords<br>
voidType ::= 'void'<br>
intType ::= 'i' integer<br>
ptrType ::= 'ptr'<br>
tokenType ::= 'token'<br>
metadataType ::= 'metadata'<br>

// FIXME Should I define "values", which are the accepted way to define a value of a given type?<br>
// E.g. "intValue ::= intType integer", "metadataValue ::= metadataType (namedMetadata | unnamedMetadata) | (namedMetadata | unnamedMetadata)"<br>
// Part of it's that I'm not sure what's valid for parameter definitions, function arguments, metadata nodes, etc. - are there<br>
//     differences? Are there certain types where the type can be inferred from the value?<br>
// Have to remember - "types" in this (defining the grammar) are for defining the syntax - what keywords are accepted where.<br>
//     I probably don't need to worry about type inference or such at this point.<br>

// Attribute Groups<br>
attributeGroup ::= 'attributes' attrID '=' '{' fnAttr* '}' // What are the actual limits on what an attribute group can have?<br>
attrID ::= '#' integer<br>

// Identifiers<br>
globalIdentifier ::= '@' identifier<br>
localIdentifier ::= '%' identifier<br>
identifier ::= ([-a-zA-Z$._][-a-zA-Z$._0-9]* | integer | identifierString )<br>
identifierString ::= '"' ([^"\] | escapeSequence)([^"\] | escapeSequence)* '"' // I believe this is correct? I believe escape sequences for local/global identifiers are only allowed in the string.<br>

// Literals<br>
string ::= '"' [^"]* '"'<br>
integer ::= [0-9][0-9]*<br>
boolean ::= 'true' | 'false'<br>

// Misc<br>
comment ::= ';' .* '\n'<br>
operand ::= integer | boolean | globalIdentifier | localIdentifier // To be extended?<br>
pointerOperand ::= globalIdentifier | localIdentifier // Can this also be an integer?<br>
escapeSequence ::= '\'[0-9][0-9]<br>