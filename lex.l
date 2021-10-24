%{
#include <stdio.h>
#include <string.h>
#include "parse.tab.h"
#include "node.h"

const char *g_srcfile;
int g_col = 1;

int create_token(int tag, const char *lexeme);
void yyerror(const char *fmt, ...);
%}

%option noyywrap
%option yylineno

%%

[ \t]+                   { g_col += strlen(yytext); }
[\n]                     { g_col = 1; }

"--".*                   { /* ignore comment */ }

"PROGRAM"                { return create_token(TOK_PROGRAM, yytext); }
"BEGIN"                  { return create_token(TOK_BEGIN, yytext); }
"END"                    { return create_token(TOK_END, yytext); }
"CONST"                  { return create_token(TOK_CONST, yytext); }
"TYPE"                   { return create_token(TOK_TYPE, yytext); }
"VAR"                    { return create_token(TOK_VAR, yytext); }
"ARRAY"                  { return create_token(TOK_ARRAY, yytext); }
"OF"                     { return create_token(TOK_OF, yytext); }
"RECORD"                 { return create_token(TOK_RECORD, yytext); }
"DIV"                    { return create_token(TOK_DIV, yytext); }
"MOD"                    { return create_token(TOK_MOD, yytext); }
"IF"                     { return create_token(TOK_IF, yytext); }
"THEN"                   { return create_token(TOK_THEN, yytext); }
"ELSE"                   { return create_token(TOK_ELSE, yytext); }
"REPEAT"                 { return create_token(TOK_REPEAT, yytext); }
"WHILE"                  { return create_token(TOK_WHILE, yytext); }
"DO"                     { return create_token(TOK_DO, yytext); }
"READ"                   { return create_token(TOK_READ, yytext); }
"WRITE"                  { return create_token(TOK_WRITE, yytext); }

[A-Za-z_][A-Za-z_0-9]*   { return create_token(TOK_IDENT, yytext); }

[0-9]+                   { return create_token(TOK_INT_LITERAL, yytext); }

":="                     { return create_token(TOK_ASSIGN, yytext); }
";"                      { return create_token(TOK_SEMICOLON, yytext); }
":"                      { return create_token(TOK_COLON, yytext); }
"="                      { return create_token(TOK_EQUALS, yytext); }
","                      { return create_token(TOK_COMMA, yytext); }
"."                      { return create_token(TOK_DOT, yytext); }
"+"                      { return create_token(TOK_PLUS, yytext); }
"-"                      { return create_token(TOK_MINUS, yytext); }
"*"                      { return create_token(TOK_TIMES, yytext); }
"<="                     { return create_token(TOK_LTE, yytext); }
"<"                      { return create_token(TOK_LT, yytext); }
">="                     { return create_token(TOK_GTE, yytext); }
">"                      { return create_token(TOK_GT, yytext); }
"="                      { return create_token(TOK_EQUALS, yytext); }
"#"                      { return create_token(TOK_HASH, yytext); }
"["                      { return create_token(TOK_LBRACKET, yytext); }
"]"                      { return create_token(TOK_RBRACKET, yytext); }
"("                      { return create_token(TOK_LPAREN, yytext); }
")"                      { return create_token(TOK_RPAREN, yytext); }

.                        { yyerror("Illegal character '%c' in input", yytext[0]); }

%%

void lexer_set_source_file(const char *filename) {
  g_srcfile = filename;
}

int create_token(int tag, const char *lexeme) {
  struct Node *tok = node_alloc_str_copy(tag, lexeme);
  struct SourceInfo info = {
    .filename = g_srcfile,
    .line = yylineno,
    .col = g_col,
  };
  node_set_source_info(tok, info);
  yylval.node = tok;
  g_col += strlen(yytext);
  return tag;
}
