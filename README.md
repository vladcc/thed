# thed
A terminal hex editor

thed is a terminal hex editor which enables you to examine and modify the binary contents of a file through the command line.

The default behavior of thed is to dump the whole file to stdout in the classic hex view format.
You can specify the offset from which thed should start, and how many lines should thed show before, after, or before and after the address. To save the dump to a file, use redirection.
thed can create comma seperated value dumps of files, which is very handy for embedding binaries in your projects.

thed can look for and optionally replace ASCII, Unicode, and byte strings. The ASCII table, number conversion, and bitwise operations are 
also included.

Use: 
thed <file> [<options> ...]

For a full list of features:
thed -?

Known issues:
Sometimes it may displace the prompt in the Linux terminal.
