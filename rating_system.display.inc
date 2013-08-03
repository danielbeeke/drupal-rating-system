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
          'rating_system_' . $rating_formula->name => array(
            'label' => $rating_formula->label . ' ' . t('(Rating system)'),
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
  if ($type != 'rating_formula') {
    $entity_info = entity_get_info($type);

    $rating_formulas = rating_formula_load_multiple_by_entity_type_and_bundle($type);
    if ($rating_formulas) {
      foreach ($rating_formulas as $rating_formula) {
        foreach ($entities as $entity_id => $entity) {
          $go_set = FALSE;

          // Difference between entity types with multiple bundles and single bundles.
          if (!empty($entity_info['entity keys']['bundle'])) {
            if ($entity->{$entity_info['entity keys']['bundle']} == $rating_formula->entity_bundle) {
              $go_set = TRUE;
            }
          }
          else {
            $go_set = TRUE;
          }

          // Set the score in the entity.
          // For the current display we don't need to set a date time.
          // Emulations of current formulas and dates will be on a lower level.
          $entity->{'rating_system_' . $rating_formula->name} = rating_system_get_entity_formula_score($entity_id, $rating_formula);
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
        $score = $entity->{'rating_system_' . $rating_formula->name};

        // Attach our extra field.
        $entity->content['rating_system_' . $rating_formula->name] = array(
          '#type' => 'markup',
          '#markup' => '<span>' . $score . '</span>'
        );
      }
    }
  }
}
