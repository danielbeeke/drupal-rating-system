<?php
/**
 * @file
 * Main rating system function.
 * Returns a score for an entity.
 * The function calling this one is responsible for using revisions when needed.
 */

/**
 * Returns a score for an entity.
 */
function rating_system_get_entity_formula_score($entity_id, $rating_formula, $reset) {
  $rating_system_cached_scores = &drupal_static(__FUNCTION__);
  if (!isset($rating_system_cached_scores[$entity_id]) || $reset) {
    $cached = rating_system_get_cached_entity_formula_score($entity_id, $rating_formula);

    if (!empty($cached) && !$reset) {
      $rating_system_cached_scores[$rating_formula->rfid][$entity_id] = $cached;
    }
    else {
      $formula = $rating_formula->calculation;

      // We check this static inside rating_system_entity_load so there will be no recursive shit.
      $rating_system_current_entities = &drupal_static('rating_system_current_entities');
      $rating_system_current_entities[$rating_formula->entity_type][$entity_id] = $entity_id;

      // Load the entity.
      $entities = entity_load($rating_formula->entity_type, array($entity_id));

      $entity = end($entities);

      // Replace the tokens with real values.
      $formula = token_replace($formula, array(
        $rating_formula->entity_type => $entity
      ));

      module_load_include('php', 'rating_system', 'lib/RPN');

      $rpn = new Math_Rpn();

      // Try to do some math
      // This PEAR class has a deprecated warning therefore we use @.
      // Add 0 + so we can use one token and no calculation.
      $score = @$rpn->calculate('0 + ' . $formula, 'deg', FALSE);

      $rating_system_cached_scores[$rating_formula->rfid][$entity_id] = $score;

      rating_system_set_cached_entity_formula_score($entity_id, $rating_formula, $score);
    }
  }

  return $rating_system_cached_scores[$rating_formula->rfid][$entity_id];
}

/**
 * Returns a cached score for an entity.
 */
function rating_system_get_cached_entity_formula_score($entity_id, $rating_formula) {
  $score = db_select('rating_score', 's')
  ->fields('s', array('score'))
  ->condition('rfid', $rating_formula->rfid, '=')
  ->condition('entity_id', $entity_id, '=')
  ->condition('entity_type', $rating_formula->entity_type, '=')
  ->execute()
  ->fetchField();

  return $score;
}

/**
 * Set a cached score for an entity.
 */
function rating_system_set_cached_entity_formula_score($entity_id, $rating_formula, $score) {
  $record = array(
    'rfid' => $rating_formula->rfid,
    'entity_type' => $rating_formula->entity_type,
    'entity_id' => $entity_id,
    'score' => $score,
    'timestamp' => REQUEST_TIME
  );

  // Update the old record.
  if (rating_system_get_cached_entity_formula_score($entity_id, $rating_formula)) {
    drupal_write_record('rating_score', $record, array('rfid', 'entity_type', 'entity_id'));
  }

  // Or insert a new one.
  else {
    drupal_write_record('rating_score', $record);
  }

  return $score;
}

/**
 * Flushes a cached score for an entity.
 */
function rating_system_flush_cached_entity_formula_score($entity_id, $rating_formula) {
  $rows_deleted = db_delete('rating_score')
  ->condition('rfid', $rating_formula->rfid, '=')
  ->condition('entity_id', $entity_id, '=')
  ->condition('entity_type', $rating_formula->entity_type, '=')
  ->execute();

  return $rows_deleted;
}
