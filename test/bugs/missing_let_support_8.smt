(set-logic ALL)
(declare-const symb_contains String)
(assert ( str.contains symb_contains "an"))
(check-sat)
(get-model)
