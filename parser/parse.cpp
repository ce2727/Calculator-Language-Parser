/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2019.
*/
#include <iostream>
#include <map>
#include <list>
#include <algorithm>
#include "scan.h"
using namespace std;

const char* names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", "rparen", "eof", "if", "while", "end", "rule"};

static token input_token;
std::map<string, list<token> > first;
map<string, list<token> > follow;
map<string, bool> eps;

void report_error(string sym) {
	std::cout << "syntax error found in: " << sym << endl;
}

void match (token expected) {
    if (input_token == expected) {
		std::cout << "matched " << names[input_token];
        if (input_token == t_id || input_token == t_literal)
            std::cout << ": " << token_image;
        std::cout <<  ("\n");
        input_token = scan ();
    }
    else report_error("match");
}

void program ();
void stmt_list ();
void stmt ();
void expr ();
void term_tail ();
void term ();
void factor_tail ();
void factor ();
void add_op ();
void mul_op ();
void condition();
void instantiateFirstSet();
void instantiateFollowSet();
void instantiateEPS();
void checkForErrors(string sym);

void program () {
	checkForErrors("P");
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
		case t_while:
		case t_if:
        case t_eof:
			std::cout << "predict program --> stmt_list eof\n";
            stmt_list ();
			checkForErrors("P");
            match (t_eof);
            break;
        default: 
			report_error("program");
    }
	std::cout << "FINISH";
}

void stmt_list () {
	checkForErrors("SL");
    switch (input_token) {
        case t_id:
        case t_read:
		case t_while:
		case t_if:
        case t_write:
			std::cout << "predict stmt_list --> stmt stmt_list\n";
			stmt();
			stmt_list();
			break;
		case t_end:
            std::cout << "predict stmt_list --> stmt stmt_list\n";
			checkForErrors("SL");
			match(t_end);
            stmt ();
            stmt_list ();
            break;

        case t_eof:
            std::cout << "predict stmt_list --> epsilon\n";
            break;          /*  epsilon production */
        default: report_error("stmt_list");
    }
}

void stmt () {
	checkForErrors("S");
    switch (input_token) {
		case t_if:
			std::cout << "predict stmt --> if condition stmt_list end\n";
			checkForErrors("S");
			match(t_if);
			condition();
			stmt_list();
			checkForErrors("S");
			match(t_end);
			break;
		case t_while:
			std::cout << "predict stmt --> while condition stmt_list end\n";
			checkForErrors("S");
			match(t_while);
			condition();
			stmt_list();
			checkForErrors("S");
			match(t_end);
			break;
        case t_id:
            std::cout << "predict stmt --> id gets expr\n";
			checkForErrors("S");
            match (t_id);
			checkForErrors("S");
            match (t_gets);
            expr ();
            break;
        case t_read:
            std::cout << "predict stmt --> read id\n";
			checkForErrors("S");
            match (t_read);
			checkForErrors("S");
            match (t_id);
            break;
        case t_write:
            std::cout << "predict stmt --> write expr\n";
			checkForErrors("S");
            match (t_write);
            expr ();
            break;
		default: report_error("stmt");
    }
}

void condition() {
	checkForErrors("C");
	switch (input_token) {
	case t_id:
	case t_literal:
	case t_lparen:
		std::cout << "predict condition --> expression rule_op expression\n";
		expr();
		checkForErrors("C");
		match(t_rule);
		expr();
		break;
	default: report_error("condition");
	}
}

void expr () {
	checkForErrors("E");
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
			std::cout << "predict expr --> term term_tail\n";
            term ();
            term_tail ();
            break;
        default: report_error ("expression");
    }
}

void term_tail () {
	checkForErrors("TT");
    switch (input_token) {
        case t_add:
        case t_sub:
			std::cout << "predict term_tail --> add_op term term_tail\n";
            add_op ();
            term ();
            term_tail ();
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
		case t_if:
		case t_while:
		case t_rule:
		case t_end:
        case t_eof:
			std::cout << "predict term_tail --> epsilon\n";
            break;          /*  epsilon production */
        default: report_error("term_tail");
    }
}

void term () {
	checkForErrors("T");
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
			std::cout << "predict term --> factor factor_tail\n";
            factor ();
            factor_tail ();
            break;
        default: report_error("term");
    }
}

void factor_tail () {
	checkForErrors("FT");
    switch (input_token) {
        case t_mul:
        case t_div:
			std::cout << "predict factor_tail --> mul_op factor factor_tail\n";
            mul_op ();
            factor ();
            factor_tail ();
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
		case t_if:
		case t_while:
		case t_rule:
		case t_end:
        case t_eof:
			std::cout << "predict factor_tail --> epsilon\n";
            break;          /*  epsilon production */
        default: report_error("factor_tail");
    }
}

void factor () {
	checkForErrors("F");
    switch (input_token) {
        case t_id :
			std::cout << "predict factor --> id\n";
			checkForErrors("F");
            match (t_id);
            break;
        case t_literal:
			std::cout << "predict factor --> literal\n";
			checkForErrors("F");
            match (t_literal);
            break;
        case t_lparen:
			std::cout << "predict factor --> lparen expr rparen\n";
			checkForErrors("F");
            match (t_lparen);
            expr ();
			checkForErrors("F");
            match (t_rparen);
            break;
        default: report_error("factor");
    }
}

