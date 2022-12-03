file ::= source_file [data_layout] [target_triple]
source_file ::= 'source_filename' '=' string

// Layout
data_layout ::= 'target' 'datalayout' '=' string
alignment ::= integer // Must be a power of two in some, but not all cases.

target_triple ::= 'target' 'triple' '=' string
functionDefinition ::= 'define' functionHeaderPreName
       resultTypeAttrs functionName argumentList functionHeaderPostName { ... }
<!-- functionHeaderPreName ::= [linkage] [PreemptionSpecifier] [visibility] [DLLStorageClass] [cconv] -->
resultTypeAttrs ::= [ret attrs] resultType
argumentList ::= '(' [parameterDesc [',' parameterDesc]*] ')'
parameterDesc ::= type parameterAttrs name
parameterAttrs ::= []
<!-- functionHeaderPostName ::= [(unnamed_addr|local_unnamed_addr)] [AddrSpace] [fn Attrs]
       [section "name"] [partition "name"] [comdat [($name)]] [align N]
       [gc] [prefix Constant] [prologue Constant] [personality Constant]
       (!name !N)* -->
// Function attributes
// These are likely wrong - I don't know exactly what can/can't be in an attribute group.
fnAttrs ::= [fnAttr|attrID]*
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
alloca ::= 'alloca' ['inalloca'] sizeType [, <ty> <NumElements>] [, 'align' alignment] [, addrspace(<num>)]

// Types
resultType ::= intType | // To be extended
sizeType ::= intType | // To be extended
intType ::= 'i' integer

// Attribute Groups
attributeGroup ::= 'attributes' attrID '=' '{' fnAttr* '}' // What are the actual limits on what an attribute group can have?
attrID ::= '#' integer

// Identifiers
globalIdentifier ::= '@' identifier
localIdentifier ::= '%' identifier
identifier ::= ([-a-zA-Z$._][-a-zA-Z$._0-9]* | integer | string )

// Literals
string ::= '"' [^"]* '"'
integer ::= [0-9][0-9]*
boolean ::= 'true' | 'false'

// Misc
comment ::= ';' .* '\n'