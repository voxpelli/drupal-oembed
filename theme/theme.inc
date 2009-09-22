<?php
// $Id$

/**
 * Theme related functions for oEmbed Core
 */

function template_preprocess_oembed(&$vars) {
  $url = $vars['url'];
  $matches = array();
  if ($provider = oembedcore_get_provider($url, $matches)) {
    $embed = oembedcore_oembed_fetch($provider, $url, $matches);
  }
  if (empty($embed)) {
    $embed = (object) array(
      'type' => 'link',
      'title' => $vars['title'],
    );
  }
  print '<pre>';
  var_dump($embed);
  print '</pre>';

  $vars['title'] = $embed->title;
  $vars['embed'] = $embed;

  $vars['template_files'][] = 'oembed';

  $valid_types = array('link', 'photo', 'video', 'rich');
  if (in_array($embed->type, $valid_types)) {
    $vars['template_files'][] = 'oembed-' . $embed->type;
  }
}