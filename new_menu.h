#ifndef MENU_MENU_H_
#define MENU_MENU_H_
#include <deque>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;
class Menu {
 public:
  Menu(const char *, const char *);
  int column();
  int line();
  bool LoadOrDie();
  int Request();
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
  pair<FindState, char> AnalyzeTag();
  void BadFile();
  char FileGet();
  pair<FindState, string> GetLegalTagOrAttributeOrDie();
  FileState IsBadOrEndOfFile();
  pair<bool, char> IsFatalApostropheOrQuotationMarkError();
  bool IsFatalCharacterError(char);
  bool IsFatalFileError();
  bool IsFatalTagOrAttributeStartError(char);
  bool IsIllegalCharacter(char);
  bool IsIllegalTagOrAttributeCharacter(char);
  bool IsIllegalTagOrAttributeStart(char);
  bool OptionIdentifierUsed(const int);
  pair<FindState, char> ParseLegalAttributeEndOrDie(char);
  pair<FindState, char> ParseLegalAttributeOrDie();
  FindState ParseLegalAttributesUntilTagEndOrDie(char);
  int column_;
  string description_;
  const string file_name_;
  bool file_open_;
  int line_;
  ifstream menu_file_;
  const string menu_name_;
  bool menu_open_;
  deque<string> open_tags_;
  vector<pair<int, string>>options_;
};
#endif
