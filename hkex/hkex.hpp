#include <sqlite3.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <string>
#include <iostream>

#define START "ASK/    HIGH/"
#define END "-------------------------------------------------------------------------------"
#define ll long long
#define SUSPENDED "TRADING SUSPENDED"
#define HALTED "TRADING HALTED"

using namespace::std;

/* struct */
struct quotation {
  string id;
  ll date;
  ll code;
  string name;
  string currency;

  double pc;
  double closing;
  double bid;
  double ask;
  double high;
  double low;

  ll turnover;
  ll shares;

  bool valid;
};

void error(char* fmt, ...);
