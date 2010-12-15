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
        'href'  => 'admin/build/oembed/preset/' . $preset->name . '/edit',
      );
      $operations[] = array(
        'title' => t('Export'),
        'href'  => 'admin/build/oembed/preset/' . $preset->name . '/export',
      );
    }

    if ($preset->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
      $operations[] = array(
        'title' => t('Revert'),
        'href'  => 'admin/build/oembed/preset/' . $preset->name . '/delete',
      );
    }
    elseif ($preset->export_type != EXPORT_IN_CODE) {
      $operations[] = array(
        'title' => t('Delete'),
        'href'  => 'admin/build/oembed/preset/' . $preset->name . '/delete',
      );
    }
    elseif (empty($preset->disabled)) {
      $operations[] = array(
        'title' => t('Disable'),
        'href'  => 'admin/build/oembed/preset/' . $preset->name . '/disable',
        'query' => drupal_get_destination(),
      );
    }
    else {
      $operations[] = array(
        'title' => t('Enable'),
        'href'  => 'admin/build/oembed/preset/' . $preset->name . '/enable',
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
    '#description'   => t('A unique machine-readable name used to identify this preset internally. It may only contain lowercase alphanumeric characters and underscores. No spaces or uppercase characters.'),
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
  $values = $form_state['values'];
  // Test uniqueness of name:
  if (!$values['name']) {
    form_error($form['name'], t('Preset name is required.'));
  }
  elseif (preg_match("/[^a-z0-9_]/", $values['name'])) {
    form_error($form['name'], t('The name may only contain lowercase alphanumeric characters and underscores.'));
  }
  else {
    $query = "SELECT pid FROM {oembedcore_preset} WHERE name = '%s'";
    if (!empty($values['pid']) && is_numeric($values['pid'])) {
      $query .= ' AND pid != ' . $values['pid'];
    }
    if (db_result(db_query($query, $values['name']))) {
      form_error($form['name'], t('Preset name must be unique.'));
    }
  }

  if (!is_numeric($values['width'])) {
    if (empty($values['width'])) {
      form_set_value($form['width'], '0', $form_state);
    }
    else {
      form_error($form['width'], t('Preset width must be number.'));
    }
  }
  elseif ($values['width'] < 0) {
    form_error($form['width'], t('Preset width must be a non-negative number.'));
  }
  if (!is_numeric($values['height'])) {
    if (empty($values['height'])) {
      form_set_value($form['height'], '0', $form_state);
    }
    else {
      form_error($form['height'], t('Preset height must be number.'));
    }
  }
  elseif ($values['height'] < 0) {
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

  $form_state['redirect'] = 'admin/build/oembed/preset';
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
    return;
  }
  $form['name'] = array('#type' => 'value', '#value' => $preset->name);
  $form['pid']  = array('#type' => 'value', '#value' => $preset->pid);
  return confirm_form($form,
    t('Are you sure you want to delete the preset "@name"?', array('@name' => $preset->name)),
    !empty($_GET['destination']) ? $_GET['destination'] : 'admin/build/oembed/preset',
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
    $form_state['redirect'] = 'admin/build/oembed/preset';
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

/**
 * Provider functions
 */

/**
 * Output a list of provider.
 */
function oembedcore_list_provider($js = NULL) {
  $header = array(
    array('data' => t('Title'),      'class' => 'oembedcore-provider-title'),
    array('data' => t('Storage'),    'class' => 'oembedcore-provider-storage'),
    array('data' => t('Operations'), 'class' => 'oembedcore-provider-operations')
  );

  $providers = oembedcore_provider_load_all();
  $rows = array();

  foreach ($providers as $provider) {
    $operations = array();

    if (empty($provider->disabled)) {
      $operations[] = array(
        'title' => t('Edit'),
        'href'  => 'admin/build/oembed/provider/' . $provider->name . '/edit',
      );
      $operations[] = array(
        'title' => t('Export'),
        'href'  => 'admin/build/oembed/provider/' . $provider->name . '/export',
      );
    }

    if ($provider->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
      $operations[] = array(
        'title' => t('Revert'),
        'href'  => 'admin/build/oembed/provider/' . $provider->name . '/delete',
      );
    }
    elseif ($provider->export_type != EXPORT_IN_CODE) {
      $operations[] = array(
        'title' => t('Delete'),
        'href'  => 'admin/build/oembed/provider/' . $provider->name . '/delete',
      );
    }
    elseif (empty($provider->disabled)) {
      $operations[] = array(
        'title' => t('Disable'),
        'href'  => 'admin/build/oembed/provider/' . $provider->name . '/disable',
        'query' => drupal_get_destination(),
      );
    }
    else {
      $operations[] = array(
        'title' => t('Enable'),
        'href'  => 'admin/build/oembed/provider/' . $provider->name . '/enable',
        'query' => drupal_get_destination(),
      );
    }

    $rows[$provider->name] = array(
      'data' => array(
        'name' => array(
          'data'  => $provider->title,
          'class' => 'oembedcore-provider-title',
        ),
        'storage' => array(
          'data'  => ($provider->export_type == EXPORT_IN_CODE) ? t('In code') : t('In database'),
          'class' => 'oembedcore-provider-storage',
        ),
        'operations' => array(
          'data'  => theme('links', $operations),
          'class' => 'oembedcore-provider-operations',
        ),
      ),
      'class' => 'oembedcore-provider-' . $provider->name . (!empty($provider->disabled) ? ' oembedcore-provider-disabled' : ''),
    );
  }

  $table = theme('table', $header, $rows, array('id' => 'oembedcore-list-providers'));

  drupal_add_css(drupal_get_path('module', 'oembedcore') . '/oembedcore.admin.css');

  return $table;
}

/**
 * Handle the add provider page.
 */
function oembedcore_add_provider() {
  $provider = oembedcore_provider_new();
  drupal_set_title(t('Add provider'));
  return oembedcore_edit_provider($provider);
}

/**
 * Edit a provider.
 *
 * Called from both the add and edit points to provide for common flow.
 */
function oembedcore_edit_provider($provider) {
  if (!is_object($provider)) {
    $provider = oembedcore_provider_load($provider);
  }
  if ($provider) {
    drupal_set_title(check_plain($provider->name));
  }

  return drupal_get_form('oembedcore_edit_form_provider', $provider);
}

/**
 * Form to edit the settings of a provider.
 */
function oembedcore_edit_form_provider(&$form_state, $provider) {
  $form = array();

  $form['pid'] = array(
    '#type'  => 'value',
    '#value' => isset($provider->pid) ? $provider->pid : '',
  );
  $form['provider'] = array(
    '#type'  => 'value',
    '#value' => $provider,
  );

  $form['name'] = array(
    '#type'          => 'textfield',
    '#size'          => 24,
    '#default_value' => $provider->name,
    '#title'         => t('Provider name'),
    '#description'   => t('A unique machine-readable name used to identify this provider internally. It may only contain lowercase alphanumeric characters and underscores. No spaces or uppercase characters.'),
  );

  $form['title'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Title'),
    '#description'   => t('A human-readable title for the provider.'),
    '#size'          => 32,
    '#maxlength'     => 255,
    '#required'      => TRUE,
    '#default_value' => $provider->title,
  );

  $form['endpoint'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Endpoint'),
    '#description'   => t('The endpoint where OEmbed requests are going to be sent.'),
    '#size'          => 32,
    '#maxlength'     => 255,
    '#required'      => TRUE,
    '#default_value' => $provider->endpoint,
  );

  $form['scheme'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Schemes'),
    '#description'   => t('Newline separated list of schemes like !example', array('!example' => 'http://*.revision3.com/*')),
    '#required'      => TRUE,
    '#default_value' => $provider->scheme,
  );

  $label = empty($provider->pid) ? t('Save and proceed') : t('Save');
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => $label,
  );

  return $form;
}

/**
 * Validate submission of the provider edit form.
 */
function oembedcore_edit_form_provider_validate($form, &$form_state) {
  $values = $form_state['values'];
  // Test uniqueness of name:
  if (!$values['name']) {
    form_error($form['name'], t('Provider name is required.'));
  }
  elseif (preg_match("/[^a-z0-9_]/", $values['name'])) {
    form_error($form['name'], t('The name may only contain lowercase alphanumeric characters and underscores.'));
  }
  else {
    $query = "SELECT pid FROM {oembedcore_provider} WHERE name = '%s'";
    if (!empty($values['pid']) && is_numeric($values['pid'])) {
      $query .= ' AND pid != ' . $values['pid'];
    }
    if (db_result(db_query($query, $values['name']))) {
      form_error($form['name'], t('Provider name must be unique.'));
    }
  }
  //TODO: Add validation of endpoint and schemes
}

/**
 * Process submission of the mini panel edit form.
 */
function oembedcore_edit_form_provider_submit($form, &$form_state) {
  $provider = $form_state['values']['provider'];

  $provider->name     = $form_state['values']['name'];
  $provider->title    = $form_state['values']['title'];
  $provider->endpoint = $form_state['values']['endpoint'];
  $provider->scheme   = $form_state['values']['scheme'];

  if (empty($provider->pid)) {
    drupal_set_message(t('Your new provider %name has been saved.', array('%name' => $provider->name)));
    oembedcore_provider_save($provider);
    $form_state['values']['pid'] = $provider->pid;
  }
  else {
    drupal_set_message(t('Your changes have been saved.'));
    oembedcore_provider_save($provider);
  }

  $form_state['redirect'] = 'admin/build/oembed/provider';
}

/**
 * Page callback to export a provider to PHP code.
 */
function oembedcore_export_provider(&$form_state, $provider) {
  if (!is_object($provider)) {
    $provider = oembedcore_provider_load($provider);
  }
  drupal_set_title(check_plain($provider->name));
  $code = oembedcore_provider_export($provider);

  $lines = substr_count($code, "\n");
  $form['code'] = array(
    '#type'          => 'textarea',
    '#title'         => $provider->name,
    '#default_value' => $code,
    '#rows'          => $lines,
  );
  return $form;
}

/**
 * Provide a form to confirm deletion of a provider.
 */
function oembedcore_delete_confirm_provider(&$form_state, $provider) {
  if (!is_object($provider)) {
    $provider = oembedcore_provider_load($provider);
  }
  if ($provider->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
    $submit = t('Revert');
  }
  elseif ($provider->export_type != EXPORT_IN_CODE) {
    $submit = t('Delete');
  }
  else {
    drupal_not_found();
    return;
  }
  $form['name'] = array('#type' => 'value', '#value' => $provider->name);
  $form['pid']  = array('#type' => 'value', '#value' => $provider->pid);
  return confirm_form($form,
    t('Are you sure you want to delete the provider "@name"?', array('@name' => $provider->name)),
    !empty($_GET['destination']) ? $_GET['destination'] : 'admin/build/oembed/provider',
    t('This action cannot be undone.'),
    $submit, t('Cancel')
  );
}

/**
 * Handle the submit button to delete a mini panel.
 */
function oembedcore_delete_confirm_provider_submit($form, &$form_state) {
  $provider = oembedcore_provider_load($form_state['values']['name']);
  if ($provider->pid == $form_state['values']['pid']) {
    oembedcore_provider_delete($provider);
    $form_state['redirect'] = 'admin/build/oembed/provider';
  }
}

/**
 * Enable a default provider.
 */
function oembedcore_enable_provider($provider) {
  if (!is_object($provider)) {
    $provider = oembedcore_provider_load($provider);
  }
  ctools_include('export');
  ctools_export_set_status('oembedcore_provider', $provider->name, FALSE);
  cache_clear_all('oembedcore:providers', 'cache');
  drupal_goto();
}

/**
 * Disable a default provider.
 */
function oembedcore_disable_provider($provider) {
  if (!is_object($provider)) {
    $provider = oembedcore_provider_load($provider);
  }
  ctools_include('export');
  ctools_export_set_status('oembedcore_provider', $provider->name, TRUE);
  cache_clear_all('oembedcore:providers', 'cache');
  drupal_goto();
}