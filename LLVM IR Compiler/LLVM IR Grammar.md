file ::= source_file [data_layout] [target_triple] ([globalValue] | [meta])
source_file ::= 'source_filename' '=' string

// Layout
data_layout ::= 'target' 'datalayout' '=' string
alignment ::= integer // Must be a power of two in some, but not all cases.

target_triple ::= 'target' 'triple' '=' string

globalValue ::= globalVariable | functionDefinition | functionDeclaration | metadataDefinition

// Global Variables
globalVariable ::= globalVariableDeclaration | globalVariableDefinition
globalVariableDeclaration
globalVariableDefinition

functionDefinition ::= 'define' functionHeaderPreName
       returnTypeAttributes returnType functionName argumentList functionHeaderPostName functionCode
functionDeclaration ::= // TODO
<!-- functionHeaderPreName ::= [linkage] [PreemptionSpecifier] [visibility] [DLLStorageClass] [cconv] -->
returnTypeAttributes ::= returnTypeAttribute*
returnTypeAttribute ::= 
argumentList ::= '(' [parameterDesc [',' parameterDesc]*] ')'
parameterDesc ::= type parameterAttrs name
parameterAttrs ::= []
<!-- functionHeaderPostName ::= [(unnamed_addr|local_unnamed_addr)] [AddrSpace] [fn Attrs]
       [section "name"] [partition "name"] [comdat [($name)]] [align N]
       [gc] [prefix Constant] [prologue Constant] [personality Constant]
       (!name !N)* -->
// Function attributes
fnAttrs ::= [fnAttr|attrID]* // These are likely wrong - I don't know exactly what can/can't be in an attribute group.
fnAttr ::= fnAttrKeyword|fnAttrStrEquals
fnAttrKeyword ::= 'mustprogress'|'noinline'|'norecurse'|'nounwind'|'optnone'|'uwtable'
fnAttrStrEquals ::= framePointer|minLegalVectorWidth|noTrappingMath|stackProtectorBufferSize|targetCPU|targetFeatures|tuneCPU
framePointer ::= '"' 'frame-pointer' '"' '=' '"' ('none' | 'non-leaf' | 'all') '"'
minLegalVectorWidth ::= '"' 'min-legal-vector-width' '"' '=' '"' integer '"'
noTrappingMath ::= '"' 'no-trapping-math' '"' '=' '"' ('true' | 'false') '"' // Not positive about accepted values for this
stackProtectorBufferSize ::= '"' 'stack-protector-buffer-size' '"' '=' '"' integer '"' // Not sure about accepted values
targetCPU ::= '"' 'target-cpu' '"' '=' '"' 'x86-64' '"' // Unsure about accepted values
targetFeatures ::= '"' 'target-features' '"' '=' '"' '+cx8,+fxsr,+mmx,+sse,+sse2,+x87' '"' // Unsure about accepted values
tuneCPU ::= '"' 'tune-cpu' '"' '=' '"' 'generic' '"' // Unsure about accepted values

functionName ::= globalIdentifier // Should only be named global identifiers?
functionCode ::= '{' block block* '}'
block ::= [label] instructions* terminator
label ::= identifier ':' // Unsure

// Instructions
instructions ::= yieldValInstruction | yieldVoidInstruction
yieldValInstruction ::= localIdentifier '=' (alloca) // To be extended
yieldVoidInstruction ::= store // To be extended
terminator ::= ret
ret ::= 
alloca ::= 'alloca' ['inalloca'] sizeType [',' <ty> <NumElements>] [',' 'align' alignment] [, addrspace(<num>)]
store ::= 'store' ['volatile'] <ty> operand ',' ptrType pointerOperand [',' 'align' <alignment>]
       [, !nontemporal !<nontemp_node>][, !invariant.group !<empty_node>]

// Metadata
metadataDefinition ::= namedMetadataDefinition | unnamedMetadataDefinition
namedMetadataDefinition ::= namedMetadata '=' '!{' unnamedMetadata [',' unnamedMetadata]* '}'
namedMetadata ::= '!' ([-a-zA-Z$._] | escapeSequence)([-a-zA-Z$._0-9] | escapeSequence)* // Are escape sequences allowed for the first 'character'?
unnamedMetadataDefinition ::= unnamedMetadata '=' metadataNode
metadataNode ::= ['distinct'] '!{' nodeValue [',' nodeValue] '}'
       // FIXME Should this differentiate between uniqued/non-uniqued metadata nodes?
unnamedMetadata ::= '!' integer
nodeValue ::= metadataValue | integerValue // What is a value? Just a value of any type?
metadataValue ::= namedMetadata | unnamedMetadata
integerValue ::= integerType integer // Should I be enforcing types on the 
metadataString = '!"' identifierString '"'

// Types
type ::= void | valueType
valueType ::= 
firstClassType ::= // Should be returnType, but w/o 
returnType ::= void | intType | // To be extended
sizeType ::= intType | // To be extended

functionType ::= returnType '(' parameterTypeList ')'
parameterTypeList ::= parameterType parameterTypeListRest | ''
parameterTypeList ::= ',' parameterType | ''
parameterType ::= 

// Specific type keywords
voidType ::= 'void'
intType ::= 'i' integer
ptrType ::= 'ptr'
tokenType ::= 'token'
metadataType ::= 'metadata'

// FIXME Should I define "values", which are the accepted way to define a value of a given type?
// E.g. "intValue ::= intType integer", "metadataValue ::= metadataType (namedMetadata | unnamedMetadata) | (namedMetadata | unnamedMetadata)"
// Part of it's that I'm not sure what's valid for parameter definitions, function arguments, metadata nodes, etc. - are there
//     differences? Are there certain types where the type can be inferred from the value?
// Have to remember - "types" in this (defining the grammar) are for defining the syntax - what keywords are accepted where.
//     I probably don't need to worry about type inference or such at this point.

// Attribute Groups
attributeGroup ::= 'attributes' attrID '=' '{' fnAttr* '}' // What are the actual limits on what an attribute group can have?
attrID ::= '#' integer

// Identifiers
globalIdentifier ::= '@' identifier
localIdentifier ::= '%' identifier
identifier ::= ([-a-zA-Z$._][-a-zA-Z$._0-9]* | integer | identifierString )
identifierString ::= '"' ([^"\] | escapeSequence)([^"\] | escapeSequence)* '"' // I believe this is correct? I believe escape sequences for local/global identifiers are only allowed in the string.

// Literals
string ::= '"' [^"]* '"'
integer ::= [0-9][0-9]*
boolean ::= 'true' | 'false'

// Misc
comment ::= ';' .* '\n'
operand ::= integer | boolean | globalIdentifier | localIdentifier // To be extended?
pointerOperand ::= globalIdentifier | localIdentifier // Can this also be an integer?
escapeSequence ::= '\'[0-9][0-9]