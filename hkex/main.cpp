#include "hkex.hpp"

static struct quotation qs = {}; // global var qs

/* Some helper functions */
void error(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void pprintq() {
  printf("%s, %lld, %lld, %s, %s, %f, %f, %f, %f, %f, %f, %lld, %lld \n",
         qs.id.c_str(), qs.date, qs.code, qs.name.c_str(), qs.currency.c_str(),
	 qs.pc, qs.closing, qs.bid, qs.ask, qs.high, qs.low, qs.turnover, qs.shares);
}

/* Set up the database
 * creating the db, as well as the table */ 
void create(const char* dbname, sqlite3* db, int* rc, char* errmsg) {
  sqlite3_stmt *stmt;
  int ret; ret = sqlite3_open(dbname, &db);
  if (ret) {
    error("sqlite cannot open database: %s", dbname);
  }
  printf("sqlite opened database successfully\n");

  string ssql = "CREATE TABLE QUOTATIONS("  \
  "ID      CHAR(14)    PRIMARY KEY     NOT NULL," \
  "DATE                INT     NOT NULL," \
  "CODE                INT     NOT NULL," \
  "NAME                TEXT    NOT NULL," \
  "CURRENCY            TEXT    NOT NULL," \
  "PRV CLO             REAL," \
  "CLOSING             REAL," \
  "BID                 REAL," \
  "ASK                 REAL," \
  "HIGH                REAL," \
  "LOW                 REAL," \
  "TURNOVER            INT," \
  "SHARE STRADED       INT);";

  const char* sql = ssql.c_str();

  ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  if (ret != SQLITE_OK) {
    printf("%s", sqlite3_errmsg(db));
    error("sqlite syntax error, on creating table, error code: %d.", ret);
  }

  while ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
    continue;
  }

  if (ret != SQLITE_DONE) {
    error("sqlite execution error, on creating table, error code: %d.", ret);
  }

  printf("sqlite created table successfully\n");
  *rc = ret;
}

void cleanqs() {
  qs.id = "";
  qs.date = -1;
  qs.code = -1;
  qs.name = "";
  qs.currency = "";

  qs.pc = -1;
  qs.closing = -1;
  qs.bid = -1;
  qs.ask = -1;
  qs.high = -1;
  qs.low = -1;
  qs.turnover = -1;
  qs.shares = -1;

  qs.valid = false;
}

void skipspace(string& strline, int& index) {
  while (strline[index] == ' ' && index < strline.size()) {
    index++;
  }
}

string getid(string date, string code) {
  for (int i = code.size(); i <= 6; i++) {
    date += '0';
  }
  date += code;
  return date;
}

string rmdot(string s) {
  string tmp = "";
  for (char c : s) {
    if (c != ',') tmp += c;
  }
  return tmp;
}

double parsedouble(string sdb) {
  if (!isdigit(sdb[0])){
    return -1;
  }
  return stod(rmdot(sdb));
}

ll parselong(string sll) {
  if (!isdigit(sll[0])) {
    return -1;
  }
  return stoll(rmdot(sll));
}

int parseint(string sint) {
  if (!isdigit(sint[0])) {
    return -1;
  }
  return stoi(rmdot(sint));
}

string getcode(string& strline, int& index) {
  skipspace(strline, index);
  string ret = "";
  while (index < strline.size()) {
    char curr = strline[index];
    if (curr == ' ') break;
    index++;
    ret += curr;
  }
  return ret;
}

string getnext(string& strline, int& index) {
  string ret = "";
  bool onespace = false;
  skipspace(strline, index);
  while (index < strline.size()) {
    char curr = strline[index];
    if (curr == ' ') {
      if (onespace) {
	break;
      }
      onespace = true;
      ret += curr;
    } else {
      onespace = false;
      ret += curr;
    }
    index++;
  }
  return ret;
}

/* Parsing the given line, and check whether we need the 2nd line
 * and fill the parsing output into qs
 * */