void add_op () {
	checkForErrors("ao");
    switch (input_token) {
        case t_add:
			std::cout << "predict add_op --> add\n";
			checkForErrors("ao");
            match (t_add);
            break;
        case t_sub:
			std::cout << "predict add_op --> sub\n";
			checkForErrors("ao");
            match (t_sub);
            break;
        default: report_error("add_op");
    }
}

void mul_op () {
	checkForErrors("mo");
    switch (input_token) {
        case t_mul:
			std::cout << "predict mul_op --> mul\n";
			checkForErrors("mo");
            match (t_mul);
            break;
        case t_div:
            std::cout <<  "predict mul_op --> div\n";
			checkForErrors("mo");
            match (t_div);
            break;
        default: report_error("mul_op");
    }
}

void instantiateFirstSet() {
	//should P_List and SL_List include epsilon? if so how
	list<token> P_List = { t_eof, t_id, t_read, t_write, t_if, t_while };
	first.insert({ "P", P_List });
	list<token> SL_List = { t_id, t_read, t_write, t_if, t_while };
	first.insert({ "SL", SL_List });
	list<token> S_List = { t_id, t_read, t_write, t_if, t_while };
	first.insert({ "S", S_List });
	list<token> C_List = { t_lparen, t_id, t_literal };
	first.insert({ "C", C_List });
	list<token> E_List = { t_lparen, t_id, t_literal };
	first.insert({ "E", E_List });
	list<token> T_List = { t_lparen, t_id, t_literal };
	first.insert({ "T", T_List });
	list<token> F_List = { t_lparen, t_id, t_literal };
	first.insert({ "F", F_List });
	list<token> TT_List = { t_add, t_sub };//should include epsilon?
	first.insert({ "TT", TT_List });
	list<token> FT_List = { t_mul, t_div };//should include epsilon?
	first.insert({ "FT", FT_List });
	list<token> ro_List = { t_rule };
	first.insert({ "ro", ro_List });
	list<token> ao_List = { t_add, t_sub };
	first.insert({ "ao", ao_List });
	list<token> mo_List = { t_mul, t_div };
	first.insert({ "mo", mo_List });
}

void instantiateFollowSet() {
	list<token> P_List = {};
	follow.insert({ "P", P_List });
	list<token> SL_List = { t_end, t_eof };
	follow.insert({ "SL", SL_List });
	list<token> S_List = { t_id, t_read, t_write, t_if, t_while, t_end, t_eof };
	follow.insert({ "S", S_List });
	list<token> C_List = { t_id, t_read, t_write, t_if, t_while };
	follow.insert({ "C", C_List });
	list<token> E_List = { t_rparen, t_rule, t_id, t_read, t_write, t_if, t_while, t_end, t_eof };
	follow.insert({ "E", E_List });
	list<token> T_List = { t_add, t_sub, t_rparen, t_rule, t_id, t_read, t_write, t_if, t_while, t_end, t_eof };
	follow.insert({ "T", T_List });
	list<token> F_List = { t_mul, t_div,t_add, t_sub, t_rparen, t_rule, t_id, t_read, t_write, t_if, t_while, t_end, t_eof };
	follow.insert({ "F", F_List });
	list<token> TT_List = { t_rparen, t_rule, t_id, t_read, t_write, t_if, t_while, t_end, t_eof };
	follow.insert({ "TT", TT_List });
	list<token> FT_List = { t_add, t_sub, t_rparen, t_rule, t_id, t_read, t_write, t_if, t_while, t_end, t_eof };
	follow.insert({ "FT", FT_List });
	list<token> ro_List = { t_lparen, t_id, t_literal };
	follow.insert({ "ro", ro_List });
	list<token> ao_List = { t_lparen, t_id, t_literal };
	follow.insert({ "ao", ao_List });
	list<token> mo_List = { t_lparen, t_id, t_literal };
	follow.insert({ "mo", mo_List });
}

void instantiateEPS() {
	eps.insert({ "P", false });
	eps.insert({ "SL", true });
	eps.insert({ "S", false });
	eps.insert({ "C", false });
	eps.insert({ "E", false });
	eps.insert({ "T", false });
	eps.insert({ "F", false });
	eps.insert({ "TT", true });
	eps.insert({ "FT", true });
	eps.insert({ "ro", false });
	eps.insert({ "ao", false });
	eps.insert({ "mo", false });
}

void checkForErrors(string sym) {
	list<token> firstSet = first[sym];
	list<token> followSet = follow[sym];
	bool EPS = eps[sym];
	bool containsInFirst = (find(firstSet.begin(), firstSet.end(), input_token) != firstSet.end());

	if (!(containsInFirst || EPS)) {
		report_error("check");
		bool containsInFollow;
		bool eof;
		do {
			input_token = scan();
			containsInFirst = (find(firstSet.begin(), firstSet.end(), input_token) != firstSet.end());
			containsInFollow = (find(followSet.begin(), followSet.end(), input_token) != followSet.end());
			eof = (input_token == t_eof);
		} while (!(containsInFirst || containsInFollow || eof));
	}
	else {
		return;
	}
}

int main () {
	instantiateFirstSet();
	instantiateFollowSet();
	instantiateEPS();
    input_token = scan ();
    program ();
    return 0;
}
