#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
  TK_EQ,
  TK_NE,
  TK_LE,
  TK_GE,
  TK_IDENT,
  ND_IDENT,
  CD_END,
  TK_RETURN,
  ND_RETURN,
  ND_NUM = 256,
};

// トークンの型
typedef struct token{
  int ty;      // トークンの型
  int val;     // tyがTK_NUMの場合、その数値
  char *input; // トークン文字列（エラーメッセージ用）
  struct token *next;
} Token, *Tokenp;

//可変長ベクタ
typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct Node {
  int ty;           // 演算子かND_NUM
  struct Node *lhs; // 左辺
  struct Node *rhs; // 右辺
  int val;          // tyがND_NUMの場合のみ使う
  char name;        // tyがND_IDENTの場合のみ使う(変数を表す)
} Node;

typedef struct Code {
  int ty;
  struct Node *sent;
  struct Code *next;
} Code, *Codep;
Codep firstcode;

// 入力プログラム
char *user_input;

int is_alnum(char c);
void vec_push(Vector *vec, void *elem);
void expect(int line, int expected, int actual); 
void runtest(); 
void error(char *fmt, ...);
void error_at(char *loc, char *msg);
void tokenize();
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
int consume(int ty);
Node *term();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *expr();
Node *program();
void gen(Node *node);