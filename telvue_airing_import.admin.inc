<?php

/**
 * Callback for module settings page.
 */
function telvue_airing_import_settings_form($form, &$form_state) {
  $form = array();

  $form['telvue_airing_import']['telvue_airing_import_xml_feed_url_1'] = array(
    '#type' => 'textfield',
    '#title' => t('XML Feed/Source URL 1'),
    '#default_value' => variable_get('telvue_airing_import_xml_feed_url_1', ''),
    '#description' => 'Set the base url from which to retrieve the airings xml feed.',
    '#size' => 120,
  );

  $form['telvue_airing_import']['telvue_airing_import_xml_feed_url_2'] = array(
    '#type' => 'textfield',
    '#title' => t('XML Feed/Source URL 2'),
    '#default_value' => variable_get('telvue_airing_import_xml_feed_url_2', ''),
    '#description' => 'Set the base url from which to retrieve the airings xml feed.',
    '#size' => 120,
  );

  $form['telvue_airing_import']['telvue_airing_import_xml_feed_url_3'] = array(
    '#type' => 'textfield',
    '#title' => t('XML Feed/Source URL 3'),
    '#default_value' => variable_get('telvue_airing_import_xml_feed_url_3', ''),
    '#description' => 'Set the base url from which to retrieve the airings xml feed.',
    '#size' => 120,
  );

  $form['telvue_airing_import']['telvue_airing_import_xml_feed_url_4'] = array(
    '#type' => 'textfield',
    '#title' => t('XML Feed/Source URL 4'),
    '#default_value' => variable_get('telvue_airing_import_xml_feed_url_4', ''),
    '#description' => 'Set the base url from which to retrieve the airings xml feed.',
    '#size' => 120,
  );

  return system_settings_form($form);
}

/**
 * Delete all Form
 */
function telvue_airing_import_delete_all_form($form, &$form_state) {
  $form['telvue_airing_import_message'] = array (
    '#type' => 'item',
    '#markup' => 'Clicking this button will delete **ALL** Airing entities and related field data.',
  );

  $form['telvue_airing_import_delete_all'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Delete all Form Submit
 */
function telvue_airing_import_delete_all_form_submit($form, &$form_state) {
  // Get count of shows for max count
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'airing');

  // Get max count
  $max_count = $query->count()->execute();

  if ($max_count > 0) {
    $form_values = array(
  	  'max' => $max_count,
    );

    $operations[] = array('telvue_airing_import_delete_all_batch_op', array($form_values));

    $batch = array(
      'operations' => $operations,
      'finished' => 'telvue_airing_import_delete_all_batch_finished',
      'title' => t("Delete All Airings Batch"),
      'init_message' => t('Deleteing Airings'),
      'error_message' => t('Error!'),
      // This is needed b/c our batch process is not in the .module file.
      'file' => drupal_get_path('module', 'telvue_airing_import') . '/telvue_airing_import.admin.inc'
    );

    batch_set($batch);
  }
  else {
    drupal_set_message(t('There are airings to delete.'));
  }
}

/**
 * Batch op to delete all airings.
 */
function telvue_airing_import_delete_all_batch_op($form_values, &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_airing'] = 0;
    $context['sandbox']['max'] = $form_values['max'];
  }
  // Number of airings to process in each batch op
  $limit = 50;

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'airing');
      $query->propertyCondition('airing_id', $context['sandbox']['current_airing'], '>');
      $query->range(0, $limit);
    $result = $query->execute();

    if (isset($result['airing'])) {
      $airing_ids = array_keys($result['airing']);
      // Load airings
      $airings = cm_airing_load_multiple($airing_ids);

      // Iterate over airings and delete.
      foreach ($airings as $airing) {
        // Delete airing
        cm_airing_delete($airing);

        // Update context vars
        $context['results'][] = $airing->airing_id . ' : ' . check_plain($airing->airing_id);
        $context['sandbox']['progress']++;
        $context['sandbox']['current_airing'] = $airing->airing_id;
        $context['message'] = 'Processed ' . $context['sandbox']['progress'] . ' of ' . $context['sandbox']['max'] . ' airings.';
      }

    }
  }
  else {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch finished delete all airings.
 */
function telvue_airing_import_delete_all_batch_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('All airings and related field data deleted! Really hope you intended to do this. :/'));
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(
      t('An error occurred while processing @operation with arguments : @args',
        array(
          '@operation' => $error_operation[0],
          '@args' => print_r($error_operation[0], TRUE),
        )
      )
    );
  }
}

/**
 * Import all Form
 */
function telvue_airing_import_import_all_form($form, &$form_state) {
  $form['telvue_airing_import_all_message'] = array (
    '#type' => 'item',
    '#markup' => 'Clicking this button will import all Airings.',
  );

  $form['telvue_airing_import_all'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Import all Form Submit
 */
function telvue_airing_import_import_all_form_submit($form, &$form_state) {
  // Get all telvue_airing_import feed sources from elysia_cron table.
  $jobs = db_select('elysia_cron', 'ec')
    ->fields('ec', array('name'))
    ->condition('name', '%' . db_like('telvue_airing_import') . '%', 'LIKE')
    ->execute()
    ->fetchAll();

  // For each job that matches, run cron.
  // @todo this will probably time out if there are a lot of feeds. What to do?
  if (isset($jobs)) {
    foreach ($jobs as $job) {
      $cron = elysia_cron_run_job($job->name, $ignore_time = TRUE, $ignore_disable = TRUE);
    }
  }

  drupal_set_message('All airings have been imported!');
}
