#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#define INVALID 0 // Invalid, initial value
#define NORMAL  2 // Two parts, code and the name
#define SPECIAL 3 // With type ('(A)')
#define CLASS   5 // Process this line and next line together

using namespace::std;


struct eachmsg {
  bool valid;
  string type;   // 01
  string typezh; // 农、林、牧、渔业
  string typeen; // A
  string code;   // 002041
  string namezh; // 登海种业
};

// State Machine

class StateMachine {
  public:
    StateMachine(string filename) : filename(filename) {
      this->msg = (eachmsg *) malloc(sizeof(eachmsg));
      this->msg->valid = false;
    };

    void ParseFile();

  protected:
    vector<string*> parseline(char* line, int size);
    // this function will return the line message
    
    void printline(); // wrapper function

    int consume(char* line, int start, int size);
    // consume a valid char paragraph (when meet ' ' stop)
    // return the end of that valid cp
    // remember using skip before call consume

    int skip(char* line, int start, int size);
    // skip all ' 's.
    
    string* concatenate(char* line, int size, int start, int end);


  private:
    string filename;
    eachmsg* msg;
};
