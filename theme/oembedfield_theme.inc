<?php
// $Id$

/**
 * @file
 * Theme related functions for oEmbed Field
 */

function template_preprocess_oembedfield_formatter_oembed_default(&$vars) {
  $element = $vars['element'];

  $vars['embed'] = '';

  if (empty($element['#item']['url'])) {
    return;
  }

  $presetname_start = strpos($element['#formatter'], '_') + 1;
  $presetname_stop  = strrpos($element['#formatter'], '_');
  $presetname = substr($element['#formatter'], $presetname_start, $presetname_stop - $presetname_start);

  $attributes = array();
  $url        = url($element['#item']['url'], $element['#item']);
  $preset     = oembedcore_preset_load($presetname);
  if (!empty($preset)) {
    if ($preset->width > 0) {
      $attributes['maxwidth'] = $preset->width;
    }
    if ($preset->height > 0) {
      $attributes['maxheight'] = $preset->height;
    }
  }

  $embed = oembedcore_oembed_data($url, $attributes);

  if (!$embed) {
    $embed = (object) array(
      'type'         => 'link',
      'original_url' => $url,
    );
  }
  if ($embed->type != 'link' && !empty($preset->conf['disable_title'])) {
    unset($embed->title);
  }
  elseif (!isset($embed->title)) {
    $embed->title = $element['#item']['display_url'];
  }

  $vars['embed'] = theme('oembed', $embed);
}