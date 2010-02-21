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
    $operations = array();

    if (empty($preset->disabled)) {
      $operations[] = array(
        'title' => t('Edit'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/edit',
      );
      $operations[] = array(
        'title' => t('Export'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/export',
      );
    }

    if ($preset->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
      $operations[] = array(
        'title' => t('Revert'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/delete',
      );
    }
    elseif ($preset->export_type != EXPORT_IN_CODE) {
      $operations[] = array(
        'title' => t('Delete'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/delete',
      );
    }
    elseif (empty($preset->disabled)) {
      $operations[] = array(
        'title' => t('Disable'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/disable',
        'query' => drupal_get_destination(),
      );
    }
    else {
      $operations[] = array(
        'title' => t('Enable'),
        'href'  => 'admin/build/oembed/' . $preset->name . '/enable',
        'query' => drupal_get_destination(),
      );
    }

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
      'class' => 'oembedcore-preset-' . $preset->name . (!empty($preset->disabled) ? ' oembedcore-preset-disabled' : ''),
      //'title => ??
    );
  }

  $table = theme('table', $header, $rows, array('id' => 'oembedcore-list-preset'));

//  $operations = '<div id="oembedcore-links" class="links">' . theme('links', $pages['operations']) . '</div>';

  drupal_add_css(drupal_get_path('module', 'oembedcore') . '/oembedcore.admin.css');

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

  $form['disable_title'] = array(
    '#type'          => 'checkbox',
    '#default_value' => !empty($preset->conf['disable_title']),
    '#title'         => t('Disable title when other embeddable data is available'),
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
  elseif (preg_match("/[^A-Za-z0-9_]/", $form_state['values']['name'])) {
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
  elseif ($form_state['values']['width'] < 0) {
    form_error($form['width'], t('Preset width must be a non-negative number.'));
  }
  if (!is_numeric($form_state['values']['height'])) {
    if (empty($form_state['values']['height'])) {
      form_set_value($form['height'], '0', $form_state);
    }
    else {
      form_error($form['height'], t('Preset height must be number.'));
    }
  }
  elseif ($form_state['values']['height'] < 0) {
    form_error($form['height'], t('Preset height must be a non-negative number.'));
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

  $preset->conf = array(
    'disable_title' => !empty($form_state['values']['disable_title']),
  );

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

/**
 * Provide a form to confirm deletion of a preset.
 */
function oembedcore_delete_confirm_preset(&$form_state, $preset) {
  if (!is_object($preset)) {
    $preset = oembedcore_preset_load($preset);
  }
  if ($preset->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
    $submit = t('Revert');
  }
  elseif ($preset->export_type != EXPORT_IN_CODE) {
    $submit = t('Delete');
  }
  else {
    drupal_not_found();
    die;
  }
  $form['name'] = array('#type' => 'value', '#value' => $preset->name);
  $form['pid']  = array('#type' => 'value', '#value' => $preset->pid);
  return confirm_form($form,
    t('Are you sure you want to delete the preset "@name"?', array('@name' => $preset->name)),
    !empty($_GET['destination']) ? $_GET['destination'] : 'admin/build/oembed',
    t('This action cannot be undone.'),
    $submit, t('Cancel')
  );
}

/**
 * Handle the submit button to delete a mini panel.
 */
function oembedcore_delete_confirm_preset_submit($form, &$form_state) {
  $preset = oembedcore_preset_load($form_state['values']['name']);
  if ($preset->pid == $form_state['values']['pid']) {
    oembedcore_preset_delete($preset);
    $form_state['redirect'] = 'admin/build/oembed';
  }
}

/**
 * Enable a default preset.
 */
function oembedcore_enable_preset($preset) {
  if (!is_object($preset)) {
    $preset = oembedcore_preset_load($preset);
  }
  ctools_include('export');
  ctools_export_set_status('oembedcore_preset', $preset->name, FALSE);
  //TODO: Extract this and move to oembedfield
  if (module_exists('content')) {
    content_clear_type_cache();
    drupal_rebuild_theme_registry();
  }
  drupal_goto();
}

/**
 * Disable a default preset.
 */
function oembedcore_disable_preset($preset) {
  if (!is_object($preset)) {
    $preset = oembedcore_preset_load($preset);
  }
  ctools_include('export');
  ctools_export_set_status('oembedcore_preset', $preset->name, TRUE);
  //TODO: Extract this and move to oembedfield
  if (module_exists('content')) {
    content_clear_type_cache();
    drupal_rebuild_theme_registry();
  }
  drupal_goto();
}