void parseline(string& strline, char** line, string dates, FILE** fp) {
  int index = 0;
  string property = "";
  string tmp = "";

  if (strline[0] != ' ') {
    if (strline[0] == '*') {
      index = 1;
    } else if (strline[0] == '<') {
      index = 33;
    } else {
      printf("Unformated %d at the begin", strline[0]);
    }
  }
  
  /* 1. ID: CHAR<14> 20200204000001*/
  property = getcode(strline, index);
  cout << "raw CODE: " << property << endl;
  string id = getid(dates, property);
  qs.id = id;

  /* 2. DATE: INT 20200204 */
  ll date = parselong(dates);
  qs.date = date;

  /* 3. CODE: INT 1 */
  ll code = parselong(property);
  qs.code = code;

  /* 4. NAME: TEXT CKH HOLDINGS */
  string name = getnext(strline, index);
  cout << "raw NAME: " << name << endl;
  qs.name = name;

  /* 5. CURRENCY TEXT HKD */
  string currency = getnext(strline, index);
  cout << "raw CURRENCY: " << property << endl;
  qs.currency = currency;

  /* 6. PRV CLO REAL 68.45 */
  property = getnext(strline, index);
  cout << "raw PRV CLO: " << property << endl;
  double pc = parsedouble(property);
  qs.pc = pc;

  /* 7. ASK REAL 68.60 */
  property = getnext(strline, index);
  cout << "raw ASK: " << property << endl;
  double ask = parsedouble(property);
  qs.ask = ask;

  /* 8. HIGH REAL 69.15 */
  property = getnext(strline, index);
  cout << "raw HIGH: " << property << endl;
  double high = parsedouble(property);
  qs.high = high;

  /* 9. SHARES TRADED INT(ll) 5403104 */
  property = getnext(strline, index);
  cout << "raw SHARES: " << property << endl;
  double shares = parselong(property);
  qs.shares = shares; 

  char* remain; size_t len;
  /* Get the next line */
  getline(line, &len, *fp);
  string strremain(*line);

  index = 0;
  if (strremain[0] != ' ') {
    if (strremain[0] == '*') {
      index = 1;
    } else if (strremain[0] == '<') {
      index = 33;
    } else {
      printf("Unformated %d at the begin", strremain[0]);
    }
  }

  /* 10. CLOSING REAL 68.50 */
  property = getnext(strremain, index);
  cout << "raw CLOSING: " << property << endl;
  double closing = parsedouble(property);
  qs.closing = closing;

  /* 11. BID REAL 68.50 */
  property = getnext(strremain, index);
  cout << "raw BID: " << property << endl;
  double bid = parsedouble(property);
  qs.bid = bid;

  /* 12. LOW REAL 68.50 */
  property = getnext(strremain, index);
  cout << "raw LOW: " << property << endl;
  double low = parsedouble(property);
  qs.low = low;

  /* 13. TURNOVER INT(ll) 371147633 */
  property = getnext(strremain, index);
  cout << "raw TURNOVER: " << property << endl;
  double turnover = parselong(property);
  qs.turnover = turnover;
}

/* Only deal with valid file 
 * without creating any intermediate representation (file)
 * just scan the file line by line
 * */ 
void parse(const char* filename, sqlite3* db) {
  FILE* fp;
  char* line = NULL;
  size_t len = 0;
  ssize_t readl;
  string retbuf = "";

  bool instatus = false;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    error("error while opening the file: %s.", filename);
  }

  while ((readl = getline(&line, &len, fp)) != -1) {
    /* main loop */
    string strline(line);
    if (instatus) {
      if (strline.find(END) != string::npos) {
	/* reach the end */
	instatus = false;
	break;
      }
      /* otherwise: parse this line */
      if (strline.find(SUSPENDED) == string::npos && strline.find(HALTED) == string::npos) {
	cleanqs();
	parseline(strline, &line, "20200204", &fp);
	pprintq();
      }
    } else if (strline.find(START) != string::npos) {
      /* we've found the start symbol, set the state */
      instatus = true;
      getline(&line, &len, fp); getline(&line, &len, fp);
    } /* else, we wait till the instatus come, or finish reading */
  }
}

int main(int argc, char** argv) {
  if (argc != 3) {
    error("Usage: ./main <DIR> <DB>");
  }
  
  /* creating the db, as well as the table */
  sqlite3* db;
  char* errmsg = 0;
  int rc;

  // create("sample.db", db, &rc, errmsg);

  /* do not loop the dir, just directly use the dates generated */
  parse("./sample/good.html", db);
}
