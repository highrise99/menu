#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "new_menu.h"
#define DEBUG
using namespace std;
Menu::Menu(const char *constructor_file_name, const char *constructor_menu_name)
    : file_name_(constructor_file_name),
      file_open_(true),
      menu_name_(constructor_menu_name),
      menu_open_(false),
      line_(1),
      column_(0) {}
pair<Menu::FindState, char> Menu::AnalyzeTag() {
  char buffer_character = FileGet();
  if (IsFatalFileError()) {
    return make_pair(kFatalError, NULL);
  }
  if (buffer_character == '<') {
    pair<FindState, string> tmp = GetLegalTagOrAttributeOrDie();
    switch (tmp.first) {
      case kFatalError: {
        return make_pair(kFatalError, NULL);
      }
      case kParsedLegal: {
//        if (tmp.second.empty()) {
//          return make_pair(kFatalError, buffer_character);
//        }
        string tag(tmp.second, 0, tmp.second.length() - 1);
//        buffer_character = tmp.second.back();
        if (tmp.second.empty()) { // TODO (Matthew) add an error message
#ifdef DEBUG
          cout << "tag buffer is empty! ";
          cin.ignore();
#endif
          return make_pair(kFatalError, NULL);
        }
        buffer_character = tmp.second.back();
#ifdef DEBUG
        cout << "the tag is \"" << tag << "\". ";
        cin.ignore();
#endif
        if (tag.front() == '/') {
          if (buffer_character != '>') {
            return make_pair(kFatalError, NULL);
          }
          string end_tag(tag, 1, tag.length());
#ifdef DEBUG
          cout << "front open tag is " << open_tags_.front() << ' ';
          cin.ignore();
#endif
          if (open_tags_.front() != end_tag) {
            return make_pair(kFatalError, NULL);
          }
          if (open_tags_.size() == 1) {
            file_open_ = false;
            return make_pair(kParsedLegal, buffer_character);
          }
          open_tags_.pop_front();
          if (end_tag == "menu") {
            menu_open_ = false;
          }
#ifdef DEBUG
          cout << "after popping the front tag is " << open_tags_.front() << ' ';
          cin.ignore();
#endif
          return make_pair(kParsedLegal, buffer_character);
        }
        if (tag == "menu") { // TODO (Matthew) implement this as a function here
                             // and if the tag is "option"
#ifdef DEBUG
          cout << "a menu tag is found--not necessarily of the specified name ";
          cin.ignore();
#endif
          if (find(open_tags_.begin(), open_tags_.end(), tag) !=
              open_tags_.end()) {
            return make_pair(kFatalError, NULL);
          }
          if (buffer_character != ' ') { // TODO (Matthew) add an error message
            return make_pair(kFatalError, NULL);
          }
          while (!IsBadOrEndOfFile()) {
            pair<FindState, string> *tmp = new pair<FindState, string>;
            *tmp = GetLegalTagOrAttributeOrDie();
            switch (tmp->first) {
              case kFatalError: {
                delete tmp;
                return make_pair(kFatalError, NULL);
              }
              case kParsedLegal: {
                string menu_attribute(tmp->second,
                          0,
                          tmp->second.length() - 1);
#ifdef DEBUG
                cout << "the menu attribute is \"" << menu_attribute << "\". ";
#endif
                if (menu_attribute == "id") {
#ifdef DEBUG
                  cout << "a menu identification tag is found here ";
                  cin.ignore();
#endif
                  if (tmp->second.empty()) { // TODO (Matthew) add an error
                                            // message
                    return make_pair(kFatalError, NULL);
                  }
                  buffer_character = tmp->second.back();
                  delete tmp;
                  if (buffer_character != '=') { // TODO (Matthew) add an error
                                                 // message
                    return make_pair(kFatalError, NULL);
                  }
                  pair<bool, char> tmp =
                  IsFatalApostropheOrQuotationMarkError();
                  if (tmp.first) {
                    return make_pair(kFatalError, NULL);
                  }
                  // TODO (Matthew) add the following as a function here and in
                  // GetLegalTagOrAttributeOrDie
                  buffer_character = FileGet();
                  if (IsFatalFileError()) {
                    return make_pair(kFatalError, NULL);
                  }
                  string menu_identification_attribute_value;
                  while (!IsFatalFileError() &&
                         !IsFatalCharacterError(buffer_character) &&
                         buffer_character != tmp.second) {
                    menu_identification_attribute_value += buffer_character;
                    buffer_character = FileGet();
                  }
                  if (IsBadOrEndOfFile() || buffer_character != tmp.second) {
                    return make_pair(kFatalError, NULL);
                  }
                  switch (ParseLegalAttributesUntilTagEndOrDie(FileGet())) {
                    case kFatalError: {
                      return make_pair(kFatalError, NULL);
                    }
                    case kParsedLegal: {
                      // TODO (Matthew) add the menu tag to the stack
                      if (menu_identification_attribute_value == menu_name_) {
#ifdef DEBUG
                      cout <<
                      "A menu of the name \"" <<
                      menu_name_ <<
                      "\" is found on line " <<
                      line_ <<
                      " and column " <<
                      column_ <<
                      " of the file \"" <<
                      file_name_ <<
                      "\". Press Enter to continue.";
                      cin.ignore();
#endif
                        open_tags_.push_front(tag);
                        menu_open_ = true;
                        return make_pair(kFound, buffer_character);
                      }
                      open_tags_.push_front(tag);
                      return make_pair(kParsedLegal, buffer_character);
                    }
                    default: { // TODO (Matthew) add an error message as a
                               // function
                      return make_pair(kFatalError, NULL);
                    }
                  }
                }
                if (tmp->second.empty()) { // TODO (Matthew) add an error
                                           // message
                  return make_pair(kFatalError, NULL);
                }
                buffer_character = tmp->second.back();
                delete tmp;
                pair<FindState, char> *tmp = new pair<FindState, char>;
                *tmp = ParseLegalAttributeEndOrDie(buffer_character);
                switch (tmp->first) {
                  case kFatalError: {
                    delete tmp;
                    return make_pair(kFatalError, NULL);
                  }
                  case kParsedLegal: {
                    buffer_character = tmp->second;
                    delete tmp;
                    break;
                  }
                  default: {
                    delete tmp;
                    return make_pair(kFatalError, NULL);
                  }
                }
                break;
              }
              default: { // TODO (Matthew) add an error message as a function
                delete tmp;
                return make_pair(kFatalError, NULL);
              }
            }
          }
          return make_pair(kFatalError, NULL);  // TODO (Matthew) add an error message
        }
        else if (tag == "description") {
          if (find(open_tags_.begin(), open_tags_.end(), "menu") ==
              open_tags_.end() ||
              find(open_tags_.begin(), open_tags_.end(), tag) !=
              open_tags_.end() ||
              find(open_tags_.begin(), open_tags_.end(), "option") !=
              open_tags_.end()) {
            return make_pair(kFatalError, NULL);
          }
          switch (ParseLegalAttributesUntilTagEndOrDie(buffer_character)) {
            case kFatalError: {
              return make_pair(kFatalError, NULL);
            }
            case kParsedLegal: {
              // TODO (Matthew) add the description tag to the stack
#ifdef DEBUG
              cout <<
              "A description tag is found at line " <<
              line_ <<
              " and column " <<
              column_ <<
              " of the file \"" <<
              file_name_ <<
              "\". Press Enter to continue.";
              cin.ignore();
#endif
              open_tags_.push_front(tag);
              return make_pair(kParsedLegal, buffer_character);
            }
            default: { // TODO (Matthew) add an error message as a function
              return make_pair(kFatalError, NULL);
            }
          }
        }
        else if (tag == "option") { // TODO (Matthew) implement this as a
                                    // function here and if the tag is "menu"
          if (find(open_tags_.begin(), open_tags_.end(), "menu") ==
              open_tags_.end() ||
              find(open_tags_.begin(), open_tags_.end(), tag) !=
              open_tags_.end() ||
              find(open_tags_.begin(), open_tags_.end(), "description") !=
              open_tags_.end()) {
#ifdef DEBUG
            cout << "something went wrong here ";
#endif
            cin.ignore();
            return make_pair(kFatalError, NULL);
          }
          if (buffer_character != ' ') { // TODO (Matthew) add an error message
#ifdef DEBUG
            cout << "not a space ";
            cin.ignore();
#endif
            return make_pair(kFatalError, NULL);
          }
          while (!IsBadOrEndOfFile()) {
            pair<FindState, string> *tmp = new pair<FindState, string>;
            *tmp = GetLegalTagOrAttributeOrDie();
            switch (tmp->first) {
              case kFatalError: {
                delete tmp;
                return make_pair(kFatalError, NULL);
              }
              case kParsedLegal: {
                string menu_attribute(tmp->second,
                          0,
                          tmp->second.length() - 1);
                if (menu_attribute == "id") {
#ifdef DEBUG
                  cout << "id found ";
                  cin.ignore();
#endif
                  if (tmp->second.empty()) { // TODO (Matthew) add an error
                                             // message
                    delete tmp;
                    return make_pair(kFatalError, NULL);
                  }
                  buffer_character = tmp->second.back();
                  delete tmp;
                  if (buffer_character != '=') { // TODO (Matthew) add an error
                                                 // message
#ifdef DEBUG
                    cout << "not an equals sign ";
                    cin.ignore();
#endif
                    return make_pair(kFatalError, NULL);
                  }
                  pair<bool, char> tmp =
                  IsFatalApostropheOrQuotationMarkError();
                  if (tmp.first) {
                    return make_pair(kFatalError, NULL);
                  }
                  // TODO (Matthew) add the following as a function here and in
                  // GetLegalTagOrAttributeOrDie
                  buffer_character = FileGet();
                  if (IsFatalFileError()) {
                    return make_pair(kFatalError, NULL);
                  }
                  string menu_identification_attribute_value;
                  while (!IsFatalFileError() &&
                         !IsIllegalCharacter(buffer_character) &&
                         buffer_character != tmp.second) {
                    menu_identification_attribute_value += buffer_character;
                    buffer_character = FileGet();
                  }
                  if (IsBadOrEndOfFile()) {
                    return make_pair(kFatalError, NULL);
                  }
                  if (IsIllegalCharacter(buffer_character) &&
                      buffer_character != tmp.second) {
                    return make_pair(kFatalError, NULL);
                  }
#ifdef DEBUG
                  cout << "the identifier is " << menu_identification_attribute_value << ". ";
                  cin.ignore();
#endif
                  switch (ParseLegalAttributesUntilTagEndOrDie(FileGet())) {
                    case kFatalError: {
                      return make_pair(kFatalError, NULL);
                    }
                    case kParsedLegal: {
                      int option_identification;
                      stringstream option_identification_attribute_value_stream(
                          menu_identification_attribute_value);
                      if (!(option_identification_attribute_value_stream >>
                            option_identification)) { // TODO
                                                                 // (Matthew)
                                                                 // add an error
                                                                 // message
                        return make_pair(kFatalError, NULL);
                      }
                      if (option_identification < 0) { // TODO (Matthew) add an
                                                        // error message
                        return make_pair(kFatalError, NULL);
                      }
                      if (OptionIdentifierUsed(option_identification)) {
                        return make_pair(kFatalError, NULL);
                      }
                      // TODO (Matthew) add the option tag to the stack and
                      // handle the id somehow
                      open_tags_.push_front(tag);
                      options_.push_back(make_pair(option_identification, ""));
                      return make_pair(kParsedLegal, buffer_character);
#ifdef DEBUG
                      cout <<
                      "An option tag is found at line " <<
                      line_ <<
                      " and column " <<
                      column_ <<
                      " of the file \"" <<
                      file_name_ <<
                      "\". Press Enter to continue.";
                      cin.ignore();
#endif
                    }
                    default: { // TODO (Matthew) add an error message as a
                               // function
                      return make_pair(kFatalError, NULL);
                    }
                  }
                }
                buffer_character = tmp->second.back();
                delete tmp;
                pair<FindState, char> *tmp = new pair<FindState, char>;
                *tmp = ParseLegalAttributeEndOrDie(buffer_character);
                switch (tmp->first) {
                  case kFatalError: {
                    delete tmp;
                    return make_pair(kFatalError, NULL);
                  }
                  case kParsedLegal: {
                    buffer_character = tmp->second;
                    delete tmp;
                    break;
                  }
                  default: {
                    delete tmp;
                    return make_pair(kFatalError, NULL);
                  }
                }
                break;
              }
              default: { // TODO (Matthew) add an error message as a function
                delete tmp;
                return make_pair(kFatalError, NULL);
              }
            }
          }
          return make_pair(kFatalError, NULL);  // TODO (Matthew) add an error message
        }
        // TODO (Matthew) parse legal tag end or die
        switch (ParseLegalAttributesUntilTagEndOrDie(buffer_character)) {
          case kFatalError: {
            return make_pair(kFatalError, NULL);
          }
          case kParsedLegal: {
            // TODO (Matthew) add the tag to the stack
            open_tags_.push_front(tag);
            return make_pair(kParsedLegal, buffer_character);
          }
          default: { // TODO (Matthew) add an error message as a function
            return make_pair(kFatalError, NULL);
          }
        }
        break;
      }
      default: {
        return make_pair(kFatalError, NULL);
      }
    }
  }
  if (buffer_character == '&') {
    return make_pair(kFatalError, NULL);
  }
  if (find(open_tags_.begin(), open_tags_.end(), "description") !=
      open_tags_.end()) {
    description_ += buffer_character;
  }
  if (find(open_tags_.begin(), open_tags_.end(), "option") !=
      open_tags_.end()) {
    // TODO (Matthew) add option logic
    if (options_.empty()) {
      return make_pair(kFatalError, NULL);
    }
    options_[options_.size() - 1].second += buffer_character;
  }
  return make_pair(kParsedLegal, buffer_character);
}
void Menu::BadFile() {
  cout <<
  "Error: the file \"" <<
  file_name_ <<
  "\" is bad at line " <<
  line_ <<
  " and column " <<
  column_ <<
  ". Press Enter to exit.";
  cin.ignore();
}
int Menu::column() {
  return column_;
}
char Menu::FileGet() {
  char c = menu_file_.get();
  if (c == '\n') {
    line_ += 1;
    column_ = 0;
  }
  else if (!IsBadOrEndOfFile()) {
    column_ += 1;
  }
  return c;
}
pair<Menu::FindState, string> Menu::GetLegalTagOrAttributeOrDie() {
  char tag_character = FileGet();
  string tag_buffer_string;
  if (IsFatalFileError()) {
    return make_pair(kFatalError, tag_buffer_string);
  }
  if (IsIllegalTagOrAttributeStart(tag_character) &&
      tag_character != '/') {
    tag_buffer_string = tag_character;
    return make_pair(kFatalError, tag_buffer_string);
  }
  do {
    tag_buffer_string += tag_character;
    tag_character = FileGet();
  }
  while (!IsFatalFileError() &&
         !IsIllegalTagOrAttributeCharacter(tag_character));
  if (IsBadOrEndOfFile()) {
    return make_pair(kFatalError, tag_buffer_string);
  }
#ifdef DEBUG
  cout << "the tag character is \'" << tag_character << "\' ";
#endif
  return make_pair(kParsedLegal, tag_buffer_string + tag_character);
}
int Menu::line() {
  return line_;
}
Menu::FileState Menu::IsBadOrEndOfFile() {
  if (!menu_file_) {
    return kBad;
  }
  if (menu_file_.eof()) {
    return kEndOf;
  }
  return kGood;
}
pair<bool, char> Menu::IsFatalApostropheOrQuotationMarkError() {
  char attribute_end_character = FileGet();
  if (IsFatalFileError()) {
    return make_pair(true, NULL);
  }
  if (attribute_end_character != '\'' && attribute_end_character != '\"') {
    cout <<
    "Error: \'" <<
    attribute_end_character <<
    "\' at line " <<
    line_ <<
    " and column " <<
    column_ <<
    " of the file \"" <<
    file_name_ <<
    "\" is not an apostrophe or a quotation mark. Press Enter to exit.";
    cin.ignore();
    return make_pair(true, NULL);
  }
  return make_pair(false, attribute_end_character);
}
bool Menu::IsFatalCharacterError(char c) {
  if (IsIllegalCharacter(c)) {
    cout <<
    "Error: the character \'" <<
    c <<
    "\' at line " <<
    line_ <<
    " and column " <<
    column_ <<
    " of the file \"" <<
    file_name_ <<
    "\" is an illegal character. Press Enter to exit.";
    cin.ignore();
    return true;
  }
  return false;
}
bool Menu::IsFatalFileError() {
  switch (IsBadOrEndOfFile()) {
    case kGood: {
      return false;
    }
    case kBad: {
      BadFile();
      return true;
    }
    case kEndOf: {
      cout <<
      "Error: the file \"" <<
      file_name_ <<
      "\" ends at line " <<
      line_ <<
      " and column " <<
      column_ <<
      ". Press Enter to exit.";
      cin.ignore();
      return true;
    }
    default: {
      return true;
    }
  }
}
bool Menu::IsFatalTagOrAttributeStartError(char first_start_character) {
  if (IsIllegalTagOrAttributeStart(first_start_character)) {
    cout <<
    "Error: the character \'" <<
    first_start_character <<
    "\' at line " <<
    line_ <<
    " and column " <<
    column_ <<
    " of the file \"" <<
    file_name_ <<
    "\" is an illegal start to a tag or attribute. Press Enter to exit.";
    cin.ignore();
    return true;
  }
  return false;
}
bool Menu::IsIllegalCharacter(char c) { // TODO (Matthew) Add support for entity
                                        // references.
  if (c == '<' || c == '&') {
    return true;
  }
  return false;
}
bool Menu::IsIllegalTagOrAttributeCharacter(char start_character) {
  if (!isalnum(start_character) && start_character != '_') {
    return true;
  }
  return false;
}
// TODO (Matthew) Add support for comments.
bool Menu::IsIllegalTagOrAttributeStart(char first_start_character) {
  if (!isalpha(first_start_character) && first_start_character != '_') {
    return true;
  }
  return false;
}
bool Menu::LoadOrDie() {
  menu_file_.open(file_name_.c_str());
  if (!menu_file_) {
    BadFile();
    return 0;
  }
#ifdef DEBUG
  cout << "analyzing tags... ";
  cin.ignore();
#endif
  pair<FindState, char> tmp = AnalyzeTag();
  switch (tmp.first) {
    case kFatalError: {
      return false;
    }
    case kFound: {
      menu_open_ = true;
    }
    case kParsedLegal: {
      break;
    }
    default: { // TODO (Matthew) add an error message as a function
      return false;
    }
  }
  while (file_open_) { // TODO (Matthew) add an expression
    tmp = AnalyzeTag();
    switch (tmp.first) {
      case kFatalError: {
        return false;
      }
      case kFound: {
        menu_open_ = true;
      }
      case kParsedLegal: {
        break;
      }
      default: { // TODO (Matthew) add an error message as a function
        return false;
      }
    }
  }
#ifdef DEBUG
  cout <<
  description_ <<
  endl;
  int identifier = 1;
  for (vector<pair<int, string>>::iterator options_iterator = options_.begin();
       options_iterator < options_.end(); ++options_iterator) {
    cout <<
    identifier <<
    ".\t" <<
    options_iterator->second <<
    endl;
    ++identifier;
  }
  cout <<
  "Input: ";
  cin.ignore();
#endif
  return true;
}
bool Menu::OptionIdentifierUsed(const int option_identifier) {
  for (vector<pair<int, string>>::iterator options_iterator = options_.begin();
       options_iterator < options_.end(); ++options_iterator) {
    if (options_iterator->first == option_identifier) {
      cout <<
      "Error on line " <<
      line_ <<
      " and column " <<
      column_ <<
      " of the file " <<
      file_name_ <<
      " is already used. Press Enter to exit.";
      cin.ignore();
      return true;
    }
  }
  return false;
}
pair<Menu::FindState, char> Menu::ParseLegalAttributeEndOrDie(
    char attribute_end_character) {
  if (IsFatalFileError()) {
    return make_pair(kFatalError, attribute_end_character);
  }
  if (IsIllegalTagOrAttributeCharacter(attribute_end_character)
      && attribute_end_character != '=') {
    // TODO (Matthew) add an error message
    return make_pair(kFatalError, attribute_end_character);
  }
  /* Attribute values must open either with an apostrophe or a
   * quotation mark.
   */
  pair<bool, char> tmp = IsFatalApostropheOrQuotationMarkError();
  if (tmp.first) {
    return make_pair(kFatalError, tmp.second);
  }
  char attribute_value_start = tmp.second;
  do {
    attribute_end_character = FileGet();
  }
  while (!IsFatalFileError() &&
         !IsFatalCharacterError(attribute_end_character) &&
         attribute_end_character != attribute_value_start);
  if (IsBadOrEndOfFile()) {
    return make_pair(kFatalError, attribute_end_character);
  }
  if (IsIllegalCharacter(attribute_end_character)) {
    return make_pair(kFatalError, NULL);
  }
  return make_pair(kParsedLegal, FileGet());
}
Menu::FindState Menu::ParseLegalAttributesUntilTagEndOrDie(char c) {
  while (!IsBadOrEndOfFile() && c == ' ') {
    /* Attributes must start with a letter or an underscore.
     */
//    char c = FileGet();
//    if (IsFatalFileError()) {
//      return make_pair(kFatalError, NULL);
//    }
//    if (IsFatalTagOrAttributeStartError(c)) {
//      return make_pair(kFatalError, NULL);
//    }
//    /* Attributes must be formed only of letters, numbers, and
//     * underscores. They must be followed immediately by an equals
//     * sign.
//     */
//    do {
//      c = FileGet();
//    }
//    while (!IsBadOrEndOfFile() &&
//           !IsIllegalTagOrAttributeCharacter(c));
//    pair<FindState, char> tmp = ParseLegalAttributeEndOrDie(c);
//    switch (tmp.first) {
//      case kFatalError: {
//        return make_pair(kFatalError, NULL);
//      }
//      case kParsedLegal: {
//        return make_pair(kParsedLegal, tmp.second);
//      }
//      default: {
//        return make_pair(kFatalError, NULL);
//      }
//    }
    pair<FindState, char> tmp = ParseLegalAttributeOrDie();
    switch (tmp.first) {
      case kFatalError: {
        return kFatalError;
      }
      case kParsedLegal: {
        c = tmp.second;
        break;
      }
      default: {
        return kFatalError;
      }
    }
  }
  if (c != '>') {
#ifdef DEBUG
    cout << "for some reason, the tag character is not what it is ";
    cin.ignore();
#endif
    return kFatalError;
  }
  return kParsedLegal;
}
pair<Menu::FindState, char> Menu::ParseLegalAttributeOrDie() {
  char c = FileGet();
  if (IsFatalFileError()) {
    return make_pair(kFatalError, NULL);
  }
  if (IsFatalTagOrAttributeStartError(c)) {
    return make_pair(kFatalError, NULL);
  }
  /* Attributes must be formed only of letters, numbers, and
   * underscores. They must be followed immediately by an equals
   * sign.
   */
  do {
    c = FileGet();
  }
  while (!IsBadOrEndOfFile() &&
         !IsIllegalTagOrAttributeCharacter(c));
  pair<FindState, char> tmp = ParseLegalAttributeEndOrDie(c);
  switch (tmp.first) {
    case kFatalError: {
      return make_pair(kFatalError, NULL);
    }
    case kParsedLegal: {
      return make_pair(kParsedLegal, tmp.second);
    }
    default: { // TODO (Matthew) add an error message as a function
      return make_pair(kFatalError, NULL);
    }
  }
}
