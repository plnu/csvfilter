# csvfilter
csvfilter is designed to let you easily select rows and columns from a csv file. While less flexible than awk, it has a few advantages:
 - It assumes that the first row in the file contains headers, and allows you to refer to columns in the file by those headers
 - It understands the rules for quotes in csv files, corrrectly handling commas in values

## Examples
All the following examples use the following input file, which should be saved as ``input.csv``:
```
name,mark,grade
neil,80.5,B
fred,93.2,A
jane,97.4,A
lucy,78.4,C
```
### Selecting columns
If you only want to see particular columns then you can specify them on the command line:
```
$ csvfilter -c "name,grade" input.csv 
name,grade
neil,B
fred,A
jane,A
lucy,C
```
You can even change the order of columns, or repeat them if you want to:
```
$ csvfilter -c "mark,name,mark" input.csv 
mark,name,mark
80.5,neil,80.5
93.2,fred,93.2
97.4,jane,97.4
78.4,lucy,78.4
```
### Selecting rows

Rows from the input file can be selected via a simple expression language - if the expression returns true, then the row is returned.

You can filter by string fields:
```
$ csvfilter -f 'name == "fred"' input.csv 
name,mark,grade
fred,93.2,A
```
or numberic fields:
```
$ csvfilter -f 'mark > 90' input.csv 
name,mark,grade
fred,93.2,A
jane,97.4,A
```
and can combine expressions:
```
$ csvfilter -f 'mark > 90 && name != "fred"' input.csv 
name,mark,grade
jane,97.4,A
```
## Operators available in expressions
csvfilter supports the following operators. In every case the operator precedence is the same as the C programming language.

### Comparison operators ( <, <=, ==, !=, >=, > )
Comparison operators can be used to compare either strings or numbers.

### Mathematical operators (+, -, *, /)
-, * and / can be used to subract, multiply or divide numbers. + can be used to add numbers and for string concatenation.

### Logical operators (||, &&)
&& and || can be used on boolean values (i.e. the result of comparison operators).

## Building csvfilter
csvfilter builds or both Linux and Mac. To build it you will need:
 * cmake v3.1 or better
 * make (I use gnu make v3.81, but any recent make should work)
 * a recent c++ compiler (Apple LLVM version 7.0.0 works, as does gcc version 5.2.1)
 * libpopt, with headers (v1.16 works, though any version should be fine).

If available, the build will make use of:
 * tclsh, which was used to write some of the automated tests
 * doxygen, if you want class-level documentation
 * awk, which the performance tests use to benchmark against.

## License
csvfilter is licensed under the BSD license.