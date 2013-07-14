<?php


/**
 * Main rating system function.
 * Returns a score for an entity.
 */
function rating_system_get_entity_formula_score($entity, $rating_formula) {
  $formula = $rating_formula->calculation;

  module_load_include('php', 'rating_system', 'lib/RPN');

  $rpn = new Math_Rpn();
  return $rpn->calculate($formula,'deg',false);
}