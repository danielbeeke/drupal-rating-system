<?php

/**
 * @file
 * Rating System rating formulas form.
 */

/**
* Form for editing/adding rating formulas
*/
function rating_formula_form($form, $form_state, $rating_formula, $op = 'edit') {
  // Get all entity information
  $entity_info = entity_get_info();

  // No formulas on formulas.
  unset($entity_info['rating_formula']);

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
    '#description' => t('A unique machine-readable name for this rating formula. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['entity_type_and_bundle'] = array(
    '#type' => 'select',
    '#title' => t('Enitity bundle'),
    '#options' => $entity_tree,
    '#default_value' => $combined_entity_and_bundle,
    '#description' => t('The entity bundle you want to attach this rating formula to.'),
  );

  $form['calculation'] = array(
    '#title' => t('Calculation'),
    '#type' => 'textarea',
    '#element_validate' => array('rating_formula_calculation_validate'),
    '#default_value' => isset($rating_formula->calculation) ? $rating_formula->calculation : '',
  );

  $entity_type = isset($rating_formula->entity_type) ? $rating_formula->entity_type : NULL;

  $form['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array($entity_type),
    '#global_types' => TRUE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save rating formula'),
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

  $stripped_calculation = $form_state['values']['calculation'];

  $tokens = token_scan($stripped_calculation);

  foreach ($tokens as $token_type) {
    foreach ($token_type as $token => $token_key) {
      $stripped_calculation = str_replace($token_key, 2, $stripped_calculation);
    }
  }

  // Try to do some math
  // This PEAR class has a deprecated warning therefore we use @.
  // Add 0 + so we can use als oonly one token and no calculation.
  $validation = @$rpn->evaluate('0 + ' . $stripped_calculation, 'deg', FALSE);

  // Validate
  if (is_object($validation)) {
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

  // Create a new revision with every save.
  $rating_formula->revision = TRUE;

  // Set the changed time.
  $rating_formula->changed = REQUEST_TIME;

  // Save and go back.
  $rating_formula->save();
  // Redirect
  $form_state['redirect'] = 'admin/structure/rating-system/rating-formulas';
}
