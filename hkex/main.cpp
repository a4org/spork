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

int gendate(int month) {
  switch(month) {
    case 1: case 3: case 5: case 7:
    case 8: case 10: case 12:
      return 31;
    case 2:
      return 28;
    default:
      return 30;
  }
}

/* call parse fn over and over again */
void iterateYear(string year, int start, int end, string dbname) {
  string date = "";
  string filename = "";
  if (start > end) return;
  for (int i = start; i <= end; i++) {
    int ndate = gendate(i);
    string smonth = to_string(i);
    if (smonth.size() == 1) {
      smonth = '0' + smonth;
    }

    for (int j = 1; j <= ndate; j++) {
      string sdate = to_string(j);
      if (sdate.size() == 1) {
	sdate = '0' + sdate;
      }
      filename = FILES + year + smonth + sdate + SUFFIX;
      date = "20" + year + smonth + sdate;
      parse(filename.c_str(), dbname, date);
    }

  }

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
  // printf("sqlite opened database successfully\n");

  /*
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
  */

  string ssqldw = "CREATE TABLE DWS ("  \
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
  "SHARESTRADED        INT," \
  "ISSUER              TEXT   NOT NULL," \
  "ASSET               TEXT   NOT NULL," \
  "E                   TEXT   NOT NULL," \
  "CP                  TEXT   NOT NULL," \
  "EXPYEAR             INT    NOT NULL," \
  "EXPMONTH            INT    NOT NULL," \
  "SERIAL              TEXT   NOT NULL);"; \

  const char* sqldw = ssqldw.c_str();

  ret = sqlite3_prepare_v2(db, sqldw, -1, &stmt, NULL);

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

  string ssqlcbbc = "CREATE TABLE CBBCS ("  \
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
  "SHARESTRADED        INT," \
  "ISSUER              TEXT   NOT NULL," \
  "ASSET               TEXT   NOT NULL," \
  "RESIDUAL            INT    NOT NULL," \
  "Bear Bull           TEXT   NOT NULL," \
  "EXPYEAR             INT    NOT NULL," \
  "EXPMONTH            INT    NOT NULL," \
  "SERIAL              TEXT   NOT NULL);"; \

  const char* sqlcbbc = ssqlcbbc.c_str();

  ret = sqlite3_prepare_v2(db, sqlcbbc, -1, &stmt, NULL);

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

  // printf("sqlite created table successfully\n");
  *rc = ret;
  sqlite3_close(db);
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

  qs.type = 0;

  qs.valid = false;
}

string quotes(string value) {
  return "'" + value + "',";
}

