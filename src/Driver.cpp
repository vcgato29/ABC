/*
 * Driver.cpp
 *
 *  Created on: Nov 17, 2014
 *      Author: baki
 */

#include "Driver.h"

namespace Vlab {

//const Log::Level Driver::TAG = Log::DRIVER;
//PerfInfo* Driver::perfInfo;

Driver::Driver()
	: script (nullptr)/*, symbol_table (nullptr)*/ {
}

Driver::~Driver() {
	delete script;
}

void Driver::error (const std::string& m) {
  std::cerr << m << std::endl;
}

int Driver::parse (std::istream* in) {
	Scanner scanner(in);
//	scanner.set_debug(trace_scanning);
	Parser parser (script, scanner);
//	parser.set_debug_level (trace_parsing);
	int res = parser.parse ();
	return res;
}

void Driver::ast2dot(std::ostream* out) {

	SMT::Ast2Dot ast2dot(out);
	ast2dot.visit(script);
	ast2dot.finish();
}
void Driver::ast2dot(std::string file_name) {
	std::ofstream outfile(file_name.c_str());
	if ( !outfile.good() ) { std::cout << "cannot open file: " << file_name << std::endl; exit(2); }
	ast2dot(&outfile);
	outfile.close();
}

//void Driver::collectStatistics() {
//	SMT::Statistics::perfInfo = perfInfo;
//	SMT::Statistics statistics;
//	statistics.start(script, symbol_table);
//}

void Driver::initializeSolver() {

	SMT::Initializer initializer(script);
	initializer.start();

//	symbol_table = std::make_shared<SMT::SymbolTable>();
//	SMT::Initializer initializer;
//	initializer.start(script, symbol_table);
//
//	SMT::BooleanVarReductor boolean_reductor;
//	boolean_reductor.start(script, symbol_table);
//
//	SMT::IntVarReductor int_reductor;
//	int_reductor.start(script, symbol_table);
//
//	SMT::LengthConstraintReductor len_reductor;
//	len_reductor.start(script, symbol_table);
}

//void Driver::reduceVars() {
//	SMT::IntVarReductor int_reductor;
//	int_reductor.start(script, symbol_table);
//
//	SMT::StrVarReductor str_reductor;
//	str_reductor.start(script, symbol_table);
//}
//
//void Driver::preProcessAst() {
//	SMT::AstConditionalProcessor condProcessor;
//	condProcessor.start(script, symbol_table);
//
//	SMT::AstSorter::perfInfo = perfInfo;
//	SMT::AstSorter command_sorter;
//	command_sorter.start(script, symbol_table);
//
//	SMT::AstOptimizer optimizer;
//	optimizer.start(script, symbol_table);
//}

void Driver::error(const Vlab::location& l, const std::string& m) {
	std::cerr << l << ": " << m << std::endl;
}


//void Driver::solveAst() {
//	int i = 0;
//	SMT::PostImageComputer solver(symbol_table);
//	solver.visit(script);
//
//	for (i = i + 1; i < 1 and symbol_table->is_all_assertions_valid(); i++) {
//		std::cout << "additional iteration: " << i << std::endl;
//		symbol_table->pop_scope();
//		SMT::PostImageComputer solver(symbol_table);
//		solver.visit(script);
//	}
//}0--7

}
