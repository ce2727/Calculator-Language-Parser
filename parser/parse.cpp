/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2019.
*/
#define P_TREE 1 //Toggles tree printing statements
#define P_PREDICT 1 //Toggles predict statements
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
#if P_PREDICT
		std::cout << "matched " << names[input_token];
        if (input_token == t_id || input_token == t_literal)
            std::cout << ": " << token_image;
        std::cout <<  ("\n");
#endif
        input_token = scan ();
    }
    else report_error("match");
}

string program ();
string stmt_list ();
string stmt ();
string expr ();
string term_tail (string t);
string term ();
string factor_tail (string f);
string factor ();
string add_op ();
string mul_op ();
string condition();
void instantiateFirstSet();
void instantiateFollowSet();
void instantiateEPS();
void checkForErrors(string sym);

string program () {
	string out = "";
	checkForErrors("P");
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
		case t_while:
		case t_if:
        case t_eof:
#if P_PREDICT
			std::cout << "predict program --> stmt_list eof\n";
#endif
			out += "(program";
            out += stmt_list ();
			checkForErrors("P");
            match (t_eof);
			out += "])";
            break;
        default: 
			report_error("program");
    }
	return out;
}

string stmt_list () {
	string out = "";
	string sl = "";
	checkForErrors("SL");
    switch (input_token) {
        case t_id:
        case t_read:
		case t_while:
		case t_if:
        case t_write:
#if P_PREDICT
			std::cout << "predict stmt_list --> stmt stmt_list\n";
#endif
			out += stmt();
			sl += stmt_list();
			out += sl;
			break;
		case t_end:
#if P_PREDICT
            std::cout << "predict stmt_list --> stmt stmt_list\n";
#endif
			checkForErrors("SL");
			match(t_end);
            stmt ();
            stmt_list ();
            break;

        case t_eof:
#if P_PREDICT
            std::cout << "predict stmt_list --> epsilon\n";
#endif
            break;          /*  epsilon production */
        default: report_error("stmt_list");
    }
	return out;
}

string stmt () {
	checkForErrors("S");
	string out = "";
    switch (input_token) {
		case t_if:
#if P_PREDICT
			std::cout << "predict stmt --> if condition stmt_list end\n";
#endif
			checkForErrors("S");
			match(t_if);
			out += "(if ";
			out += condition();
			out += ")[";
			out += stmt_list();
			checkForErrors("S");
			match(t_end);
			out += "])";
			break;
		case t_while:
#if P_PREDICT
			std::cout << "predict stmt --> while condition stmt_list end\n";
#endif
			out += "(while ";
			checkForErrors("S");
			match(t_while);
			out += condition();
			out += ")[";
			out += stmt_list();
			checkForErrors("S");
			match(t_end);
			out += "])";
			break;
        case t_id:
#if P_PREDICT
            std::cout << "predict stmt --> id gets expr\n";
#endif
			checkForErrors("S");
			out += "(:= \"";
			out += token_image;
			out += "\"";
            match (t_id);
			checkForErrors("S");
            match (t_gets);
            out += expr ();
			out += ")";
            break;
        case t_read:
#if P_PREDICT
            std::cout << "predict stmt --> read id\n";
#endif
			checkForErrors("S");
			out += "(read ";
            match (t_read);
			checkForErrors("S");
			out += "\"";
			out += token_image;
			out += "\"";
            match (t_id);
			out += ")";
            break;
        case t_write:
#if P_PREDICT
            std::cout << "predict stmt --> write expr\n";
#endif
			checkForErrors("S");
			out += "(write ";
            match (t_write);
            out += expr ();
			out += ")";
            break;
		default: report_error("stmt");
    }
	return out;
}

string condition() {
	string out = "(";
	string expression = "";
	checkForErrors("C");
	switch (input_token) {
	case t_id:
	case t_literal:
	case t_lparen:
#if P_PREDICT
		std::cout << "predict condition --> expression rule_op expression\n";
#endif
		out += "(";
		expression += expr();
		checkForErrors("C");
		out += token_image;
		match(t_rule);
		out += expression + " ";
		out += expr();
		break;
	default: 
		report_error("condition");
	}
	return out;
}

