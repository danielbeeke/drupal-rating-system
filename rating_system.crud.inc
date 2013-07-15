<?php
/**
 * @file
 * CRUD functions for the rating system module.
 */

/**
 * Implements hook_entity_info().
 */
function rating_system_entity_info() {
  $return = array(
    'rating_formula' => array(
      'label' => t('Rating formula'),
      'plural label' => t('Rating formulas'),
      'description' => t('A Rating formula'),
      'entity class' => 'RatingFormula',
      'controller class' => 'EntityAPIControllerExportable',
      'views controller class' => 'EntityDefaultViewsController',
      'base table' => 'rating_formula',
      'revision table' => 'rating_formula_revision',
      'exportable' => TRUE,
        'module' => 'rating_system',
      'entity keys' => array(
        'id' => 'rfid',
        'name' => 'name',
        'label' => 'label',
        'revision' => 'vid'
      ),
      'admin ui' => array(
        'path' => 'admin/structure/rating-system/rating-formulas',
        'file' => 'rating_system.rating_formula.admin.inc',
      ),
      'access callback' => 'rating_formula_access',
      'access arguments' => array('administer rating formulas'),
    ),
  );

  return $return;
}

/**
 * Use a separate class for Rating formulas so we can specify some defaults
 * modules may alter.
 */
class RatingFormula extends Entity {

  /**
   * Returns whether the Rating formula is locked, thus may not be deleted or renamed.
   *
   * Rating formulas provided in code are automatically treated as locked, as well
   * as any fixed Rating formula.
   */
  public function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}

/**
* Gets an array of all Rating formulas, keyed by the type name.
*
* @param $rating_formula_name
*   May be a formula name.
* @return RatingFormula[]
*   Depending whether $type isset, an array of Rating formulas or a single one.
*/
function rating_formula_load($rating_formula_name = NULL) {
  $rating_formulas = entity_load_multiple_by_name('rating_formula', isset($rating_formula_name) ? array($rating_formula_name) : FALSE);
  return isset($rating_formula_name) ? reset($rating_formulas) : $rating_formulas;
}

/**
* Gets an array of all Rating formulas, keyed by the type name.
*
* @param $entity_type
*   Entity type.
* @param $entity_bundle
*   Entity bundle.
* @return RatingFormula[]
*   Depending whether $type isset, an array of Rating formulas or a single one.
*/
function rating_formula_load_multiple_by_entity_type_and_bundle($entity_type = NULL, $entity_bundle = NULL) {
  // Set conditions.
  $conditions = array();
  if (isset($entity_type)) {
    $conditions['entity_type'] = $entity_type;
  }
  if (isset($entity_bundle)) {
    $conditions['entity_bundle'] = $entity_bundle;
  }

  // I know this is deprecated.
  // When using entityFieldQuery hook_entity_load takes forever.
  $rating_formulas = entity_load_multiple_by_name('rating_formula', FALSE, $conditions);

  return $rating_formulas;
}
