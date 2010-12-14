<?php
// $Id$

/**
 * @file
 * Functions for the oEmbed filter
 */

function _oembed_filter_apply($text, $format) {
  global $_oembed_default_attributes;
  $_oembed_default_attributes = array(
    'maxwidth'  => intval(variable_get('oembed_maxwidth_'  . $format, '')),
    'maxheight' => intval(variable_get('oembed_maxheight_' . $format, '')),
  );
  if (empty($_oembed_default_attributes['maxwidth'])) {
    unset($_oembed_default_attributes['maxwidth']);
  }
  if (empty($_oembed_default_attributes['maxheight'])) {
    unset($_oembed_default_attributes['maxheight']);
  }
  $text = preg_replace_callback("`(^|<p(?:\s[^>]*)*>|<li(?:\s[^>]*)*>|<br(?:\s[^>]*)*>|[ \n\r\t\(])((http://|https://|ftp://|mailto:|smb://|afp://|file://|gopher://|news://|ssl://|sslv2://|sslv3://|tls://|tcp://|udp://)([a-zA-Z0-9@:%_+*~#?&=.,/;-]*[a-zA-Z0-9@:%_+*~#&=/;-]))([.,?!]*?)(?=($|</p>|</li>|<br\s*/?>|[ \n\r\t\)]))`i", '_oembed_preg_parse', $text);
  unset($_oembed_default_attributes);
  return $text;
}

function _oembed_filter_settings($format) {
  $form = array();
  $form['oembed'] = array(
    '#type' => 'fieldset',
    '#title' => t('oEmbed'),
    '#collapsible' => TRUE,
  );
  $form['oembed']['oembed_maxwidth_' . $format] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum width of embed'),
    '#default_value' => variable_get('oembed_maxwidth_' . $format, ''),
    '#description' => t('The maximum width of an embed, isn\'t respected by all providers'),
  );
  $form['oembed']['oembed_maxheight_' . $format] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum height of embed'),
    '#default_value' => variable_get('oembed_maxheight_' . $format, ''),
    '#description' => t('The maximum height of an embed, isn\'t respected by all providers'),
  );
  return $form;
}

function _oembed_preg_parse($match) {
  return _oembed_resolve_link($match[2], $match[1], $match[5]);
}

function _oembed_resolve_link($match, $prefix, $suffix) {
  $return = '';
  $url = decode_entities($match);
  $matches = array();
  if ($provider = oembedcore_get_provider($url, $matches)) {
    $embed = oembedcore_oembed_fetch($provider, $url, $matches);
    if ($embed) {
      $return = oembedcore_oembed_html($embed, $url);
    }
  }
  if (empty($return)) {
    $return  = l($match, $url, array('absolute' => TRUE));
  }

  return $prefix . $return . $suffix;
}