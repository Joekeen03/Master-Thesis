file ::= source_file [data_layout] [target_triple]
source_file ::= 'source_filename' '=' string
data_layout ::= 'target' 'datalayout' '=' string
target_triple ::= 'target' 'triple' '=' string
functionDefinition ::= 'define' functionHeaderPreName
       resultType  '('[argumentList]')' functionHeaderPostName { ... }
<!-- functionHeaderPreName ::= [linkage] [PreemptionSpecifier] [visibility] [DLLStorageClass] [cconv] [ret attrs] -->
resultType ::= 'void' | 
argumentList ::= [parameterDesc argumentListContinued]
argumentListContinued ::= [',' parameterDesc]*
parameterDesc ::= type parameterAttrs name
parameterAttrs ::= []
<!-- functionHeaderPostName ::= [(unnamed_addr|local_unnamed_addr)] [AddrSpace] [fn Attrs]
       [section "name"] [partition "name"] [comdat [($name)]] [align N]
       [gc] [prefix Constant] [prologue Constant] [personality Constant]
       (!name !N)* -->
functionName ::= globalIdentifier
globalIdentifier
string ::= '"' [^"]* '"'