//
// Created by Saikat Chakraborty on 9/11/20.
//
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <ctype.h>
#include "Lexer.h"

/*
 * We simulate the state transitions with a DFA here.
 * The next state should be constructed based on the current state and the
 * input character.
 * Gotcha note!
 *   1. Be careful about whitespaces.
 *      Whitespaces usually finish a token, except for strings and comments.
 *      For example, `hello world ` contains 2 identifier tokens, each terminated
 *      with a whitespace.
 *      However, `"Hello world "` contains 1 string token, and both spaces are
 *      included in the token lexeme.
 *   2. Whitespaces (i.e. ' ', '\t', '\n') usually end sequences of characters
 *      in a token. However, other characters can also end a token stream.
 *      For example,
 *        a. print ( ) -> the tokens here are <ID, "print">, <LPAR, "(">, and
 *           <RPAR, ")">
 *        b. print() -> the tokens here are also <ID, "print">, <LPAR, "(">, and
 *           <RPAR, ")">
 *      Note that, in the former case, the space, ' ', after the token print
 *      ended the "print" token.
 *      However, in the latter case, "print" is followed by a left parenthesis,
 *      '(', which ends the "print" token.
 *      Your code should be able to handle both these scenarios.
 *   3. For comments, you should discard the newline character ('\n') that ends
 *      the comments. See LexerTest.cpp for the relevant test case.
 */
std::string stateTransition(std::string current_state, char ch) {
    char state = current_state[0];
    char status = current_state[1];
    switch (state) {
        // state 0: init
        case '0':
            if (ch == '_' || isalpha(ch)){
                current_state[0] = '1'; // transit to sate 1: identifier state
            }
            else if (ch == '.' || isdigit(ch)){
                current_state[0] = '2'; // transit to state 2: number state
                current_state[1] = (ch == '.')? 'i': '0';
            }
            else if (ch == '\"'){
                current_state[0] = '3'; // transit to state 3: string state
                current_state[1] = 'i';
            }
            else if (ch == '/'){
                current_state[0] = '4'; // transit to state 4: comment state
                current_state[1] = '1';
            }
            else if (ch == '+' || ch == '-'){
                current_state[0] = '5'; // transit to state 5: additive operator state
                current_state[1] = '1';
            }
            else if (ch == '<' || ch == '>' || ch == '='){
                current_state[0] = '6'; // transit to state 6: comparitive operator state
                current_state[1] = '1';
            }
            else if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ';'){
                current_state[0] = 'f'; // transit to state f: special character
            }
            else if (isspace(ch)){
                current_state[0] = 'f'; // transit to state f: whitespace
            }
            else{
                current_state[0] = 'n'; // not implemented
            }
            current_state.push_back(ch);
            return current_state;
        // state 1: identifier
        case '1':
            if (ch == '_' || isalpha(ch) || isdigit(ch)){
                current_state.push_back(ch);
            }
            else{
                // encounter end-words; transit to state F: final state without taking the current character
                current_state[0] = 'F';
            }
            return current_state;
        // state 2: number (status: 0 -> no dot / i -> one dot with no following digit / 1 -> one dot with following digit)
        case '2':
            if (ch == '.' || isalpha(ch) || isdigit(ch) || ch == '_'){
                current_state.push_back(ch);
                if (status == 'i'){
                    if (isdigit(ch)){
                        current_state[1] = '1';
                    } else {
                        current_state[0] = 'f';
                        current_state[1] = 'e';
                    }
                }
                else if (status == '0' && ch == '.'){
                    current_state[1] = 'i';
                }
                else if ((status == '1' && ch == '.') || !isdigit(ch)){
                    current_state[0] = 'f';
                    current_state[1] = 'e'; // set status to e: error
                }
            }
            else {
                if (status == 'i'){
                    current_state[0] = 'f';
                    current_state[1] = 'e';
                }
                current_state[0] = 'F'; // encounter end-words; transit to state F: final state without taking the current character
            }
            return current_state;
        // state 3: string (status: 1 -> single quote / f -> double)
        case '3':
            if (ch == '\"'){
                current_state[0] = 'f'; // encounter end-words; transit to state f: final state with taking the current character
                current_state[1] = '2';
            }
            current_state.push_back(ch);
            return current_state;
        // state 4: comment (status: 1 -> single back slash / i -> double back slash & waiting \n)
        case '4':
            if (status == '1'){
                if (ch == '/') {
                    current_state[1] = 'i';
                    current_state.push_back(ch);
                } else {
                    current_state[0] = 'F';
                    current_state[1] = 'e';
                }
            }
            else if (ch == '\n') {
                current_state[0] = 'F';
            }
            else{
                current_state.push_back(ch);
            }
            return current_state;
        // state 5: addtive op (status: 1 -> single op / f -> double)
        case '5':
            if (status == '1'){
                if (current_state.back() == ch){
                    current_state[0] = 'f';
                    current_state.push_back(ch);
                }
                else if (isdigit(ch) || (ch == '.')){
                    current_state[0] = '2'; // transit to state 2: number state
                    current_state[1] = (ch == '.')? 'i': '0';
                    current_state.push_back(ch);
                }
                else{
                    current_state[0] = 'F'; // transit to state F
                }
            }
            return current_state;
        // state 6: comp op (status: 1 -> single op / f -> contains =)
        case '6':
            if ((status == '1') && (ch == '=')){
                current_state[0] = 'f';
                current_state.push_back(ch);
            }
            else{
                current_state[0] = 'F';
            }
            return current_state;
        default:
            current_state[1] = 'n';
            return current_state;
    }
}

