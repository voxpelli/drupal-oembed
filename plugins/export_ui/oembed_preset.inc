<?php
// $Id$

$plugin = array(
  'schema' => 'oembedcore_preset',
  'access' => 'administer oembed presets',
  'menu' => array(
    'menu prefix' => 'admin/build/oembed',
    'menu item' => 'preset',
    'menu title' => 'Presets',
    'menu description' => 'Add, edit and remove oembed preset from the system.',
  ),
  'handler' => array(
    'class' => 'oembed_preset_ui',
    'parent' => 'ctools_export_ui',
  ),
  'title singular' => t('preset'),
  'title singular proper' => t('Preset'),
  'title plural' => t('presets'),
  'title plural proper' => t('Presets'),
);
