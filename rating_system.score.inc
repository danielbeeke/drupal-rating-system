<?php


/**
 * Main rating system function.
 * Returns a score for an entity.
 * The function calling this one is responsible for using revisions when needed.
 */
function rating_system_get_entity_formula_score($entity, $rating_formula) {
  $formula = $rating_formula->calculation;

  // Replace the tokens with real values.
  $formula = token_replace($formula, array(
  	$rating_formula->entity_type => $entity
  ));

  module_load_include('php', 'rating_system', 'lib/RPN');

  $rpn = new Math_Rpn();
  return $rpn->calculate($formula,'deg',false);
}