std::vector<Token> pushBackToken(std::vector<Token> tokens, std::string res) {
    char state = res[0];
    char status = res[1];
    std::string token = res.substr(2);

    std::vector<std::string> kw_list={"if", "else", "for", "while", "extern", "asm"};

    if (status == 'e'){
        switch (state){
        case '2': tokens.emplace_back(TokenType::type_error, "Invalid Number"); break;
        case '3': tokens.emplace_back(TokenType::type_error, "Invalid String"); break;
        case '4': tokens.emplace_back(TokenType::type_error, "Invalid Comment"); break;
        default: break;
        }
        return tokens;
    }

    switch (state){
    case '0':
        switch (token[0]){
        case '(': tokens.emplace_back(TokenType::type_lpar, "("); break;
        case ')': tokens.emplace_back(TokenType::type_rpar, ")"); break;
        case '{': tokens.emplace_back(TokenType::type_lcurly_brace, "{"); break;
        case '}': tokens.emplace_back(TokenType::type_rcurly_brace, "}"); break;
        case ';': tokens.emplace_back(TokenType::type_semicolon, ";"); break;
        default: break;
        }
        break;
    case '1':
        if (std::find(std::begin(kw_list), std::end(kw_list), token) != std::end(kw_list)){
            tokens.emplace_back(TokenType::type_keyword, token);
        }
        else{
            tokens.emplace_back(TokenType::type_identifier, token);
        }
        break;
    case '2': tokens.emplace_back(TokenType::type_number, token); break;
    case '3': tokens.emplace_back(TokenType::type_string, token); break;
    case '4': tokens.emplace_back(TokenType::type_comment, token); break;
    case '5':
    case '6':
        if (token == "="){
            tokens.emplace_back(TokenType::type_assign_op, token); break;
        }
        else if ((token == "++") || (token == "--")){
            tokens.emplace_back(TokenType::type_unaryop, token); break;
        }
        else {
            tokens.emplace_back(TokenType::type_binop, token); break;
        }
    default: break;
    }
    return tokens;
}

/***
 * The main function for lexical analysis/tokenization.
 *
 * @param   _character_stream of type std::string
 * @return  std::vector of type Token (declared above)
 *
 */
std::vector<Token> tokenizeCode(std::string _character_stream) {
    std::vector<Token> tokens;
    /*
     * Note to the students:
     *   You should not make any changes to the function prototype (i.e.,
     *   function name, parameter, and return type). Any such changes cause the
     *   test suite to fail.
     *   You may define auxiliary/helper functions, which can then be called
     *   from this function.
     */

    // Initially, we start with a null or empty state.
    std::string current_state = "00";
    std::string next_state = "";
    // bool token_accepted = false;
    // We scan character by character.
    for (std::string::iterator string_iter = _character_stream.begin();
         string_iter != _character_stream.end();
         string_iter++)
    {
        char current_character = *string_iter;
        next_state = stateTransition(current_state, current_character);

        if (next_state[0] == 'f' || next_state[0] == 'F'){
            if (next_state[0] == 'F'){
                string_iter--; // iterator again on current char
            }
            next_state[0] = current_state[0];
            current_state = "00";
            tokens = pushBackToken(tokens, next_state);
            if (next_state[1] == 'e'){
                break;
            }
        }
        else{
            current_state = next_state;
        }
    }
    if (current_state != "00" && next_state[1] != 'e'){
        if (current_state[1] == 'i' || (current_state[0] == '4' && current_state[1] == '1')){
            current_state[1] = 'e';
        }
        tokens = pushBackToken(tokens, current_state);
    }
    return tokens;
}

TokenType Token::getType() {
    return this->type;
}

std::string Token::getLexeme() {
    return this->lexeme;
}

bool Token::operator==(const Token& other) const{
    return this->type == other.type && this->lexeme == other.lexeme;
}

bool Token::operator!=(const Token& other) const{
    return type != other.type || lexeme != other.lexeme;
}

std::string Token::get_repr()
{
    std::string return_str = "<" + get_token_type_string(this->type) + ", " +
                             this->lexeme + ">";
    return return_str;
}

Token::Token(TokenType _type, std::string _lexeme) {
    this->type = _type;
    this->lexeme = std::move(_lexeme);
}

std::string get_token_type_string(TokenType _type) {
    switch (_type) {
    case type_lpar:
        return "LPAR";
    case type_rpar:
        return "RPAR";
    case type_lcurly_brace:
        return "L-CURLY-BRACE";
    case type_rcurly_brace:
        return "R-CURLY-BRACE";
    case type_keyword:
        return "KEYWORD";
    case type_identifier:
        return "ID";
    case type_number:
        return "NUMBER";
    case type_string:
        return "STRING";
    case type_comment:
        return "COMMENT";
    case type_error:
        return "ERROR";
    case type_unaryop:
        return "UNOP";
    case type_binop:
        return "BINOP";
    case type_semicolon:
        return "SEMICOLON";
    case type_assign_op:
        return "ASSIGN";
    default:
        return "UNKNOWN";
    }
}
