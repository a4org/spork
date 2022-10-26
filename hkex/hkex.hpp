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

/* Marcos */
#define START "ASK/    HIGH/"
#define END "-------------------------------------------------------------------------------"
#define ll long long
#define SUSPENDED "TRADING SUSPENDED"
#define HALTED "TRADING HALTED"
#define DW 1
#define CBBC 2

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

  bool valid; // whether we need to insert it into db
  int type;   // DW(1) or CBBC(2)

  /* DWs 1*/
  string issuer; // e.g, HS
  string asset;  // e.g, PINAN
  string e;        // e.g, European
  string cp;     // e.g, C
  int expyear;   // e.g, 20
  int expmonth;  // e.g, 8
  string serial;   // e.g, F
  bool rmb;      // e.g, * -> true
};

void error(char* fmt, ...);
