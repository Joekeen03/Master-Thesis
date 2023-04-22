Integer types: padded to ceil(bitWidth/8) bytes; padding is at the upper end of the integer (most significant bits)

* Example: i4  -> ppppbbbb, where p=padding bit, b=integer bit.
* i4 10 -> 00001010