string expr () {
	string out = "";
	string t = "";
	string ttail = "";
	checkForErrors("E");
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
#if P_PREDICT
			std::cout << "predict expr --> term term_tail\n";
#endif
            t += term ();
            ttail += term_tail (t);
			out += ttail;
			if (t.compare(ttail) != 0) out += ")";
            break;
        default: report_error ("expression");
    }
	return out;
}

string term_tail (string t) {
	string out = "";
	string tc = "";
	checkForErrors("TT");
    switch (input_token) {
        case t_add:
        case t_sub:
#if P_PREDICT
			std::cout << "predict term_tail --> add_op term term_tail\n";
#endif
			out += "(";
			out += add_op ();
			out += t + " ";
			tc += term();
            out += term_tail (tc);
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
#if P_PREDICT
			std::cout << "predict term_tail --> epsilon\n";
#endif
			out += t;
            break;          /*  epsilon production */
        default:
			out += t;
			report_error("term_tail");
    }
	return out;
}

string term () {
	string out = "";
	string f = "";
	string ft = "";
	checkForErrors("T");
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
#if P_PREDICT
			std::cout << "predict term --> factor factor_tail\n";
#endif
            f += factor ();
            ft += factor_tail (f);
			out += ft;
			if (f.compare(ft) != 0) out += ")";
            break;
        default: report_error("term");
    }
	return out;
}

string factor_tail (string f) {
	string out = "";
	string ft = "";
	string fc = "";
	checkForErrors("FT");
    switch (input_token) {
        case t_mul:
        case t_div:
#if P_PREDICT
			std::cout << "predict factor_tail --> mul_op factor factor_tail\n";
#endif
			out += "(";
			out += mul_op ();
			out += f + " ";
            fc += factor ();
            ft += factor_tail (fc);
			out += ft;
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
#if P_PREDICT
			std::cout << "predict factor_tail --> epsilon\n";
#endif
			out += f;
            break;          /*  epsilon production */
        default: report_error("factor_tail");
    }
	return out;
}

string factor () {
	string out = "";
	checkForErrors("F");
    switch (input_token) {
        case t_id :
#if P_PREDICT
			std::cout << "predict factor --> id\n";
#endif
			out += "(id \"";
			out += token_image;
			out += "\")";
			checkForErrors("F");
            match (t_id);
            break;
        case t_literal:
#if P_PREDICT
			std::cout << "predict factor --> literal\n";
#endif
			out += "(id \"";
			out += token_image;
			out += "\")";
			checkForErrors("F");
            match (t_literal);
            break;
        case t_lparen:
#if P_PREDICT
			std::cout << "predict factor --> lparen expr rparen\n";
#endif
			checkForErrors("F");
            match (t_lparen);
			out += "(";
            out += expr ();
			checkForErrors("F");
            match (t_rparen);
			out += ")";
            break;
        default: report_error("factor");
    }
	return out;
}

string add_op () {
	string out = "";
	checkForErrors("ao");
    switch (input_token) {
        case t_add:
#if P_PREDICT
			std::cout << "predict add_op --> add\n";
#endif
			out += "+ ";
			checkForErrors("ao");
            match (t_add);
            break;
        case t_sub:
#if P_PREDICT
			std::cout << "predict add_op --> sub\n";
#endif
			out += "- ";
			checkForErrors("ao");
            match (t_sub);
            break;
        default: report_error("add_op");
    }
	return out;
}

string mul_op () {
	string out = "";
	checkForErrors("mo");
    switch (input_token) {
        case t_mul:
#if P_PREDICT
			std::cout << "predict mul_op --> mul\n";
#endif
			out += "* ";
			checkForErrors("mo");
            match (t_mul);
            break;
        case t_div:
#if P_PREDICT
            std::cout <<  "predict mul_op --> div\n";
#endif
			out += "/ ";
			checkForErrors("mo");
            match (t_div);
            break;
        default: report_error("mul_op");
    }
	return out;
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
	string tree = program();
#if P_TREE
    cout << tree;
#endif
    return 0;
}
