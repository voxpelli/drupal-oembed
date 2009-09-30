<?php
// $Id$

/**
 * Theme related functions for oEmbed Field
 */

function template_preprocess_oembedfield_formatter_oembed_default(&$vars) {
  $element = $vars['element'];

  $vars['embed'] = '';

  if (empty($element['#item']['url'])) {
    return;
  }

  $url   = url($element['#item']['url'], $element['#item']);
  $embed = oembedcore_oembed_data($url);

  if (!$embed) {
    $embed = (object) array(
      'type'         => 'link',
      'original_url' => $url,
    );
  }
  if (!isset($embed->title)) {
    $embed->title = $element['#item']['display_url'];
  }

  $vars['embed'] = theme('oembed', $embed);
}