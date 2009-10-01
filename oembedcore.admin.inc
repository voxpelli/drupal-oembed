<?php
// $Id$

/**
 * @file
 * Administrative functions for the oembed module.
 *
 * This provides the UI to list, create, edit and delete presets.
 */

/**
 * Output a list of presets.
 */
function oembedcore_list_preset($js = NULL) {
  $header = array(
    array('data' => t('Name'),    'class' => 'oembedcore-preset-name'),
    array('data' => t('Width'),   'class' => 'oembedcore-preset-width'),
    array('data' => t('Height'),  'class' => 'oembedcore-preset-height'),
    array('data' => t('Storage'), 'class' => 'oembedcore-preset-storage'),
  );

  $header[] = array('data' => t('Operations'), 'class' => 'oembedcore-preset-operations');

  $presets = oembedcore_preset_load_all();
  $rows = array();

  foreach ($presets as $preset) {
    $operations = array(
      array(
        'title' => t('Edit'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/edit',
      ),
      array(
        'title' => t('Export'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/export',
      ),
    );

    $rows[$preset->name] = array(
      'data' => array(
        'name' => array(
          'data'  => $preset->name,
          'class' => 'oembedcore-preset-name',
        ),
        'width' => array(
          'data'  => $preset->width,
          'class' => 'oembedcore-preset-width',
        ),
        'height' => array(
          'data'  => $preset->height,
          'class' => 'oembedcore-preset-height',
        ),
        'storage' => array(
          'data'  => ($preset->export_type == EXPORT_IN_CODE) ? t('In code') : t('In database'),
          'class' => 'oembedcore-preset-storage',
        ),
        'operations' => array(
          'data'  => theme('links', $operations),
          'class' => 'oembedcore-preset-operations',
        ),
      ),
      'class' => 'oembedcore-preset-' . $preset->name,
      //'title => ??
    );
  }

  $table = theme('table', $header, $rows, array('id' => 'oembedcore-list-preset'));

//  $operations = '<div id="oembedcore-links" class="links">' . theme('links', $pages['operations']) . '</div>';

//  drupal_add_css(drupal_get_path('module', 'page_manager') . '/css/page-manager.css');

  return $table;
}

/**
 * Handle the add preset page.
 */
function oembedcore_add_preset() {
  $preset = oembedcore_preset_new();
  drupal_set_title(t('Add preset')); //TODO: Isn't this redundant?
  return oembedcore_edit_preset($preset);
}

/**
 * Edit a preset.
 *
 * Called from both the add and edit points to provide for common flow.
 */
function oembedcore_edit_preset($preset) {
  if (!is_object($preset)) {
    $preset = oembedcore_preset_load($preset);
  }
  if ($preset) {
    drupal_set_title(check_plain($preset->name));
  }

  return drupal_get_form('oembedcore_edit_form_preset', $preset);
}

/**
 * Form to edit the settings of a preset.
 */
function oembedcore_edit_form_preset(&$form_state, $preset) {
  $form = array();

  $form['pid'] = array(
    '#type'  => 'value',
    '#value' => isset($preset->pid) ? $preset->pid : '',
  );
  $form['preset'] = array(
    '#type'  => 'value',
    '#value' => $preset,
  );

  $form['name'] = array(
    '#type'          => 'textfield',
    '#size'          => 24,
    '#default_value' => $preset->name,
    '#title'         => t('Preset name'),
    '#description'   => t('A unique name used to identify this preset internally. It must be only be alpha characters and underscores. No spaces, numbers or uppercase characters.'),
  );

  $form['width'] = array(
    '#type'          => 'textfield',
    '#size'          => 6,
    '#default_value' => $preset->width,
    '#title'         => t('Max width'),
    '#description'   => t('A maximum width in pixels of the embed or 0 for no maximum.'),
  );

  $form['height'] = array(
    '#type'          => 'textfield',
    '#size'          => 6,
    '#default_value' => $preset->height,
    '#title'         => t('Max height'),
    '#description'   => t('A maximum height in pixels of the embed or 0 for no maximum.'),
  );

  $label = empty($preset->pid) ? t('Save and proceed') : t('Save');
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => $label,
  );

  return $form;
}

/**
 * Validate submission of the preset edit form.
 */
function oembedcore_edit_form_preset_validate($form, &$form_state) {
  // Test uniqueness of name:
  if (!$form_state['values']['name']) {
    form_error($form['name'], t('Preset name is required.'));
  }
  else if (preg_match("/[^A-Za-z0-9_]/", $form_state['values']['name'])) {
    form_error($form['name'], t('Preset name must be alphanumeric or underscores only.'));
  }
  else {
    $query = "SELECT pid FROM {oembedcore_preset} WHERE name = '%s'";
    if (!empty($form_state['values']['pid']) && is_numeric($form_state['values']['pid'])) {
      $query .= ' AND pid != ' . $form_state['values']['pid'];
    }
    if (db_result(db_query($query, $form_state['values']['name']))) {
      form_error($form['name'], t('Preset name must be unique.'));
    }
  }

  if (!is_numeric($form_state['values']['width'])) {
    if (empty($form_state['values']['width'])) {
      form_set_value($form['width'], '0', $form_state);
    }
    else {
      form_error($form['width'], t('Preset width must be number.'));
    }
  }
  if (!is_numeric($form_state['values']['height'])) {
    if (empty($form_state['values']['height'])) {
      form_set_value($form['height'], '0', $form_state);
    }
    else {
      form_error($form['height'], t('Preset height must be number.'));
    }
  }
}

/**
 * Process submission of the mini panel edit form.
 */
function oembedcore_edit_form_preset_submit($form, &$form_state) {
  $preset = $form_state['values']['preset'];

  $preset->title  = $form_state['values']['title'];
  $preset->name   = $form_state['values']['name'];
  $preset->width  = (int)$form_state['values']['width'];
  $preset->height = (int)$form_state['values']['height'];

  if (empty($preset->pid)) {
    drupal_set_message(t('Your new preset %name has been saved.', array('%name' => $preset->name)));
    oembedcore_preset_save($preset);
    $form_state['values']['pid'] = $preset->pid;
    //TODO: Some redirect?
//    $form_state['redirect'] = "admin/build/panel-mini/$panel_mini->name/edit-context/next";
  }
  else {
    drupal_set_message(t('Your changes have been saved.'));
    oembedcore_preset_save($preset);
  }

  $form_state['redirect'] = 'admin/build/oembed';
}

/**
 * Page callback to export a preset to PHP code.
 */
function oembedcore_export_preset(&$form_state, $preset) {
  if (!is_object($preset)) {
    $preset = oembedcore_preset_load($preset);
  }
  drupal_set_title(check_plain($preset->name));
  $code = oembedcore_preset_export($preset);

  $lines = substr_count($code, "\n");
  $form['code'] = array(
    '#type'          => 'textarea',
    '#title'         => $preset->name,
    '#default_value' => $code,
    '#rows'          => $lines,
  );
  return $form;
}