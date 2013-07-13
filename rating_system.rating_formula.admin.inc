<?php

/**
 * @file
 * Rating System rating groups form.
 */

/**
* Form for editing/adding rating groups
*/
function rating_formula_form($form, $form_state, $rating_formula, $op = 'edit') {
  // Get all entity information
  $entity_info = entity_get_info();

  // You can not rate a rating group entity.
  foreach($entity_info as $entity_key => $entity) {
    if($entity_key == 'rating_formula') {
      unset($entity_info[$entity_key]);
    }
  }

  // Let's make some options for the select widget
  foreach ($entity_info as $entity_name => $entity) {
    foreach ($entity['bundles'] as $bundle_name => $bundle) {
      // This array will be used for the select with optgroups
      $entity_tree[$entity_name][$entity_name . '|' . $bundle_name] = $bundle['label'];
    }
  }

  // Setting the default value for entity_type_and_bundle
  if (isset($rating_formula->entity_type) && isset($rating_formula->entity_bundle)) {
    $combined_entity_and_bundle = $rating_formula->entity_type . '|' . $rating_formula->entity_bundle;
  }
  else {
    $combined_entity_and_bundle = '';
  }

  $form['label'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => isset($rating_formula->label) ? $rating_formula->label : '',
  );

  $form['name'] = array(
    '#title' => t('System name'),
    '#type' => 'machine_name',
    '#default_value' => isset($rating_formula->name) ? $rating_formula->name : '',
    '#disabled' => $rating_formula->isLocked(),
    '#machine_name' => array(
      'exists' => 'rating_formula_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this rating group. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['entity_type_and_bundle'] = array(
    '#type' => 'select',
    '#title' => t('Enitity bundle'),
    '#options' => $entity_tree,
    '#default_value' => $combined_entity_and_bundle,
    '#description' => t('The entity bundle you want to attach this rating group to.'),
  );

  $form['calculation'] = array(
    '#title' => t('Calculation'),
    '#type' => 'textarea',
    '#element_validate' => array('rating_formula_calculation_validate'),
    '#default_value' => isset($rating_formula->calculation) ? $rating_formula->calculation : '',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save rating group'),
  );

  return $form;
}

/**
* Formule validation.
*/
function rating_formula_calculation_validate($element, &$form_state, $form) {
  // Pear library
  module_load_include('php', 'rating_system', 'lib/RPN');

  $rpn = new Math_Rpn();

  // Try to do some math
  // This PEAR class has a deprecated warning therefore we use @.
  $validation = @$rpn->evaluate($form_state['values']['calculation'], 'deg', FALSE);

  // Validate
  if(is_object($validation)) {
    form_set_error('calculation', t('Could not parse the calculation.'));
  }
}

/**
* Form API submit callback for the type form.
*/
function rating_formula_form_submit(&$form, &$form_state) {
  // Split values of entity_type_and_bundle
  $exploded = explode('|', $form_state['values']['entity_type_and_bundle']);
  $form_state['values']['entity_type'] = $exploded[0];
  $form_state['values']['entity_bundle'] = $exploded[1];

  // Fill the object
  $rating_formula = entity_ui_form_submit_build_entity($form, $form_state);

  // Set the created time.
  if (!isset($rating_formula->created)) {
    $rating_formula->created = REQUEST_TIME;
  }

  // Set the changed time.
  $rating_formula->changed = REQUEST_TIME;

  // Save and go back.
  $rating_formula->save();
  // Redirect
  $form_state['redirect'] = 'admin/structure/rating-system/rating-groups';
}
