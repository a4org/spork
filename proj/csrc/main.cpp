#include "csrc.hpp"


int main(void) {

  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen("sample/sample.txt", "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  int ct = 0;
  while ((read = getline(&line, &len, fp)) != -1) {
    if (read > 100) {
      if (ct == 2) break;
      // state machine, read next page
      ct++;
      printf("Retrieved line of length %zu:\n", read);
      printf("%s", line);
      for (int i = 0; i < read; i++) {
	cout << line[i] << ", ";
      }
      cout << endl;
    }
    printf("%s", line);
    for (int i = 0; i < read; i++) {
      cout << line[i] << ", ";
    }
    cout << endl;
  }
  cout << ct << endl;

  fclose(fp);
  if (line)
    free(line);

  exit(EXIT_SUCCESS);
}