/* Perform inserting accorting to the current qs */
void sqlinsert(sqlite3* db, string dbname) {
  int ret; ret = sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
  if (ret) {
    error("sqlite cannot open database: %s", dbname.c_str());
  }
  if (!qs.valid) return;

  char sql[1000];
  if (qs.type == DW) {
    snprintf(sql, sizeof(sql), "INSERT INTO DWS VALUES ('%s',%lld,%lld,'%s','%s',%f,%f,%f,%f,%f,%f,%lld,%lld,'%s','%s','%s','%s',%d,%d,'%s');"
			      ,qs.id.c_str(), qs.date,
			       qs.code, qs.name.c_str(), qs.currency.c_str(), qs.pc, 
			       qs.closing, qs.bid, qs.ask,
			       qs.high, qs.low, qs.turnover, qs.shares,
			       qs.issuer.c_str(), qs.asset.c_str(), qs.e.c_str(), 
			       qs.cp.c_str(), qs.expyear, qs.expmonth, qs.serial.c_str());
  } else if (qs.type == CBBC) {
    snprintf(sql, sizeof(sql), "INSERT INTO CBBCS VALUES ('%s',%lld,%lld,'%s','%s',%f,%f,%f,%f,%f,%f,%lld,%lld,'%s','%s',%d,'%s',%d,%d,'%s');"
			      ,qs.id.c_str(), qs.date,
			       qs.code, qs.name.c_str(), qs.currency.c_str(), qs.pc, 
			       qs.closing, qs.bid, qs.ask,
			       qs.high, qs.low, qs.turnover, qs.shares,
			       qs.issuer.c_str(), qs.asset.c_str(), qs.residual, 
			       qs.bb.c_str(), qs.expyear, qs.expmonth, qs.serial.c_str());
  }

  const char* csql = sql;

  // printf("%s\n", csql);

  sqlite3_stmt* stmt;
  int retcode = sqlite3_prepare(db, csql, -1, &stmt, NULL);

  if (retcode != SQLITE_OK) {
    printf("%s\n", sqlite3_errmsg(db));
    error("sqlite syntax error, on inserting, error code: %d.", retcode);
  }

  while ((retcode = sqlite3_step(stmt)) == SQLITE_ROW) {
    continue;
  }

  if (retcode != SQLITE_DONE) {
    printf("%s\n", sqlite3_errmsg(db));
    error("sqlite execution error, on inserting, error code: %d.", retcode);
  }

  // printf("sqlite finish inserting\n");
  sqlite3_close(db);
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
  int i = 0;
  while (sint[i] == '0' && i < sint.size()) {
    i++;
  }

  if (i == sint.size()) {
    // all zero
    return 0;
  }

  return stoi(rmdot(sint.substr(i, sint.size()-i)));
}

void parsedwnd(string secondp, string& e, string& cp,
               int& expyear, int& expmonth, string& serial) {
  if (secondp[0] == 'L') {
    /* Inline Warrants (ignore now) */
    qs.valid = false;
    qs.type = 0;
    return;
  }

  /* 1. parse E */
  switch(secondp[0]) {
  case ' ':
    e = "American";
    break;
  case 'E':
    e = "European";
    break;
  case 'R':
    e = "Regional";
    break;
  case 'X':
    e = "Exotic";
    break;
  default:
    error("Invalid E: %c.", secondp[0]);
  }

#ifdef DEBUG
  printf("DW E: %s", e.c_str());
#endif

  /* 2. parse Call/Put */
  switch(secondp[1]) {
  case ' ':
    cp = "Non C/P";
    break;
  case 'C': case 'P':
    cp = secondp[1];
    break;
  default:
    error("Invalid C/P: %c.", secondp[1]);
  }

#ifdef DEBUG
  printf("DW C/P: %s", cp.c_str());
#endif

  /* 3. parse expriation year and month */
  expyear = parseint(secondp.substr(2, 2));

#ifdef DEBUG
  printf("DW EXP YEAR: %d", expyear);
#endif

  expmonth = parseint(secondp.substr(4, 2));

#ifdef DEBUG
  printf("DW EXP MONTH: %d", expmonth);
#endif

  /* 4. parse serial number */
  serial = secondp[secondp.size()-1];
#ifdef DEBUG
  printf("DW SERIAL: %s", serial.c_str());
#endif
}

void parsedwst(string firstp, string& issuer, string& asset) {
  issuer = firstp.substr(0, 2);

#ifdef DEBUG
  printf("DW ISSUER: %s", issuer.c_str());
#endif
  asset = "";
  for (int i = 2; i < firstp.size(); i++) {
    if (firstp[i] == ' ' || firstp[i] == '-') continue; // skip
    asset += firstp[i];
  }

#ifdef DEBUG
  printf("DW ASSET: %s", asset.c_str());
#endif
}

string rmspace(string s) {
  int i = 0, j = s.size()-1;
  while (s[i] == ' ' && i < s.size()) {
    i++;
  }
  int p = 0;
  while (s[j] == ' ' && j >= 0) {
    p++;
    j--;
  }
  if (i >= j) return "";
  return s.substr(i, s.size()-i-p);
}

/* Parse the name of CBBCs
 * it has 2 formats:
 * 1. ZZ#QQQQQNCYYMMA
 * 2. ZZ#QQQQNCYYMMA (rmb)
 * */
