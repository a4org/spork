// 000998/ 隆平高科/ 01/ 农业/ (A)/ 农、林、牧、渔业
// https://www.aconvert.com/tw/file.php

#include "csrczh.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <locale>
#include <codecvt>

using namespace::std;

void skip(char* line, int& start, ssize_t size) {
  // skip all the space, and return the new non-space start
  int ret = start;
  if (start >= size) return;
  while (line[start] == ' ' && start < size) {
    start++;
  }
}

string get(char* line, int& start, ssize_t size) {
  // read till meet space, return the string and update new start
  string ret = "";
  if (start >= size) return ret;

  while (line[start] != ' ' && start < size) {
    ret += line[start];
    start++;
  }

  return ret;
}

int main(void) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  string str;

  fp = fopen("sample/industry.txt", "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  unordered_map<string, pair<string, string>> zhbigcode = {};
  zhbigcode["农、林、牧、渔业"] = {"农、林、牧、渔业", "(A)"};
  zhbigcode["采矿业(B)"] = {"采矿业", "(B)"};
  zhbigcode["制造业(C)"] = {"制造业", "(C)"};
  zhbigcode["电力、热力、燃气"] = {"电力、热力、燃气 及水生产和供应业", "(D)"};
  zhbigcode["建筑业(E)"] = {"建筑业", "(E)"};
  zhbigcode["批发和零售业(F)"] = {"批发和零售业", "(F)"};
  zhbigcode["邮政业(G)"] = {"交通运输、仓储和邮政业", "(G)"};
  zhbigcode["交通运输、仓储和"] = {"交通运输、仓储和邮政业", "(G)"};
  zhbigcode["住宿和餐饮业(H)"] = {"住宿和餐饮业", "(H)"};
  zhbigcode["信息传输、软件"] = {"信息传输、软件和信息技术服务业", "(I)"};
  zhbigcode["金融业(J)"] = {"金融业", "(J)"};
  zhbigcode["房地产业(K)"] = {"房地产业", "(K)"};
  zhbigcode["租赁和商务服务业"] = {"租赁和商务服务业", "(L)"};
  zhbigcode["科学研究和技术服"] = {"科学研究和技术服务业", "(M)"};
  zhbigcode["水利、环境和公共"] = {"水利、环境和公共设施管理业", "(N)"};
  zhbigcode["居民服务、修理和"] = {"居民服务、修理和其他服务业", "(O)"};
  zhbigcode["教育(P)"] = {"教育", "(P)"};
  zhbigcode["卫生和社会工作"] = {"卫生和社会工作", "(Q)"};
  zhbigcode["文化、体育和娱乐"] = {"文化、体育和娱乐", "(R)"};
  zhbigcode["综合(S)"] = {"综合", "(S)"};

  unordered_map<string, string> zhclasscode = {};
  zhclasscode["皮革、毛皮、羽毛及其制"] = "皮革、毛皮、羽毛及其制 品和制鞋业";
  zhclasscode["木材加工和木、竹、藤、"] = "木材加工和木、竹、藤、 棕、草制品业";
  zhclasscode["文教、工美、体育和娱乐"] = "文教、工美、体育和娱乐用品制造业";
  zhclasscode["石油加工、炼焦和核燃"] = "石油加工、炼焦和核燃料加工业";
  zhclasscode["化学原料和化学制品制造"] = "化学原料和化学制品制造业";
  zhclasscode["黑色金属冶炼和压延加工"] = "黑色金属冶炼和压延加工业";
  zhclasscode["有色金属冶炼和压延加工"] = "有色金属冶炼和压延加工业";
  zhclasscode["铁路、船舶、航空航天和"] = "铁路、船舶、航空航天和其他运输设备制造业";
  zhclasscode["计算机、通信和其他电子"] = "计算机、通信和其他电子设备制造业";
  zhclasscode["电信、广播电视和卫星传"] = "电信、广播电视和卫星传输服务";
  zhclasscode["机动车、电子产品和日用"] = "机动车、电子产品和日用产品修理业";
  zhclasscode["广播、电视、电影和影视"] = "广播、电视、电影和影视录音制作业";
  zhclasscode["输服务"] = "电信、广播电视和卫星传输服务";

  int linen = 0;

  // global variables (for each output line)
  // e.g. 000998/ 隆平高科/ 01/ 农业/ (A)/ 农、林、牧、渔业 
  string code; // 000998
  string zhname; // 隆平高科
  string classcode; // 01
  string zhclass; // 农业
  string bigclass; // (A)
  string zhbigclass; // 农、林、牧、渔业 


  while ((read = getline(&line, &len, fp)) != -1) {
    linen += 1;
    // cout << read << endl;
    if (read <= 140 && linen > 3) {
      // valid line (not index & title)

      // 1. specify the type of this line
      int ct = 0;
      for (int i = 0; i < read; i++) {
        if (line[i] != ' ') {
          ct++;
          while (line[i] != ' ' && i < read) {
            i++;
          }
        }
      }
      // cout << ct << endl;

      // 2. according to the type, we choose different ways:
      //
      // 2: normal case (most common): 000780 ST平能 -> Add them directly
      // 3: special case (A) 002086  *ST东洋 -> ignore the first one
      // 4: normal case: 03 畜牧业 000735 罗牛山 -> change state and add them directly
      // 5: normal case: 农、林、牧、渔业  01  农业   000998    隆平高科 -> change state


      int first = 0, second = 0, third = 0, fourth = 0;
      switch (ct) {
        case 3:
          skip(line, first, read);
          get(line, first, read);
        case 2:
          skip(line, first, read);
          code = get(line, first, read);
          second = first;
          skip(line, second, read);
          zhname = get(line, second, read);
          zhname = zhname.substr(0, zhname.size()-2); // rm newline
          break;
        case 5:
          skip(line, first, read);
          zhbigclass = get(line, first, read);
          if (zhbigcode.find(zhbigclass) == zhbigcode.end()) {
            cout << zhbigclass << endl;
            return -1;
          }
          bigclass = zhbigcode[zhbigclass].second;
          zhbigclass = zhbigcode[zhbigclass].first;
        case 4:
          skip(line, first, read);
          classcode = get(line, first, read);
          second = first;
          skip(line, second, read);
          zhclass = get(line, second, read);

          if (zhclasscode.find(zhclass) != zhclasscode.end()) {
            zhclass = zhclasscode[zhclass];
          }

          third = second;
          skip(line, third, read);
          code = get(line, third, read);
          fourth = third;
          skip(line, third, read);
          zhname = get(line, third, read);
          zhname = zhname.substr(0, zhname.size()-2); // rm newline
          break;
      }


      cout << code << "," << zhname << "," << classcode << "," << zhclass
        << "," << bigclass << "," << zhbigclass << endl;
    }

  }

  fclose(fp);
  if (line)
    free(line);

  exit(EXIT_SUCCESS);
}
