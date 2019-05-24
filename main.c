#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  if (strncmp(argv[1], "-test", 5)==0) {
    runtest();
    return 0;
  }

  // トークナイズしてパース
  user_input = argv[1];
  tokenize();
  Node *node = program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 抽象構文木を下りながらコード生成
  Codep mycode = firstcode;
  while (mycode->ty != CD_END) {
    gen(mycode->sent);
    printf("  pop rax\n");
    mycode = mycode->next;
  }

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}