#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "new_menu.h"
using namespace std;
Menu::Menu(const char *constructor_file_name, const char *constructor_menu_name)
    : file_name_(constructor_file_name),
      menu_name_(constructor_menu_name),
      line_(1),
      column_(0) {}
Menu::FindState Menu::AnalyzeTag() {
  char buffer_character = FileGet();
  if (IsFatalFileError()) {
    return kFatalError;
  }
  if (buffer_character == '<') {
    pair<FindState, string> tmp = GetLegalTagOrAttributeOrDie();
    switch (tmp.first) {
      case kFatalError: {
        return kFatalError;
      }
      case kParsedLegal: {
//        if (tmp.second.empty()) {
//          return make_pair(kFatalError, buffer_character);
//        }
        string tag(tmp.second, 0, tmp.second.length() - 1);
//        buffer_character = tmp.second.back();
        if (tmp.second.empty()) { // TODO (Matthew) add an error message
          return kFatalError;
        }
        buffer_character = tmp.second.back();
        if (tag == "menu") { // TODO (Matthew) implement this as a function here
                             // and if the tag is "option"
          if (buffer_character != ' ') { // TODO (Matthew) add an error message
            return kFatalError;
          }
          while (!IsBadOrEndOfFile()) {
            tmp = GetLegalTagOrAttributeOrDie();
            switch (tmp.first) {
              case kFatalError: {
                return kFatalError;
              }
              case kParsedLegal: {
                string menu_attribute(tmp.second,
                          tmp.second.begin(),
                          tmp.second.length() - 1);
                if (menu_attribute == "id") {
                  if (tmp.second.empty()) { // TODO (Matthew) add an error
                                            // message
                    return kFatalError;
                  }
                  buffer_character = tmp.second.back();
                  if (buffer_character != '=') { // TODO (Matthew) add an error
                                                 // message
                    return kFatalError;
                  }
                  pair<bool, char> tmp =
                  IsFatalApostropheOrQuotationMarkError();
                  if (tmp.first) {
                    return kFatalError;
                  }
                  // TODO (Matthew) add the following as a function here and in
                  // GetLegalTagOrAttributeOrDie
                  buffer_character = FileGet();
                  if (IsFatalFileError()) {
                    return kFatalError;
                  }
                  string menu_identification_attribute_value;
                  while (!IsFatalFileError() &&
                         !IsIllegalCharacter(buffer_character)) {
                    menu_identification_attribute_value += buffer_character;
                    buffer_character = FileGet();
                  }
                  if (IsBadOrEndOfFile()) {
                    return kFatalError;
                  }
                  if (IsIllegalCharacter(buffer_character) &&
                      buffer_character != tmp.second) {
                    return kFatalError;
                  }
                  switch (ParseLegalAttributesUntilEndOrDie(FileGet())) {
                    case kFatalError: {
                      return kFatalError;
                    }
                    case kParsedLegal: {
                      // TODO (Matthew) add the menu tag to the stack
                      if (menu_identification_attribute_value == menu_name) {
#ifdef DEBUG
                      cout <<
                      "A menu of the name \"" <<
                      menu_name <<
                      "\" is found on line " <<
                      line <<
                      " and column " <<
                      column <<
                      " of the file \"" <<
                      file_name <<
                      "\". Press Enter to continue.";
                      cin.ignore();
#endif
                        return kFound;
                      }
                      return kParsedLegal;
                    }
                    default: { // TODO (Matthew) add an error message as a
                               // function
                      return kFatalError;
                    }
                  }
                }
                buffer_character = FileGet();
                if (IsFatalFileError()) {
                  return kFatalError;
                }
                if (buffer_character != ' ') { // TODO (Matthew) add an error
                                               // message
                  return kFatalError;
                }
              }
              default: { // TODO (Matthew) add an error message as a function
                return kFatalError;
              }
            }
          }
          return kFatalError;  // TODO (Matthew) add an error message
        }
        else if (tag == "description") {
          switch (ParseLegalAttributesUntilEndOrDie(buffer_character)) {
            case kFatalError: {
              return kFatalError;
            }
            case kParsedLegal: {
              // TODO (Matthew) add the description tag to the stack
#ifdef DEBUG
              cout <<
              "A description tag is found at line " <<
              line <<
              " and column " <<
              column <<
              " of the file \"" <<
              file_name <<
              "\". Press Enter to continue.";
              cin.ignore();
#endif
              return kParsedLegal;
            }
            default: { // TODO (Matthew) add an error message as a function
              return kFatalError;
            }
          }
        }
        else if (tag == "option") { // TODO (Matthew) implement this as a
                                    // function here and if the tag is "menu"
          if (buffer_character != ' ') { // TODO (Matthew) add an error message
            return kFatalError;
          }
          while (!IsBadOrEndOfFile()) {
            tmp = GetLegalTagOrAttributeOrDie();
            switch (tmp.first) {
              case kFatalError: {
                return kFatalError;
              }
              case kParsedLegal: {
                menu_attribute(tmp.second,
                          tmp.second.first(),
                          tmp.second.length() - 1);
                if (menu_attribute == "id") {
                  if (tmp.second.empty()) { // TODO (Matthew) add an error
                                            // message
                    return kFatalError;
                  }
                  buffer_character = tmp.second.back();
                  if (buffer_character != '=') { // TODO (Matthew) add an error
                                                 // message
                    return kFatalError;
                  }
                  pair<bool, char> tmp =
                  IsFatalApostropheOrQuotationMarkError();
                  if (tmp.first) {
                    return kFatalError;
                  }
                  // TODO (Matthew) add the following as a function here and in
                  // GetLegalTagOrAttributeOrDie
                  buffer_character = FileGet();
                  if (IsFatalFileError()) {
                    return kFatalError;
                  }
                  string menu_identification_attribute_value;
                  while (!IsFatalFileError() &&
                         !IsIllegalCharacter(buffer_character)) {
                    menu_identification_attribute_value += buffer_character;
                    buffer_character = FileGet();
                  }
                  if (IsBadOrEndOfFile()) {
                    return kFatalError;
                  }
                  if (IsIllegalCharacter(buffer_character) &&
                      buffer_character != tmp.second) {
                    return kFatalError;
                  }
                  switch (ParseLegalAttributesUntilTagEndOrDie(FileGet())) {
                    case kFatalError: {
                      return kFatalError;
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
                        return kFatalError;
                      }
                      if (option_identification <= 0) { // TODO (Matthew) add an
                                                        // error message
                        return kFatalError;
                      }
                      // TODO (Matthew) add the option tag to the stack and
                      // handle the id somehow
                      return kFound;
#ifdef DEBUG
                      cout <<
                      "An option tag is found at line " <<
                      line <<
                      " and column " <<
                      column <<
                      " of the file \"" <<
                      file_name <<
                      "\". Press Enter to continue.";
                      cin.ignore();
#endif
                    }
                    default: { // TODO (Matthew) add an error message as a
                               // function
                      return kFatalError;
                    }
                  }
                }
                buffer_character = FileGet();
                if (IsFatalFileError()) {
                  return kFatalError;
                }
                if (buffer_character != ' ') { // TODO (Matthew) add an error
                                               // message
                  return kFatalError;
                }
              }
              default: { // TODO (Matthew) add an error message as a function
                return kFatalError;
              }
            }
          }
          return kFatalError;  // TODO (Matthew) add an error message
        }
        // TODO (Matthew) parse legal tag end or die
        switch (ParseLegalAttributesUntilTagEndOrDie(buffer_character)) {
          case kFatalError: {
            return kFatalError;
          }
          case kParsedLegal: {
            // TODO (Matthew) add the tag to the stack
            return kParsedLegal;
          }
          default: { // TODO (Matthew) add an error message as a function
            return kFatalError;
          }
        }
        break;
      }
      default: {
        return kFatalError;
      }
    }
  }
  return kParsedLegal;
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
  if (IsFatalTagOrAttributeStartError(tag_character)) {
    tag_buffer_string = tag_character;
    return make_pair(kFatalError, tag_buffer_string);
  }
  do {
    tag_buffer_string = tag_character;
    tag_character = FileGet();
  }
  while (!IsFatalFileError() &&
         !IsIllegalTagOrAttributeCharacter(tag_character));
  if (IsBadOrEndOfFile()) {
    return make_pair(kFatalError, tag_buffer_string);
  }
  return make_pair(kParsedLegal, tag_buffer_string + tag_character);
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
bool Menu::IsFatalTagOrAttributeStartError(char first_start_character) {
  if (IsIllegalTagOrAttributeStartCharacter(first_start_character)) {
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
bool Menu::LoadOrDie() {
  menu_file_.open(file_name_.c_str());
  if (!menu_file_) {
    BadFile();
    return 0;
  }
  switch (AnalyzeTag()) {
    case kFatalError: {
      return false;
    }
    case kFound: {
      return true;
    }
    case kParsedLegal: {
      if (tmp.second != '<') {
        return false;
      }
      break;
    }
    default: { // TODO (Matthew) add an error message as a function
      return false;
    }
  }
  while (!IsFatalFileError()) { // TODO (Matthew) add an expression
    switch (AnalyzeTag()) {
      case kFatalError: {
        return false;
      }
      case kFound: {
        return true;
      }
      case kParsedLegal: {
        break;
      }
      default: { // TODO (Matthew) add an error message as a function
        return false;
      }
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
    IllegalTagOrAttributeCharacter(attribute_end_character);
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
  while (!IsBadOrEndOfFile() &&
         !IsIllegalCharacter(attribute_end_character) &&
         attribute_end_character != attribute_value_start);
  if (IsFatalFileError()) {
    return make_pair(kFatalError, attribute_end_character);
  }
  if (IsIllegalCharacter(attribute_end_character) &&
      attribute_end_character != attribute_value_start) {
    if (IsFatalCharacterError(attribute_end_character)) {
      return make_pair(kFatalError, attribute_end_character);
    }
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
