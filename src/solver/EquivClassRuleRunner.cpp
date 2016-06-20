#include "EquivClassRuleRunner.h"

namespace Vlab {
namespace Solver {

using namespace SMT;

const int EquivClassRuleRunner::VLOG_LEVEL = 16;

EquivClassRuleRunner::EquivClassRuleRunner(Script_ptr script, SymbolTable_ptr symbol_table, SubstitutionTable& substitution_table, std::set<Visitable_ptr> mark_as_false)
  : root(script), symbol_table_(symbol_table), substitution_table_ (substitution_table), mark_as_false_(mark_as_false) {
}

EquivClassRuleRunner::~EquivClassRuleRunner() {
}

void EquivClassRuleRunner::start() {
  //TOOD: Also need to check if any terms should be replaced by false.
  if (not has_optimization_rules()) {
    return;
  }

  symbol_table_->push_scope(root);
  visit(root);
  symbol_table_->pop_scope();

  end();

}
void EquivClassRuleRunner::end() {
  SyntacticOptimizer syntactic_optimizer(root, symbol_table_);
  syntactic_optimizer.start();
}


void EquivClassRuleRunner::visitScript(Script_ptr script) {
  visit_children_of(script);
}

void EquivClassRuleRunner::visitCommand(Command_ptr command) {

  switch (command->getType()) {
  case Command::Type::ASSERT: {
    Assert_ptr assert_command = dynamic_cast<Assert_ptr>(command);
    check_and_substitute_var(assert_command->term);
    visit_children_of(assert_command);
    break;
  }
  default:
    LOG(FATAL) << "'" << *command << "' is not expected.";
    break;
  }
}

void EquivClassRuleRunner::visitAssert(Assert_ptr assert_command) {
  check_and_substitute_var(assert_command->term);
  visit_children_of(assert_command);
}

void EquivClassRuleRunner::visitTerm(Term_ptr term) {
}

void EquivClassRuleRunner::visitExclamation(Exclamation_ptr exclamation_term) {
}

void EquivClassRuleRunner::visitExists(Exists_ptr exists_term) {
}

void EquivClassRuleRunner::visitForAll(ForAll_ptr for_all_term) {
}

void EquivClassRuleRunner::visitLet(Let_ptr let_term) {
  LOG(ERROR) << "optimizations are not checked for let term";
}

void EquivClassRuleRunner::visitAnd(And_ptr and_term) {
  for (auto& term : * (and_term->term_list)) {
    check_and_substitute_var(term);
    visit(term);
  }
}

void EquivClassRuleRunner::visitOr(Or_ptr or_term) {
  for (auto& term : * (or_term->term_list)) {
    check_and_substitute_var(term);
    symbol_table_->push_scope(term);
    visit(term);
    symbol_table_->pop_scope();
  }
}

void EquivClassRuleRunner::visitNot(Not_ptr not_term) {
  check_and_substitute_var(not_term->term);
  visit_children_of(not_term);
}

void EquivClassRuleRunner::visitUMinus(UMinus_ptr u_minus_term) {
  check_and_substitute_var(u_minus_term->term);
  visit_children_of(u_minus_term);
}

void EquivClassRuleRunner::visitMinus(Minus_ptr minus_term) {
  check_and_substitute_var(minus_term->left_term);
  check_and_substitute_var(minus_term->right_term);
  visit_children_of(minus_term);
}

void EquivClassRuleRunner::visitPlus(Plus_ptr plus_term) {
  for (auto& term_ptr : * (plus_term->term_list)) {
    check_and_substitute_var(term_ptr);
    visit(term_ptr);
  }
}

void EquivClassRuleRunner::visitTimes(Times_ptr times_term) {
  for (auto& term_ptr : * (times_term->term_list)) {
    check_and_substitute_var(term_ptr);
    visit(term_ptr);
  }
}

void EquivClassRuleRunner::visitEq(Eq_ptr eq_term) {
  check_and_substitute_var(eq_term->left_term);
  check_and_substitute_var(eq_term->right_term);
  visit_children_of(eq_term);
}


void EquivClassRuleRunner::visitNotEq(NotEq_ptr not_eq_term) {
  check_and_substitute_var(not_eq_term->left_term);
  check_and_substitute_var(not_eq_term->right_term);
  visit_children_of(not_eq_term);
}

void EquivClassRuleRunner::visitGt(Gt_ptr gt_term) {
  check_and_substitute_var(gt_term->left_term);
  check_and_substitute_var(gt_term->right_term);
  visit_children_of(gt_term);
}

void EquivClassRuleRunner::visitGe(Ge_ptr ge_term) {
  check_and_substitute_var(ge_term->left_term);
  check_and_substitute_var(ge_term->right_term);
  visit_children_of(ge_term);
}

void EquivClassRuleRunner::visitLt(Lt_ptr lt_term) {
  check_and_substitute_var(lt_term->left_term);
  check_and_substitute_var(lt_term->right_term);
  visit_children_of(lt_term);
}

void EquivClassRuleRunner::visitLe(Le_ptr le_term) {
  check_and_substitute_var(le_term->left_term);
  check_and_substitute_var(le_term->right_term);
  visit_children_of(le_term);
}

void EquivClassRuleRunner::visitConcat(Concat_ptr concat_term) {
  for (auto& term_ptr : * (concat_term->term_list)) {
    check_and_substitute_var(term_ptr);
    visit(term_ptr);
  }
}

void EquivClassRuleRunner::visitIn(In_ptr in_term) {
  check_and_substitute_var(in_term->left_term);
  check_and_substitute_var(in_term->right_term);
  visit_children_of(in_term);
}

void EquivClassRuleRunner::visitNotIn(NotIn_ptr not_in_term) {
  check_and_substitute_var(not_in_term->left_term);
  check_and_substitute_var(not_in_term->right_term);
  visit_children_of(not_in_term);
}

void EquivClassRuleRunner::visitLen(Len_ptr len_term) {
  check_and_substitute_var(len_term->term);
  visit_children_of(len_term);
}

void EquivClassRuleRunner::visitContains(Contains_ptr contains_term) {
  check_and_substitute_var(contains_term->subject_term);
  check_and_substitute_var(contains_term->search_term);
  visit_children_of(contains_term);
}

void EquivClassRuleRunner::visitNotContains(NotContains_ptr not_contains_term) {
  check_and_substitute_var(not_contains_term->subject_term);
  check_and_substitute_var(not_contains_term->search_term);
  visit_children_of(not_contains_term);
}

void EquivClassRuleRunner::visitBegins(Begins_ptr begins_term) {
  check_and_substitute_var(begins_term->subject_term);
  check_and_substitute_var(begins_term->search_term);
  visit_children_of(begins_term);
}

void EquivClassRuleRunner::visitNotBegins(NotBegins_ptr not_begins_term) {
  check_and_substitute_var(not_begins_term->subject_term);
  check_and_substitute_var(not_begins_term->search_term);
  visit_children_of(not_begins_term);
}

void EquivClassRuleRunner::visitEnds(Ends_ptr ends_term) {
  check_and_substitute_var(ends_term->subject_term);
  check_and_substitute_var(ends_term->search_term);
  visit_children_of(ends_term);
}

void EquivClassRuleRunner::visitNotEnds(NotEnds_ptr not_ends_term) {
  check_and_substitute_var(not_ends_term->subject_term);
  check_and_substitute_var(not_ends_term->search_term);
  visit_children_of(not_ends_term);
}

void EquivClassRuleRunner::visitIndexOf(IndexOf_ptr index_of_term) {
  check_and_substitute_var(index_of_term->subject_term);
  check_and_substitute_var(index_of_term->search_term);
  if (index_of_term->from_index) {
    check_and_substitute_var(index_of_term->from_index);
  }
  visit_children_of(index_of_term);
}

void EquivClassRuleRunner::visitLastIndexOf(LastIndexOf_ptr last_index_of_term) {
  check_and_substitute_var(last_index_of_term->subject_term);
  check_and_substitute_var(last_index_of_term->search_term);
  if (last_index_of_term->from_index) {
    check_and_substitute_var(last_index_of_term->from_index);
  }
  visit_children_of(last_index_of_term);
}

void EquivClassRuleRunner::visitCharAt(CharAt_ptr char_at_term) {
  check_and_substitute_var(char_at_term->subject_term);
  check_and_substitute_var(char_at_term->index_term);
  visit_children_of(char_at_term);
}

void EquivClassRuleRunner::visitSubString(SubString_ptr sub_string_term) {
  check_and_substitute_var(sub_string_term->subject_term);
  check_and_substitute_var(sub_string_term->start_index_term);
  if (sub_string_term->end_index_term) {
    check_and_substitute_var(sub_string_term->end_index_term);
  }
  visit_children_of(sub_string_term);
}

void EquivClassRuleRunner::visitToUpper(ToUpper_ptr to_upper_term) {
  check_and_substitute_var(to_upper_term->subject_term);
  visit_children_of(to_upper_term);
}

void EquivClassRuleRunner::visitToLower(ToLower_ptr to_lower_term) {
  check_and_substitute_var(to_lower_term->subject_term);
  visit_children_of(to_lower_term);
}

void EquivClassRuleRunner::visitTrim(Trim_ptr trim_term) {
  check_and_substitute_var(trim_term->subject_term);
  visit_children_of(trim_term);
}

void EquivClassRuleRunner::visitToString(ToString_ptr to_string_term) {
  check_and_substitute_var(to_string_term->subject_term);
  visit_children_of(to_string_term);
}

void EquivClassRuleRunner::visitToInt(ToInt_ptr to_int_term) {
  check_and_substitute_var(to_int_term->subject_term);
  visit_children_of(to_int_term);
}

void EquivClassRuleRunner::visitReplace(Replace_ptr replace_term) {
  check_and_substitute_var(replace_term->subject_term);
  check_and_substitute_var(replace_term->search_term);
  check_and_substitute_var(replace_term->replace_term);
  visit_children_of(replace_term);

}

void EquivClassRuleRunner::visitCount(Count_ptr count_term) {
  check_and_substitute_var(count_term->subject_term);
  check_and_substitute_var(count_term->bound_term);
  visit_children_of(count_term);
}

void EquivClassRuleRunner::visitIte(Ite_ptr ite_term) {
}

void EquivClassRuleRunner::visitReConcat(ReConcat_ptr re_concat_term) {
}

void EquivClassRuleRunner::visitReUnion(ReUnion_ptr re_union_term) {
}

void EquivClassRuleRunner::visitReInter(ReInter_ptr re_inter_term) {
}

void EquivClassRuleRunner::visitReStar(ReStar_ptr re_star_term) {
}

void EquivClassRuleRunner::visitRePlus(RePlus_ptr re_plus_term) {
}

void EquivClassRuleRunner::visitReOpt(ReOpt_ptr re_opt_term) {
}

void EquivClassRuleRunner::visitToRegex(ToRegex_ptr to_regex_term) {
}

void EquivClassRuleRunner::visitUnknownTerm(Unknown_ptr unknown_term) {
}

void EquivClassRuleRunner::visitAsQualIdentifier(AsQualIdentifier_ptr as_qid_term) {
}

void EquivClassRuleRunner::visitQualIdentifier(QualIdentifier_ptr qi_term) {
}

void EquivClassRuleRunner::visitTermConstant(TermConstant_ptr term_constant) {
}

void EquivClassRuleRunner::visitIdentifier(Identifier_ptr identifier) {
}

void EquivClassRuleRunner::visitPrimitive(Primitive_ptr primitive) {
}

void EquivClassRuleRunner::visitTVariable(TVariable_ptr t_variable) {
}

void EquivClassRuleRunner::visitTBool(TBool_ptr t_bool) {
}

void EquivClassRuleRunner::visitTInt(TInt_ptr t_int) {
}

void EquivClassRuleRunner::visitTString(TString_ptr t_string) {
}

void EquivClassRuleRunner::visitVariable(Variable_ptr variable) {
}

void EquivClassRuleRunner::visitSort(Sort_ptr sort) {
}

void EquivClassRuleRunner::visitAttribute(Attribute_ptr attribute) {
}

void EquivClassRuleRunner::visitSortedVar(SortedVar_ptr sorted_var) {
}

void EquivClassRuleRunner::visitVarBinding(VarBinding_ptr var_binding) {
}


//Currently working only on variables.
bool EquivClassRuleRunner::check_and_substitute_var(Term_ptr& term) {
  if (Term::Type::QUALIDENTIFIER == term->type()) {
    Variable_ptr variable = symbol_table_->getVariable(term);
    Term_ptr subs_term = get_substitution_term(variable);
    if (subs_term != nullptr) {
      DVLOG(VLOG_LEVEL) << "apply rule: " << *variable << " (" << variable << ") -> " << *subs_term << " ("
                        << subs_term << " )";
      Term_ptr tmp_term = term;
      term = subs_term->clone();
      delete tmp_term;
      //add the rule we just followed to the symbol table if both are variables!

      auto subs_variable = symbol_table_->getVariable(subs_term);
      if (subs_variable) {
        symbol_table_->add_variable_substitution_rule(variable, subs_variable);
      }
      //If the substituted term is not a variable, then it is a constant!
      //TODO add solved value to the symbol table...
    }
  }
  return false;
}

Term_ptr EquivClassRuleRunner::get_substitution_term(Variable_ptr variable) {
  auto current_scope = symbol_table_->top_scope();
  auto it = substitution_table_[current_scope].find(variable);
  if (it == substitution_table_[current_scope].end()) {
    return nullptr;
  }
  return it->second;
}


bool EquivClassRuleRunner::has_optimization_rules() {
  for (auto& pair : substitution_table_) {
    if (not pair.second.empty()) {
      return true;
    }
  }
  return false;
}



} /* namespace Solver */
} /* namespace Vlab */