void parsecbbc(string name) {
  qs.type = CBBC;
  string issuer;
  string asset;
  int residual;
  string bb; // bull / bear
  int expyear;
  int expmonth;
  string serial;
  if (name.size() == 15) {
    qs.rmb = false;
    issuer = name.substr(0, 2);
    asset = rmspace(name.substr(3, 5));
    residual = (name[8] == 'N') ? 0 : 1;
    bb = (name[9] == 'C') ? "Bull" : "Bear";
    expyear = parseint(name.substr(10, 2));
    expmonth = parseint(name.substr(12, 2));
    serial = name[name.size()-1];
  } else if (name.size() == 14) {
    /* rmb */
    qs.rmb = true;
    issuer = name.substr(0, 2);
    asset = rmspace(name.substr(3, 4));
    residual = (name[7] == 'N') ? 0 : 1;
    bb = (name[8] == 'C') ? "Bull" : "Bear";
    expyear = parseint(name.substr(9, 2));
    expmonth = parseint(name.substr(11, 2));
    serial = name[name.size()-1];
  } else {
    error("Unsupported CBBC name: %s.", name.c_str());
  }

  /* set the value */
  qs.issuer = issuer;
  qs.asset = asset;
  qs.residual = residual;
  qs.bb = bb;
  qs.expyear = expyear;
  qs.expmonth = expmonth;
  qs.serial = serial;
}

/* Parse the name of DWs 
 * basically, it has 3 formats:
 * 1. ZZQQQQQ@ECYYMMA
 * 2. ZZ-QQQQ@ECYYMMA
 * 3. ZZQQQQ@ECYYMMA* (rmb)
 * */
void parsedw(string name) {
  qs.type = DW;
  if (name.size() != 15) { return; }
  string issuer;
  string asset;
  string e;
  string cp;
  int expyear;
  int expmonth;
  string serial;
  if (name.find('@') == 6) {
    qs.rmb = true;
    /* parse the first part */
    parsedwst(name.substr(0, 6), issuer, asset);
    parsedwnd(name.substr(7, 7), e, cp, expyear, expmonth, serial);
  } else if (name.find('@') == 7) {
    qs.rmb = false;
    /* parse the first part */
    parsedwst(name.substr(0, 7), issuer, asset);
    parsedwnd(name.substr(8, 7), e, cp, expyear, expmonth, serial);
  } else {
    error("Unsupported dw format: %s.", name.c_str());
  }

  /* set the value */
  qs.issuer = issuer;
  qs.asset = asset;
  qs.e = e;
  qs.cp = cp;
  qs.expyear = expyear;
  qs.expmonth = expmonth;
  qs.serial = serial;
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
	ret = ret.substr(0, ret.size()-1); // remove the last space
	break;
      }
      onespace = true;
      ret += curr;
    } else {
      if (curr == '&') index+=4; // &amp;
      if (curr == '\'') curr = ' ';
      onespace = false;
      ret += curr;
    }
    index++;
  }
  return ret;
}


