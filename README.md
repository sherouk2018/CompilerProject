# CompilerProject

Part 1 
Description:

    Your task in this phase of the assignment is to design and implement a lexical
    analyzer generator tool.
    The lexical analyzer generator is required to automatically construct a lexical analyzer from
    a regular expression description of a set of tokens. The tool is required to construct a
    nondeterministic finite automata (NFA) for the given regular expressions, combine these
    NFAs together with a new starting state, convert the resulting NFA to a DFA, minimize it
    and emit the transition table for the reduced DFA together with a lexical analyzer program
    that simulates the resulting DFA machine.
    The generated lexical analyzer has to read its input one character at a time, until it finds
    the longest prefix of the input, which matches one of the given regular expressions. It
    should create a symbol table and insert each identifier in the table. If more than one
    regular expression matches some longest prefix of the input, the lexical analyzer should
    break the tie in favor of the regular expression listed first in the regular specifications. If a
    match exists, the lexical analyzer should produce the token class and the attribute value.
    If none of the regular expressions matches any input prefix, an error recovery routine is
    to be called to print an error message and to continue looking for tokens.
    The lexical analyzer generator is required to be tested using the given lexical rules of
    tokens of a small subset of Java. Use the given simple program to test the generated lexical
    analyzer.
    Keep in mind that the generated lexical analyzer will integrate with a generated parser
    which you should implement in phase 2 of the assignment such that the lexical analyzer is
    to be called by the parser to find the next token.


Lexical Rules
    • The tokens of the given language are: identifiers, numbers, keywords, operators and
    punctuation symbols.
    • The token id matches a letter followed by zero or more letters or digits.
    • The token num matches an unsigned integer or a floating- point number. The number
    consists of one or more decimal digits, an optional decimal point followed by one or
    more digits and an optional exponent consisting of an E followed by one or more digits.
    • Keywords are reserved. The given keywords are: int, float, boolean, if, else, while.
    • Operators are: +, -, *, /, =, <=, <, >, >=, !=, ==
    • Punctuation symbols are parentheses, curly brackets, commas and semicolons.
    • Blanks between tokens are optional.
Lexical Rules Input File Format
    • Lexical rules input file is a text file.
    • Regular definitions are lines in the form LHS = RHS
    • Regular expressions are lines in the form LHS: RHS
    • Keywords are enclosed by { } in separate lines.
    • Punctuations are enclosed by [ ] in separate lines
    • \L represents Lambda symbol.
    • The following symbols are used in regular definitions and regular expressions with the
    meaning discussed in class: - | + * ( )
    • Any reserved symbol needed to be used within the language, is preceded by an
    escape backslash character.
    
Input file example for the above lexical rules:
    letter = a-z | A-Z
    digit = 0 - 9
    id: letter (letter|digit)*
    digits = digit+
    {boolean int float}
    num: digit+ | digit+ . digits ( \L | E digits)
    relop: \=\= | !\= | > | >\= | < | <\=
    assign: =
    { if else while }
    Page 2/4
    [; , \( \) { }]
    addop: \+ | -
    mulop: \* | /
Lexical Analyzer Output File Format
    Your program should output the transition table of the generated lexical analyzer in a
    format of your choice as well as recognized token classes one per line (like the following
    format).
    Output file example for the following test program:
        int
        id
        ,
        id
        ,i
        d
        ,
        id
        ;
        while
        (
        id
        relop
        num
        )…
        .to be continued
Test Program
      int sum , count , pass ,
      mnt; while (pass != 10)
      {
      pass = pass + 1 ;
      }
      
      
      
      
Part two:
Parser Generator

Objective
Phase 2: Parser Generator
    This phase of the assignment aims to practice techniques for building automatic parser
    generator tools.
