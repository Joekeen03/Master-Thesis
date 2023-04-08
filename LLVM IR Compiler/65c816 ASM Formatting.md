Includes:

	* TBD
newline

Macros:

	* TBD
newline

Global variables:

	* TBD

newline

SNES init:

	* Comment header
	* TBD
newline

General functions:

	* Comment header
	* newline
	* List of functions (for-each):
		> Possible intro comment
		> Function label
		> *Increase indent
		> List of code blocks (for-each):
			§ Optional description comment
			§ If label:
				□ label
				□ *Increase indent
				□ List of segments (for-each):
					® If "chunk":
						◊ If not first, newline
						◊ Chunk description comment
						◊ Lines of code:
							* Instruction; possible trailing comment
					® If single comment:
						◊ If not first, newline
						◊ Comment
					® If single instruction:
						◊ If not first AND prior segment was not a single instruction, newline
						◊ Instruction; possible trailing comment
				□ *Decrease indent
				□ Comment with label
			§ Else:
				□ List of segments: (see prior if-clause)
			§ newline
		> *Decrease indent
		> Comment with function name
		> \n
		

