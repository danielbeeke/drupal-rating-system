<?php
/**
 * @file
 * All display related functions and hooks.
 */

/**
 * Implements hook_field_extra_fields().
 * Declare our extra fields to the field UI
 */
function rating_system_field_extra_fields() {
  $rating_formulas = rating_formula_load();

  if ($rating_formulas) {
    foreach ($rating_formulas as $rating_formula) {
      $extra[$rating_formula->entity_type][$rating_formula->entity_bundle] = array(
        'display' => array(
          'rating_system_' . $rating_formula->name . '_score' => array(
            'label' => $rating_formula->label . ' ' . t('(Rating system score)'),
            'weight' => 0,
          ),
          'rating_system_' . $rating_formula->name . '_percentage' => array(
            'label' => $rating_formula->label . ' ' . t('(Rating system percentage)'),
            'weight' => 0,
          ),
        )
      );
    }

    return $extra;
  }
}

/**
 * Implements hook_entity_load().
 */
function rating_system_entity_load($entities, $type) {
  // We have set this static inside get_entity_score.
  $rating_system_current_entities = &drupal_static('rating_system_current_entities');

  if ($type != 'rating_formula') {
    $entity_info = entity_get_info($type);

    $rating_formulas = rating_formula_load_multiple_by_entity_type_and_bundle($type);
    if ($rating_formulas) {
      foreach ($rating_formulas as $rating_formula) {
        foreach ($entities as $entity_id => $entity) {
          $go_set = FALSE;
          $reset = FALSE;

          // If our formule flushes on hook_entity_load we need to do it now.
          if ($rating_formula->flush == 1) {
            $reset = TRUE;
          }

          // Difference between entity types with multiple bundles and single bundles.
          if (!empty($entity_info['entity keys']['bundle']) && !isset($rating_system_current_entities[$type][$entity_id])) {
            if ($entity->{$entity_info['entity keys']['bundle']} == $rating_formula->entity_bundle) {
              $go_set = TRUE;
            }
          }
          else if (!isset($rating_system_current_entities[$type][$entity_id])) {
            $go_set = TRUE;
          }

          if ($go_set) {
            // Set the score in the entity.
            // For the current display we don't need to set a date time.
            // Emulations of current formulas and dates will be on a lower level.
            $score = rating_system_get_entity_formula_score($entity_id, $rating_formula, $reset);
            $min_max = rating_system_get_formula_min_max($rating_formula);

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

            $entity->{'rating_system_' . $rating_formula->name} = array(
              'score' => $score,
              'percentage' => $percentage,
            );
          }
        }
      }
    }
  }
}

/**
 * Implements hook_entity_view().
 */
function rating_system_entity_view($entity, $type, $view_mode, $langcode) {
  if ($type != 'rating_formula') {
    $entity_info = entity_get_info($type);

    // An entity type can have bundles but it isn't required.
    $bundle_key = isset($entity_info['bundle keys']['bundle']) ? $entity_info['bundle keys']['bundle'] : NULL;
    $bundle = isset($entity->{$bundle_key}) ? $entity->{$bundle_key} : NULL;

    $rating_formulas = rating_formula_load_multiple_by_entity_type_and_bundle($type, $bundle);
    if ($rating_formulas) {
      foreach ($rating_formulas as $rating_formula) {
        $score = $entity->{'rating_system_' . $rating_formula->name}['score'];
        $percentage = $entity->{'rating_system_' . $rating_formula->name}['percentage'];

        // Attach our extra field.
        $entity->content['rating_system_' . $rating_formula->name . '_score'] = array(
          '#type' => 'markup',
          '#markup' => '<span>' . $score . '</span>'
        );

        // Attach our extra field.
        $entity->content['rating_system_' . $rating_formula->name . '_percentage'] = array(
          '#type' => 'markup',
          '#markup' => '<span>' . $percentage . '</span>'
        );

      }
    }
  }
}

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
