<?php
/**
 * @file
 * Replace token hooks for the module statistics.
 */

/**
 * Implements hook_tokens for statistics.
 * Make it timespan aware.
 */
function rating_system_statistics_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);
  $replacements = array();

  if ($type == 'node' & !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      if ($name == 'total-count') {
        $statistics = rating_system_statistics_get($node);
        $replacements[$original] = $statistics['totalcount'];
      }
      elseif ($name == 'day-count') {
        $statistics = rating_system_statistics_get($node);
        $replacements[$original] = $statistics['daycount'];
      }
      elseif ($name == 'last-view') {
        $statistics = rating_system_statistics_get($node);
        $replacements[$original] = format_date($statistics['timestamp']);
      }
    }

    if ($created_tokens = token_find_with_prefix($tokens, 'last-view')) {
      $statistics = rating_system_statistics_get($node);
      $replacements += token_generate('date', $created_tokens, array('date' => $statistics['timestamp']), $options);
    }
  }

  return $replacements;
}

/**
 * Retrieves a node's "view statistics".
 *
 * @param $node
 *   A node object.
 *
 * @return
 *   An associative array containing:
 *   - totalcount: Integer for the total number of times the node has been
 *     viewed.
 *   - daycount: Integer for the total number of times the node has been viewed
 *     "today". For the daycount to be reset, cron must be enabled.
 *   - timestamp: Integer for the timestamp of when the node was last viewed.
 */
function rating_system_statistics_get($node) {
  if ($node->nid > 0) {
    // Retrieve an array with both totalcount and daycount.
    $result = db_query('SELECT totalcount, daycount, timestamp FROM {node_counter} WHERE nid = :nid AND timestamp >= :time_from AND timestamp <= :time_to',
      array(
        ':nid' => $node->nid,
        ':time_from' => $node->rating_system['time_from'],
        ':time_to' => $node->rating_system['time_to'],
      ), array('target' => 'slave'))->fetchAssoc();
  }

  if (!$result) {
    $result = array(
      'totalcount' => 0,
      'daycount' => 0,
      'timestamp' => 0
    );
  }

  return $result;
}
