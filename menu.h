#ifndef MENU_MENU_H_
#define MENU_MENU_H_
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <utility>
using namespace std;
class Menu {
 public:
  Menu(const char *, const char *);
  int RequestOrDie();
 private:
  enum FileState {
    kGood = 0,
    kBad,
    kEndOf
  };
  enum FindState {
    kFatalError = 0,
    kFound,
    kParsedLegal
  };
  FileState IsBadOrEndOfFile();
  void BadFile();
  bool IsFatalFileError();
  char FileGet();
  bool IsIllegalTagOrAttributeStart(char);
  bool IsFatalTagOrAttributeStartError(char);
  pair<FindState, char> FindStartOrParseLegalOrDie(const char *);
  bool IsIllegalTagOrAttributeCharacter(char);
  void IllegalTagOrAttributeCharacter(char);
  bool IsFatalTagOrAttributeCharacterError(char);
  bool IsIllegalCharacter(char);
  bool IsFatalCharacterError(char);
  FindState FindMenuTagStartOrParseLegalOrDie();
  void FoundMenuTag();
  const string file_name;
  const string menu_name;
  ifstream menu_file;
  unsigned int line;
  unsigned int column;
};
#endif
