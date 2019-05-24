#include "9cc.h"

Tokenp firstp;
Tokenp mytoken;

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize() {
  char *p = user_input;

  int i = 0;
  firstp = (Tokenp)malloc(sizeof(Token));
  mytoken = firstp;
  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }
    
    if (strncmp(p, "==", 2)==0) {
      mytoken->ty = TK_EQ;
      mytoken->input = p;
      mytoken->next = (Tokenp)malloc(sizeof(Token));
      mytoken = mytoken->next;
      i++;
      p += 2;
      continue;
    }

    if (strncmp(p, "!=", 2)==0) {
      mytoken->ty = TK_NE;
      mytoken->input = p;      
      mytoken->next = (Tokenp)malloc(sizeof(Token));
      mytoken = mytoken->next;
      i++;
      p += 2;
      continue;
    }

    if (strncmp(p, "<=", 2)==0) {
      mytoken->ty = TK_LE;
      mytoken->input = p;      
      mytoken->next = (Tokenp)malloc(sizeof(Token));
      mytoken = mytoken->next;
      i++;
      p += 2;
      continue;
    }

    if (strncmp(p, ">=", 2)==0) {
      mytoken->ty = TK_GE;
      mytoken->input = p;
      mytoken->next = (Tokenp)malloc(sizeof(Token));
      mytoken = mytoken->next;
      i++;
      p += 2;
      continue;
    }    
    
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '>' || *p == '<') {
      mytoken->ty = *p;
      mytoken->input = p;     
      mytoken->next = (Tokenp)malloc(sizeof(Token));
      mytoken = mytoken->next;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      mytoken->ty = TK_NUM;
      mytoken->input = p;
      mytoken->val = strtol(p, &p, 10);
      mytoken->next = (Tokenp)malloc(sizeof(Token));
      mytoken = mytoken->next;
      i++;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  mytoken->ty = TK_EOF;
  mytoken->input = p;      
  mytoken->next = NULL;
  mytoken = firstp;
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

int consume(int ty) {
  if (mytoken->ty != ty) 
    return 0;
  mytoken = mytoken->next;
  return 1;
}

Node *term() {
  // 次のトークンが'('なら、"(" expr ")"のはず
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')'))
      error_at(mytoken->input,
               "開きカッコに対応する閉じカッコがありません");
    return node;
  }

  // そうでなければ数値のはず
  if (mytoken->ty == TK_NUM){
    int val = mytoken->val;
    mytoken = mytoken->next;
    return new_node_num(val);
  }

  error_at(mytoken->input,
           "数値でも開きカッコでもないトークンです");
}

Node *unary() {
  if (consume('+'))
    return term();
  if (consume('-'))
    return new_node('-', new_node_num(0), term());
  return term();
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume('*'))
      node = new_node('*', node, unary());
    else if (consume('/'))
      node = new_node('/', node, unary());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume('<'))
      node = new_node('<', node, add());
    else if (consume('>'))
      node = new_node('<', add(), node);
    else if (consume(TK_LE))
      node = new_node(TK_LE, node, add());
    else if (consume(TK_GE))
      node = new_node(TK_LE, add(), node);
    else
      return node;
  }
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume(TK_EQ)){
      node = new_node(TK_EQ, node, relational());
    }
    else if (consume(TK_NE))
      node = new_node(TK_NE, node, relational());
    else
      return node;
  }
}

Node *expr() {
  return equality();
}