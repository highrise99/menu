#include "menu.h"
#define DEBUG
using namespace std;
Menu::Menu(const char* constructor_file_name, const char* constructor_menu_name)
    : file_name(constructor_file_name),
      menu_name(constructor_menu_name),
      line(1),
      column(0) {}
Menu::FileState Menu::IsBadOrEndOfFile() {
  if (!menu_file) {
    return kBad;
  }
  if (menu_file.eof()) {
    return kEndOf;
  }
  return kGood;
}
void Menu::BadFile() {
  cout <<
  "Error: the file \"" <<
  file_name <<
  "\" is bad at line " <<
  line <<
  " and column " <<
  column <<
  ". Press Enter to exit.";
  cin.ignore();
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
      file_name <<
      "\" ends at line " <<
      line <<
      " and column " <<
      column <<
      ". Press Enter to exit.";
      cin.ignore();
      return true;
    }
    default: {
      return true;
    }
  }
}
char Menu::FileGet() {
  char c = menu_file.get();
  if (c == '\n') {
    line += 1;
    column = 0;
  }
  else if (!IsBadOrEndOfFile()) {
    column += 1;
  }
  return c;
}
// TODO: Add support for comments.
bool Menu::IsIllegalTagOrAttributeStart(char first_start_character) {
  if (!isalpha(first_start_character) && first_start_character != '_') {
    return true;
  }
  return false;
}
bool Menu::IsFatalTagOrAttributeStartError(char first_start_character) {
  if (IsIllegalTagOrAttributeStart(first_start_character)) {
    cout <<
    "Error: the character \'" <<
    first_start_character <<
    "\' at line " <<
    line <<
    " and column " <<
    column <<
    " of the file \"" <<
    file_name <<
    "\" is an illegal start to a tag or attribute. Press Enter to exit.";
    cin.ignore();
    return true;
  }
  return false;
}
pair<Menu::FindState, char> Menu::FindStartOrParseLegalOrDie(
    const char *kStart) {
  char start_character = FileGet();
  if (start_character != kStart[0]) {
    if (IsFatalTagOrAttributeStartError(start_character)) {
      return make_pair(kFatalError, start_character);
    }
    return make_pair(kParsedLegal, start_character);
  }
  for (unsigned int start_element = 1;
       start_element < strlen(kStart);
       ++start_element) {
    start_character = FileGet();
    if (start_character != kStart[start_element]) {
      return make_pair(kParsedLegal, start_character);
    }
  }
  return make_pair(kFound, start_character);
}
// TODO: Add support for comments and namespaces.
bool Menu::IsIllegalTagOrAttributeCharacter(char start_character) {
  if (!isalnum(start_character) && start_character != '_') {
    return true;
  }
  return false;
}
void Menu::IllegalTagOrAttributeCharacter(char start_character) {
  cout <<
  "Error: the character \'" <<
  start_character <<
  "\' at line " <<
  line <<
  " and column " <<
  column <<
  " of the file \"" <<
  file_name <<
  "\" is an illegal character in a tag or attribute. Press Enter to exit.";
  cin.ignore();
}
bool Menu::IsFatalTagOrAttributeCharacterError(char start_character) {
  if (IsIllegalTagOrAttributeCharacter(start_character)) {
    IllegalTagOrAttributeCharacter(start_character);
    return true;
  }
  return false;
}
// TODO: Add support for entity references.
bool Menu::IsIllegalCharacter(char c) {
  if (c == '<' || c == '&') {
    return true;
  }
  return false;
}
bool Menu::IsFatalCharacterError(char c) {
  if (IsIllegalCharacter(c)) {
    cout <<
    "Error: the character \'" <<
    c <<
    "\' at line " <<
    line <<
    " and column " <<
    column <<
    " of the file \"" <<
    file_name <<
    "\" is an illegal character. Press Enter to exit.";
    cin.ignore();
    return true;
  }
  return false;
}
Menu::FindState Menu::FindMenuTagStartOrParseLegalOrDie() {
  /*char tag_start_character = FileGet();
  const char *kMenuTagStart = "menu ";
  bool found = true;
  if (tag_start_character != kMenuTagStart[0]) {
    if (IsFatalTagOrAttributeStartError(tag_start_character)) {
      return 0;
    }
    found = false;
  }
  else {
    for (int menu_tag_start_element = 1;
         menu_tag_start_element < strlen(kMenuTagStart);
         ++menu_tag_start_element) {
      tag_start_character = FileGet();
      if (tag_start_character != kMenuTagStart[menu_tag_start_element]) {
        found = false;
        break;
      }
    }
  }
  if (found) {
    return 1;
  }*/
  pair<FindState, char> tmp = FindStartOrParseLegalOrDie("menu ");
  switch (tmp.first) {
    case kFatalError: {
      return kFatalError;
    }
    case kFound: {
      return kFound;
    }
    case kParsedLegal: {
      char tag_start_character = tmp.second;
      while (!IsBadOrEndOfFile() &&
             !IsIllegalTagOrAttributeCharacter(tag_start_character)) {
        tag_start_character = FileGet();
      }
      if (IsFatalFileError()) {
        return kFatalError;
      }
      switch (tag_start_character) {
        case ' ': {
          /* Until a greater than is read, a file error is encountered, or the
           * end of the file is reached, attributes are parsed for syntax
           * errors.
           */
          while (!IsBadOrEndOfFile() && tag_start_character != '>') {
            /* Attributes must start with a letter or an underscore.
             */
            tag_start_character = FileGet();
            if (IsFatalFileError()) {
              return kFatalError;
            }
            if (IsFatalTagOrAttributeStartError(tag_start_character)) {
              return kFatalError;
            }
            /* Attributes must be formed only of letters, numbers, and
             * underscores. They must be followed immediately by an equals
             * sign.
             */
            do {
              tag_start_character = FileGet();
            }
            while (!IsBadOrEndOfFile() &&
                   !IsIllegalTagOrAttributeCharacter(
                       tag_start_character));
            if (IsFatalFileError()) {
              return kFatalError;
            }
            if (IsIllegalTagOrAttributeCharacter(tag_start_character)
                && tag_start_character != '=') {
              IllegalTagOrAttributeCharacter(tag_start_character);
              return kFatalError;
            }
            /* Attribute values must open either with an apostrophe or a
             * quotation mark.
             */
            tag_start_character = FileGet();
            if (IsFatalFileError()) {
              return kFatalError;
            }
            if (tag_start_character != '\'' && tag_start_character != '\"') {
              cout <<
              "Error: \'" <<
              tag_start_character <<
              "\' at line " <<
              line <<
              " and column " <<
              column <<
              " of the file \"" <<
              file_name <<
              "\" is not an apostrophe or a quotation mark. Press Enter to exit.";
              cin.ignore();
              return kFatalError;
            }
            /* Attribute values must open and close with the same thing.
             */
            char attribute_value_start = tag_start_character;
            do {
              tag_start_character = FileGet();
            }
            while (!IsBadOrEndOfFile() &&
                   !IsIllegalCharacter(tag_start_character) &&
                   tag_start_character != attribute_value_start);
            if (IsFatalFileError()) {
              return kFatalError;
            }
            if (IsIllegalCharacter(tag_start_character) &&
                tag_start_character != attribute_value_start) {
              if (IsFatalCharacterError(tag_start_character)) {
                return kFatalError;
              }
            }
            tag_start_character = FileGet();
          }
          return kParsedLegal;
        }
        case '>': {
          return kParsedLegal;
        }
        default: {
          if (IsFatalTagOrAttributeCharacterError(tag_start_character)) {
            return kFatalError;
          }
        }
      }
    }
    default: {
      return kFatalError;
    }
  }
} 
void Menu::FoundMenuTag() {
#ifdef DEBUG
  cout <<
  "The start of a menu tag is found at line " <<
  line <<
  " and column " <<
  column <<
  " of the file \"" <<
  file_name <<
  "\". Press Enter to continue.";
  cin.ignore();
#endif
}
int Menu::RequestOrDie() {
  menu_file.open(file_name.c_str());
  if (!menu_file) {
    BadFile();
    return 0;
  }
  char current_character = FileGet();
  if (IsFatalFileError()) {
    return 0;
  }
  if (current_character == '<') {
    switch (FindMenuTagStartOrParseLegalOrDie()) {
      case kFatalError: {
        return 0;
      }
      case kFound: {
        FoundMenuTag();
        return 1;
      }
      case kParsedLegal: {
        break;
      }
      default: {
        return 0;
      }
    }
  }
  else {
    cout <<
    "Error: \'" <<
    current_character <<
    "\' at line " <<
    line <<
    " and column " <<
    column <<
    " of the file \"" <<
    file_name <<
    "\" is not an greater-than sign. Press Enter to exit.";
    cin.ignore();
    return 0;
  }
  current_character = FileGet();
  while (!IsBadOrEndOfFile()) {
    if (current_character == '<') {
      switch (FindMenuTagStartOrParseLegalOrDie()) {
        case kFatalError: {
          return 0;
        }
        case kFound: {
          FoundMenuTag();
          return 1;
        }
        case kParsedLegal: {
          break;
        }
      }
    }
    else if (IsFatalCharacterError(current_character)) {
      return 0;
    }
    current_character = FileGet();
  }
  return 0;
}
