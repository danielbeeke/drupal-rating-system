<?php

/**
 * Main rating system function.
 * Returns a score for an entity.
 * The function calling this one is responsible for using revisions when needed.
 */
function rating_system_get_entity_formula_score($entity, $rating_formula, $time_from = 0, $time_to = REQUEST_TIME) {
  $formula = $rating_formula->calculation;

  // Set the timespan, our token replacements use these values.
  $entity->rating_system = array(
  	'time_from' => $time_from,
  	'time_to' => $time_to
  );

  // Replace the tokens with real values.
  $formula = token_replace($formula, array(
  	$rating_formula->entity_type => $entity
  ));

  module_load_include('php', 'rating_system', 'lib/RPN');

  $rpn = new Math_Rpn();

  // Try to do some math
  // This PEAR class has a deprecated warning therefore we use @.
  // Add 0 + so we can use als oonly one token and no calculation.
  return @$rpn->calculate('0 + ' . $formula,'deg',false);
}