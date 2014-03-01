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
pair<bool, char> Menu::IsFatalApostropheOrQuotationMarkError() {
  char attribute_end_character = FileGet();
  if (IsFatalFileError()) {
    return make_pair(true, attribute_end_character);
  }
  if (attribute_end_character != '\'' && attribute_end_character != '\"') {
    cout <<
    "Error: \'" <<
    attribute_end_character <<
    "\' at line " <<
    line <<
    " and column " <<
    column <<
    " of the file \"" <<
    file_name <<
    "\" is not an apostrophe or a quotation mark. Press Enter to exit.";
    cin.ignore();
    return make_pair(true, attribute_end_character);
  }
  return make_pair(false, attribute_end_character);
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
//  attribute_end_character = FileGet();
//  if (IsFatalFileError()) {
//    return make_pair(kFatalError, attribute_end_character);
//  }
//  if (attribute_end_character != '\'' && attribute_end_character != '\"') {
//    cout <<
//    "Error: \'" <<
//    attribute_end_character <<
//    "\' at line " <<
//    line <<
//    " and column " <<
//    column <<
//    " of the file \"" <<
//    file_name <<
//    "\" is not an apostrophe or a quotation mark. Press Enter to exit.";
//    cin.ignore();
//    return make_pair(kFatalError, attribute_end_character);
//  }
//  /* Attribute values must open and close with the same thing.
//   */
//  char attribute_value_start = attribute_end_character;
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
  while (!IsBadOrEndOfFile() && c != '>') {
    /* Attributes must start with a letter or an underscore.
     */
    c = FileGet();
    if (IsFatalFileError()) {
      return kFatalError;
    }
    if (IsFatalTagOrAttributeStartError(c)) {
      return kFatalError;
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
  return kParsedLegal;
}
Menu::FindState Menu::FindMenuTagStartOrParseLegalOrDie() {
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
//          while (!IsBadOrEndOfFile() && tag_start_character != '>') {
//            /* Attributes must start with a letter or an underscore.
//             */
//            tag_start_character = FileGet();
//            if (IsFatalFileError()) {
//              return kFatalError;
//            }
//            if (IsFatalTagOrAttributeStartError(tag_start_character)) {
//              return kFatalError;
//            }
//            /* Attributes must be formed only of letters, numbers, and
//             * underscores. They must be followed immediately by an equals
//             * sign.
//             */
//            do {
//              tag_start_character = FileGet();
//            }
//            while (!IsBadOrEndOfFile() &&
//                   !IsIllegalTagOrAttributeCharacter(
//                       tag_start_character));
//            pair<FindState, char> tmp =
//              ParseLegalAttributeEndOrDie(tag_start_character);
//            switch (tmp.first) {
//              case kFatalError: {
//                return kFatalError;
//              }
//              case kParsedLegal: {
//                tag_start_character = tmp.second;
//                break;
//              }
//              default: {
//                return kFatalError;
//              }
//            }
//          }
//          return kParsedLegal;
          switch(ParseLegalAttributesUntilTagEndOrDie(tag_start_character)) {
            case kFatalError: {
              return kFatalError;
            }
            case kParsedLegal: {
              return kParsedLegal;
            }
            default: {
              return kFatalError;
            }
          }
        }
        case '>': {
          return kParsedLegal;
        }
        default: { // TODO (Matthew) make a unique function for this error
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
Menu::FindState Menu::FindMenuIdentificationValueOrParseLegalOrDie() {
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
  while (!IsBadOrEndOfFile()) {
    pair<FindState, char> tmp = FindStartOrParseLegalOrDie("id=");
    switch (tmp.first) {
      case kFatalError: {
        return kFatalError;
      }
      case kFound: {
        // TODO (Matthew)
#ifdef DEBUG
        cout <<
        "A menu identification attribute is found at line " <<
        line <<
        " and column " <<
        column <<
        " of the file \"" <<
        file_name <<
        "\". Press Enter to continue.";
        cin.ignore();
#endif
        pair<bool, char> tmp = IsFatalApostropheOrQuotationMarkError();
        bool found = true;
        if (tmp.first) {
          return kFatalError;
        }
        char first_menu_identification_value_character = tmp.second;
        char menu_identification_value_start_character = FileGet();
        string menu_identification_value = menu_name +
        first_menu_identification_value_character;
        if (menu_identification_value_start_character != menu_identification_value[0]) {
          if (IsFatalCharacterError(menu_identification_value_start_character)) {
            return kFatalError;
          }
          found = false;
        }
        if (found) {
          for (unsigned int start_element = 1;
               start_element < menu_identification_value.length();
               ++start_element) {
            menu_identification_value_start_character = FileGet();
            if (menu_identification_value_start_character != menu_identification_value[start_element]) {
              found = false;
              break;
            }
          }
        }
        if (!found) {
          while (!IsBadOrEndOfFile() &&
                 !IsIllegalTagOrAttributeCharacter(menu_identification_value_start_character) &&
                 menu_identification_value_start_character != first_menu_identification_value_character) {
            menu_identification_value_start_character = FileGet();
          }
          if (IsFatalFileError()) {
            return kFatalError;
          }
          if (IsIllegalCharacter(menu_identification_value_start_character) &&
              menu_identification_value_start_character !=
              first_menu_identification_value_character) {
            return kFatalError;
          }
        }
        if (menu_identification_value_start_character == first_menu_identification_value_character) {
          menu_identification_value_start_character = FileGet();
          if (IsBadOrEndOfFile()) {
            return kFatalError;
          }
          switch (menu_identification_value_start_character) { // TODO (Matthew) this exists in FindMenuTagStartOrParseLegalOrDie
            case ' ': {
              switch(ParseLegalAttributesUntilTagEndOrDie(FileGet())) {
                case kFatalError: {
                  return kFatalError;
                }
                case kParsedLegal: {
                  if(found) {
                    return kFound;
                  }
                  return kParsedLegal;
                }
                default: {
                  return kFatalError;
                }
              }
            }
            case '>': {
              if (found) {
                return kFound;
              }
              return kParsedLegal;
            }
            default: {
              // TODO (Matthew) make a unique function for this error
              return kFatalError;
            }
          }
          switch(ParseLegalAttributesUntilTagEndOrDie(FileGet())) {
            case kFatalError: {
              return kFatalError;
            }
            case kParsedLegal: {
              if(found) {
                return kFound;
              }
              return kParsedLegal;
            }
            default: {
              return kFatalError;
            }
          }
        }
        return kFatalError;
      }
      case kParsedLegal: {
        char attribute_end_character = tmp.second;
        while (!IsBadOrEndOfFile() &&
               !IsIllegalTagOrAttributeCharacter(
                   attribute_end_character)) {
          attribute_end_character = FileGet();
        }
        tmp = ParseLegalAttributeEndOrDie(
            attribute_end_character);
        switch (tmp.first) {
          case kFatalError: {
            return kFatalError;
          }
          case kParsedLegal: {
            attribute_end_character = tmp.second;
            switch (attribute_end_character) {
              case ' ': {
                continue;
              }
              case '>': {
                return kParsedLegal;
              }
              default: {
                if (IsFatalTagOrAttributeCharacterError(
                    attribute_end_character)) {
                  return kFatalError;
                }
              }
            }
            break;
          }
          default: {
            return kFatalError;
          }
        }
        switch (ParseLegalAttributesUntilTagEndOrDie(
            attribute_end_character)) {
          case kFatalError: {
            return kFatalError;
          }
          case kParsedLegal: {
            return kParsedLegal;
          }
          default: {
            return kFatalError;
          }
        }
      }
      default: {
        return kFatalError;
      }
    }
  }
  return kFatalError;
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
        switch (FindMenuIdentificationValueOrParseLegalOrDie()) {
          case kFatalError: {
            return 0;
          }
          case kFound: {
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
          switch (FindMenuIdentificationValueOrParseLegalOrDie()) {
            case kFatalError: {
              return 0;
            }
            case kFound: {
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
        case kParsedLegal: {
          break;
        }
        default: {
          return 0;
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
