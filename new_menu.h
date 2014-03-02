#ifndef MENU_MENU_H_
#define MENU_MENU_H_
#include <fstream>
#include <string>
#include <utility>
using namespace std;
class Menu {
 public:
  Menu(const char *, const char *);
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
  FindState AnalyzeTag();
  void BadFile();
  char FileGet();
  pair<FindState, string> GetLegalTagOrAttributeOrDie();
  FileState IsBadOrEndOfFile();
  pair<bool, char> IsFatalApostropheOrQuotationMarkError();
  bool IsFatalFileError();
  bool IsFatalTagOrAttributeStartError(char);
  bool IsIllegalCharacter(char);
  bool IsIllegalTagOrAttributeCharacter(char);
  pair<FindState, char> ParseLegalAttributeEndOrDie(char);
  pair<FindState, char> ParseLegalAttributeOrDie();
  FindState ParseLegalAttributesUntilTagEndOrDie(char);
  const string file_name_;
  const string menu_name_;
  int line_;
  int column_;
  ifstream menu_file_;
};
#endif
