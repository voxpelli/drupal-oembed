<?php
// $Id$

class oembed_preset_ui extends ctools_export_ui {

  /**
   * Provide the actual editing form.
   */
  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);
    $form['width'] = array(
      '#type'          => 'textfield',
      '#size'          => 6,
      '#default_value' => $form_state['item']->width,
      '#title'         => t('Max width'),
      '#description'   => t('A maximum width in pixels of the embed or 0 for no maximum.'),
    );

    $form['height'] = array(
      '#type'          => 'textfield',
      '#size'          => 6,
      '#default_value' => $form_state['item']->height,
      '#title'         => t('Max height'),
      '#description'   => t('A maximum height in pixels of the embed or 0 for no maximum.'),
    );

    $form['disable_title'] = array(
      '#type'          => 'checkbox',
      '#default_value' => !empty($form_state['item']->conf['disable_title']),
      '#title'         => t('Disable title when other embeddable data is available'),
    );
  }

  /**
   * Handle the submission of the edit form.
   */
  function edit_form_submit(&$form, &$form_state) {
    parent::edit_form_submit($form, $form_state);

    // Since items in our settings are not in the schema, we have to do these
    // manually:
    $form_state['item']->conf['disable_title'] = $form_state['values']['disable_title'];
  }

  /**
   * Provide a list of sort options.
   */
  function list_sort_options() {
    $options = parent::list_sort_options();

    $options += array(
      'width' => t('Width'),
      'height' => t('Height'),
    );

    return $options;
  }

  /**
   * Build a row based on the item.
   */
  function list_build_row($item, &$form_state, $operations) {
    // Set up sorting
    $name = $item->{$this->plugin['export']['key']};

    // Note: $item->type should have already been set up by export.inc so
    // we can use it safely.
    switch ($form_state['values']['order']) {
      case 'disabled':
        $this->sorts[$name] = empty($item->disabled) . $name;
        break;
      case 'title':
        $this->sorts[$name] = $item->{$this->plugin['export']['admin_title']};
        break;
      case 'width':
        $this->sorts[$name] = $item->width;
        break;
      case 'height':
        $this->sorts[$name] = $item->height;
        break;
      case 'name':
        $this->sorts[$name] = $name;
        break;
      case 'storage':
        $this->sorts[$name] = $item->type . $name;
        break;
    }

    $this->rows[$name]['data'] = array();
    $this->rows[$name]['class'] = !empty($item->disabled) ? 'ctools-export-ui-disabled' : 'ctools-export-ui-enabled';

    // If we have an admin title, make it the first row.
    if (!empty($this->plugin['export']['admin_title'])) {
      $this->rows[$name]['data'][] = array('data' => check_plain($item->{$this->plugin['export']['admin_title']}), 'class' => 'ctools-export-ui-title');
    }
    $this->rows[$name]['data'][] = array('data' => check_plain($name), 'class' => 'ctools-export-ui-name');
    $this->rows[$name]['data'][] = array('data' => check_plain($item->width), 'class' => 'ctools-export-ui-width');
    $this->rows[$name]['data'][] = array('data' => check_plain($item->height), 'class' => 'ctools-export-ui-height');
    $this->rows[$name]['data'][] = array('data' => check_plain($item->type), 'class' => 'ctools-export-ui-storage');
    $this->rows[$name]['data'][] = array('data' => theme('links', $operations), 'class' => 'ctools-export-ui-operations');
    // Add an automatic mouseover of the description if one exists.
    if (!empty($this->plugin['export']['admin_description'])) {
      $this->rows[$name]['title'] = $item->{$this->plugin['export']['admin_description']};
    }
  }

  /**
   * Provide the table header.
   */
  function list_table_header() {
    $header = array();
    if (!empty($this->plugin['export']['admin_title'])) {
      $header[] = array('data' => t('Title'), 'class' => 'ctools-export-ui-title');
    }

    $header[] = array('data' => t('Name'), 'class' => 'ctools-export-ui-name');
    $header[] = array('data' => t('Width'),   'class' => 'oembedcore-preset-width');
    $header[] = array('data' => t('Height'),  'class' => 'oembedcore-preset-height');
    $header[] = array('data' => t('Storage'), 'class' => 'ctools-export-ui-storage');
    $header[] = array('data' => t('Operations'), 'class' => 'ctools-export-ui-operations');

    return $header;
  }
}