Description
    Your task in this phase of the assignment is to design and implement an LL (1) parser
    generator tool.
    The parser generator expects an LL (1) grammar as input. It should compute First and Follow
    sets and uses them to construct a predictive parsing table for the grammar.
    The table is to be used to drive a predictive top-down parser. If the input grammar is not
    LL (1), an appropriate error message should be produced.
    The generated parser is required to produce some representation of the leftmost derivation for
    a correct input. If an error is encountered, a panic-mode error recovery routine is to be called
    to print an error message and to resume parsing.
    The parser generator is required to be tested using the given context free grammar of a small
    subset of Java. Of course, you have to modify the grammar to allow predictive parsing.
    Combine the lexical analyzer generated in phase1 and parser such that the lexical analyzer is
    to be called by the parser to find the next token. Use the simple program given in phase 1 to
    test the combined lexical analyzer and parser.
Bonus Task
    Automatically eliminating grammar left recursion and performing left factoring before
    generating the parser will be considered a bonus work.

Java CFG
    METHOD_BODY:: = STATEMENT_LIST
    STATEMENT_LIST::= STATEMENT | STATEMENT_LIST STATEMENT
    STATMENT ::= DECLARATION
    | IF
    | WHILE
    | ASSIGNMENT
    DECLARATION ::= PRIMITIVE_TYPE IDENTIFIER;
    PRIMITIVE_TYPE ::= int | float
    IF ::= if ( EXPRESSION ) { STATEMENT} else { STATEMENT }
    WHILE ::= while ( EXPRESSION ) { STATEMENT }
    ASSIGNMENT ::= IDENTIFIER = EXPRESSION;
    EXPRESSION ::= NUMBER
    | EXPRESSION INFIX_OPERATOR EXPRESSION
    | IDENTIFIER
    | ( EXPRESSION )
    INFIX_OPERATOR ::= + | - | * | / | %| < | > | <= | >= | == | !=| | | &&
CFG Input File Format
     CFG input file is a text file.
     Production rules are lines in the form LHS ::= RHS
     Production rule can be expanded over many lines.
     Terminal symbols are enclosed in single quotes.
     \L represents Lambda symbol.
     The symbol | is used in RHS of production rules with the meaning discussed in class.
     Any reserved symbol needed to be used within the language, is preceded by an escape
    backslash character.
Input file example:
    # METHOD_BODY = STATEMENT_LIST
    # STATEMENT_LIST = STATEMENT | STATEMENT_LIST STATEMENT
    # STATEMENT = DECLARATION
    | IF
    | WHILE
    | ASSIGNMENT
    # DECLARATION = PRIMITIVE_TYPE ‘id’ ‘;’
    # PRIMITIVE_TYPE = ‘int’ | ‘float’
    # IF = ‘if’ ‘(‘ EXPRESSION ‘)’ ‘{‘ STATEMENT ’}’ ‘else’ ‘{’ STATEMENT ‘}’
    # WHILE = ‘while’ ‘(‘ EXPRESSION ‘)’ ‘{‘ STATEMENT ‘}’
    # ASSIGNMENT = ‘id’ ‘=’ EXPRESSION ‘;’
    # EXPRESSION = SIMPLE_EXPRESSION
    | SIMPLE_EXPRESSION ‘relop’ SIMPLE_EXPRESSION
    # SIMPLE_EXPRESSION = TERM | SIGN TERM | SIMPLE_EXPRESSION ‘addop’ TERM
    # TERM = FACTOR | TERM ‘mulop’ FACTOR
    # FACTOR = ‘id’ | ‘num’ | ‘(‘ EXPRESSION ‘)’
    # SIGN = ‘+’ | ‘-‘
Parser Output File Format
    Your program should output the predictive parsing table of the generated parser in a format of
    your choice as well as the leftmost derivation sententials one per line (like the following
    format).
    Output file example for the given test program:
        int x;
        x = 5;
        if (x > 2)
        {
        x = 0;
        }
METHOD_BODY
    STATEMENT_LIST
    STATEMENT_LIST STATEMENT
    STATEMENT_LIST STATEMENT STATEMENT
    STATEMENT STATEMENT STATEMENT
    DECLARATION STATEMENT STATEMENT
    PRIMITIVE_TYPE IDENTIFIER; STATEMENT STATEMENT
    int IDENTIFIER; STATEMENT STATEMENT
    ….to be continued
