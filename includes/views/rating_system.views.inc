<?php

/**
 * Implements hook_views_data.
 */
function rating_system_views_data() {
  $data['rating_score']['table']['group'] = t('Rating score');
  $data['rating_score']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'entity_id',
    ),
  );

  // The score field.
  $data['rating_score']['score'] = array(
    'title' => t('Score'),
    'help' => t('The score of an entity.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // The Rating formula reference.
  $data['rating_score']['rfid'] = array(
    'title' => t('Rating formula'),
    'help' => t('The rating formula reference.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_in_operator',
      'options callback' => 'rating_system_views_get_rating_formulas'
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}

function rating_system_views_get_rating_formulas() {
  $formulas = rating_formula_load();

  foreach ($formulas as $name => $formula) {
    $options[$formula->rfid] = $formula->label;
  }

  return $options;
}