string getname(string& strline, int& index) {
  string ret = "";
  skipspace(strline, index);
  if (index + 2 < strline.size() && strline[index+2] == '#') {
    /* is a cbbc */
    if (index + 14 < strline.size() && strline[index+14] == '*') {
      /* rmb */
      ret = strline.substr(index, 14);
      index += 15;
      return ret;
    } else {
      ret = strline.substr(index, 15);
      index += 15;
      return ret;
    }
  } else {
    return getnext(strline, index);
  }
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
#ifdef DEBUG
  cout << "raw CODE: " << property << endl;
#endif
  string id = getid(dates, property);
  qs.id = id;

  /* 2. DATE: INT 20200204 */
  ll date = parselong(dates);
  qs.date = date;

  /* 3. CODE: INT 1 */
  ll code = parselong(property);
  qs.code = code;

  /* 4. NAME: TEXT CKH HOLDINGS 
   * This is special in CBBC: which has two columns */
  // string name = getnext(strline, index);
  string name = getname(strline, index);
#ifdef DEBUG
  cout << "raw NAME: " << name << endl;
#endif
  qs.name = name;

  /* Extra Step, perform name extracting and only set it valid
   * (means insert it into db)
   * Indicators:
   * DW: @
   * CBBC: # */
  if (name.find('#') != string::npos) {
    /* CBBC */
    parsecbbc(name);
  } else if (name.find('@') != string::npos) {
    /* DWs */
    parsedw(name);
  }

  /* 5. CURRENCY TEXT HKD */
  string currency = getnext(strline, index);
#ifdef DEBUG
  cout << "raw CURRENCY: " << currency << endl;
#endif
  qs.currency = currency;

  /* 6. PRV CLO REAL 68.45 */
  property = getnext(strline, index);
#ifdef DEBUG
  cout << "raw PRV CLO: " << property << endl;
#endif
  double pc = parsedouble(property);
  qs.pc = pc;

  /* 7. ASK REAL 68.60 */
  property = getnext(strline, index);
#ifdef DEBUG
  cout << "raw ASK: " << property << endl;
#endif
  double ask = parsedouble(property);
  qs.ask = ask;

  /* 8. HIGH REAL 69.15 */
  property = getnext(strline, index);
#ifdef DEBUG
  cout << "raw HIGH: " << property << endl;
#endif
  double high = parsedouble(property);
  qs.high = high;

  /* 9. SHARES TRADED INT(ll) 5403104 */
  property = getnext(strline, index);
#ifdef DEBUG
  cout << "raw SHARES: " << property << endl;
#endif
  double shares = parselong(property);
  qs.shares = shares; 

  char* remain; size_t len;
  /* Get the next line */
  getline(line, &len, *fp);

  if (qs.type != DW && qs.type != CBBC) return;

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
#ifdef DEBUG
  cout << "raw CLOSING: " << property << endl;
#endif
  double closing = parsedouble(property);
  qs.closing = closing;

  /* 11. BID REAL 68.50 */
  property = getnext(strremain, index);
#ifdef DEBUG
  cout << "raw BID: " << property << endl;
#endif
  double bid = parsedouble(property);
  qs.bid = bid;

  /* 12. LOW REAL 68.50 */
  property = getnext(strremain, index);
#ifdef DEBUG
  cout << "raw LOW: " << property << endl;
#endif
  double low = parsedouble(property);
  qs.low = low;

  /* 13. TURNOVER INT(ll) 371147633 */
  property = getnext(strremain, index);
#ifdef DEBUG
  cout << "raw TURNOVER: " << property << endl;
#endif
  double turnover = parselong(property);
  qs.turnover = turnover;

  qs.valid = true; // parsing over, no error report
}

/* Only deal with valid file 
 * without creating any intermediate representation (file)
 * just scan the file line by line
 * */ 
void parse(const char* filename, string dbname, string date) {
  FILE* fp;
  char* line = NULL;
  size_t len = 0;
  ssize_t readl;
  string retbuf = "";
  sqlite3* db;

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
	parseline(strline, &line, date, &fp);
#ifdef DEBUG
	pprintq(); // just for debugging
#endif
	sqlinsert(db, dbname);
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

  string dirname = argv[1];
  string dbname = argv[2];
  
  /* creating the db, as well as the table */
  sqlite3* db;
  char* errmsg = 0;
  int rc;

  create(dbname.c_str(), db, &rc, errmsg);

  /* do not loop the dir, just directly use the dates generated */
  iterateYear("19", 5, 12, dbname);
  iterateYear("20", 1, 12, dbname);
  iterateYear("21", 1, 12, dbname);
  iterateYear("22", 1, 8, dbname);

  // parse("./sample/good.html", "sample.db");
  // parse("./sample/empty.html", "sample.db");
}
