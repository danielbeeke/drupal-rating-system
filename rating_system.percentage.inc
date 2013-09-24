<?php
/**
* @file
* The file with functions to give percentage percentage for the rating system module.
*/

/**
 * Returns the min and max for a formula.
 */
function rating_system_get_formula_min_max($rating_formula) {
  $query =  db_select('rating_score', 's');
  $query->condition('rfid', $rating_formula->rfid, '=');
  $query->condition('entity_type', $rating_formula->entity_type, '=');
  $query->addExpression('MAX(score)', 'max_score');
  $query->range(0, 1);

  $max = $query->execute()->fetchField();

  $query =  db_select('rating_score', 's');
  $query->condition('rfid', $rating_formula->rfid, '=');
  $query->condition('entity_type', $rating_formula->entity_type, '=');
  $query->addExpression('MIN(score)', 'min_score');
  $query->range(0, 1);

  $min = $query->execute()->fetchField();

  if ($min == 0 || $max == 0) {
    return;
  }

  return array(
    'min' => $min,
    'max' => $max
  );
}


/**
 * Returns a percentage for an entity.
 */
function rating_system_get_entity_formula_percentage($entity_id, $rating_formula, $reset) {
  $rating_system_cached_percentages = &drupal_static(__FUNCTION__);
  if (!isset($rating_system_cached_percentages[$entity_id]) || $reset) {
    $cached = rating_system_get_cached_entity_formula_percentage($entity_id, $rating_formula);

    if ($cached !== 0 && !$reset) {
      $rating_system_cached_percentages[$rating_formula->rfid][$entity_id] = $cached;
    }
    else {
      // Calculate percentage.
      $min_max = rating_system_get_formula_min_max($rating_formula);

      if ($min_max) {
        $score = rating_system_get_entity_formula_score($entity_id, $rating_formula, $reset);

        if ($score == $min_max['min']) {
          $percentage = 0;
        } else if ($score == $min_max['max']) {
          $percentage = 100;
        }
        else {
          $delta = $min_max['max'] - $min_max['min'];
          $a = $score - $min_max['min'];
          $b = $delta / $a;
          $percentage = 100 / $b;
        }

        // Rounding.
        $percentage = round($percentage);

        $rating_system_cached_percentages[$rating_formula->rfid][$entity_id] = $percentage;

        rating_system_set_cached_entity_formula_percentage($entity_id, $rating_formula, $percentage);
      }
    }
  }

  return $rating_system_cached_percentages[$rating_formula->rfid][$entity_id];
}

/**
 * Returns a cached percentage for an entity.
 */
function rating_system_get_cached_entity_formula_percentage($entity_id, $rating_formula) {
  $percentage = db_select('rating_percentage', 's')
  ->fields('s', array('percentage'))
  ->condition('rfid', $rating_formula->rfid, '=')
  ->condition('entity_id', $entity_id, '=')
  ->condition('entity_type', $rating_formula->entity_type, '=')
  ->execute()
  ->fetchField();

  return $percentage;
}

/**
 * Set a cached percentage for an entity.
 */
function rating_system_set_cached_entity_formula_percentage($entity_id, $rating_formula, $percentage) {
  $record = array(
    'rfid' => $rating_formula->rfid,
    'entity_type' => $rating_formula->entity_type,
    'entity_id' => $entity_id,
    'percentage' => $percentage,
    'timestamp' => REQUEST_TIME
  );

  $old_record = rating_system_get_cached_entity_formula_percentage($entity_id, $rating_formula);

  // Update the old record.
  if ($old_record !== 0) {
    drupal_write_record('rating_percentage', $record, array('rfid', 'entity_type', 'entity_id'));
  }

  // Or insert a new one.
  else {
    drupal_write_record('rating_percentage', $record);
  }

  return $percentage;
}

/**
 * Flushes a cached percentage for an entity.
 */
function rating_system_flush_cached_entity_formula_percentage($entity_id, $rating_formula) {
  $rows_deleted = db_delete('rating_percentage')
  ->condition('rfid', $rating_formula->rfid, '=')
  ->condition('entity_id', $entity_id, '=')
  ->condition('entity_type', $rating_formula->entity_type, '=')
  ->execute();

  return $rows_deleted;
}
