# Var

Var is a simple progrming language that I am creating for fun

## How to use it

The Var compiler/interpreter is used in command line with this command:
```bash
Var <SETUP_FILE>
```

## Setup File

The setup file is composed like this :

```Var
{<source_file_name.V> :: <compiled/interpreted>}
{<entry_point> :: {<arguments_separated_by_colon>}}
{options [
    {<option1>},
    {<option2>},
    {<...>},
]}
{modules [
    {<graphics>},
]}
{<output_file_name>}
```
in this file you will need to replace any thing inside of <> with the necessary strings

## languages specifications

there is many instruction that you can use in this language

### variables

in Var there is two types of variables : simple variable and dictionary

the name of a variable need to begin with either "var_" for a simple variable or "dict_" for a dictionary

to create a variable you need to use
```Var
~create var_variableName = value,
~create dict_dictionaryName = {index1; value1#index2; value2},
```
