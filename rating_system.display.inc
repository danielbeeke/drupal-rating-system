<?php

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
        foreach ($entities as $entity) {
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
          $entity->{'rating_system_' . $rating_formula->name} = rating_system_get_entity_formula_score($entity, $rating_formula);
        }
      }
    }
  }  
}

/**
 * Implements hook_entity_view_alter().
 * Attach our extra fields to the display.
 */
function rating_system_entity_view_alter(&$build, $type) {
  if ($type != 'rating_formula') {
    $entity_info = entity_get_info($type);

    $rating_formulas = rating_formula_load_multiple_by_entity_type_and_bundle($type, $build['#bundle']);
    if ($rating_formulas) {
      foreach ($rating_formulas as $rating_formula) {
        $key = rating_system_get_entity_render_key($type);
        $score = $build['#' . $key]->{'rating_system_' . $rating_formula->name};

        // Attach our extra field.
        $build['rating_system_' . $rating_formula->name] = array(
          '#type' => 'markup',
          '#markup' => '<span>' . $score . '</span>'
        );
      }
    }
  }  
}

/**
 * Returns the entity render array key name by entity name.
 * TODO discover why this is needed.
 * Or where I am making a mistake.
 *
 * This is build upon the idea that I need to add
 * values to an entity on hook_entity_load and add the theme functions
 * on hook_entity_view_alter.
 * This way the scores are in the load and the theming happens in the view.
 */
function rating_system_get_entity_render_key($entity_type) {
  $keys = array(
    'node' => 'node',
    'user' => 'account',
  );

  if (isset($keys[$entity_type])) {
    return $keys[$entity_type];
  }
  else {
    return 'content';
  }
}
