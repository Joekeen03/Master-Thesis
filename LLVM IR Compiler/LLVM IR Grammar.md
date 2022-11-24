file ::= source_file data_layout
source_file ::= 'source_filename' '=' string
data_layout ::= 'target' 'datalayout' '=' string
string ::= '"' [^"]* '"'