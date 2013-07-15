<?php

/**
 * This file replaces all the hook_tokens with the ones 
 * provided inside includes/token_replacements.
 */

foreach (rating_system_get_token_replacements() as $replacement) {
  include_once('includes/token_replacements/rating_system.tokens.' . $replacement . '.inc');
}

/**
 * Returns all available token replacements.
 */
function rating_system_get_token_replacements() {
  $all_replacements = scandir(dirname(__FILE__) . '/includes/token_replacements');

  $result = array();

  unset($all_replacements[0]);
  unset($all_replacements[1]);

  foreach ($all_replacements as $replacement) {
  	$replacement_exploded = explode('.', $replacement);

  	$result[] = $replacement_exploded[2];
  }

  return $result;
}

/**
 * Implements hook_module_implements_alter().
 */
function rating_system_module_implements_alter(&$implementations, $hook) {
  if ($hook == 'tokens') {
	foreach (rating_system_get_token_replacements() as $replacement) {
	  $implementations[$replacement] = FALSE;
	}
  }
}

/**
 * Implements hook_tokens($type, $tokens, array $data = array(), array $options = array().
 */
function rating_system_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  foreach (rating_system_get_token_replacements() as $replacement) {
    $token_functions[] = 'rating_system_' . $replacement . '_tokens';
  }

  foreach ($token_functions as $token_function) {
  	$function_replacements = call_user_func($token_function, $type, $tokens, $data, $options);

  	$replacements = array_merge($function_replacements, $replacements);
  }

  return $replacements;
}
