#include "csrc.hpp"

string* StateMachine::concatenate(char* line, int size, int start, int end) {
  string tmp = "";
  if (end > size) end = size;
  for (int i = 0; i < end; i++) {
    tmp += line[i];
  }
  return new string(tmp);
};

int StateMachine::consume(char* line, int start, int size) {
  int ret = start;
  for (int i = start; i < size; i++) {
    if (line[i] == ' ') {
      ret = i;
      break;
    }
  }
  return ret;
};

int StateMachine::skip(char* line, int start, int size) {
  int ret = start;
  for (int i = start; i < size; i++) {
    if (line[i] != ' ') {
      ret = i;
      break;
    }
  }
  return ret;
}

// Parse a whole txt file, and print the (TODO: to a file)
void StateMachine::ParseFile() {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(this->filename.c_str(), "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  int ct = 0;
  while ((read = getline(&line, &len, fp)) != -1) {
    if (read > 100) {
      ct = 0;
      this->msg->valid = false; // update a new attr
      continue;
    }
    ct++;
    if (!this->msg->valid) {
      // start state, read next two lines


    } else {
      // just read next line, must be a (code, namezh) format

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
};

vector<string*> StateMachine::parseline(char* line, int size) {
  // return the line message
  int start = 0;
  int tmpend = start;
  vector<string*> ret = {};

  while (start < size-1) {
    start = skip(line, start, size);
    tmpend = consume(line, start, size);
    ret.push_back(concatenate(line, size, start, tmpend));
  }
};
