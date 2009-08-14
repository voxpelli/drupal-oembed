<?php

function _oembed_filter_apply($text, $format) {
  global $_oembed_default_attributes;
  $_oembed_default_attributes = array(
    'maxwidth'  => intval(variable_get('oembed_maxwidth_'  . $format, '')),
    'maxheight' => intval(variable_get('oembed_maxheight_' . $format, '')),
  );
  $text = preg_replace_callback("`(^|<p>|<li>|<br\s*/?>|[ \n\r\t\(])((http://|https://|ftp://|mailto:|smb://|afp://|file://|gopher://|news://|ssl://|sslv2://|sslv3://|tls://|tcp://|udp://)([a-zA-Z0-9@:%_+*~#?&=.,/;-]*[a-zA-Z0-9@:%_+*~#&=/;-]))([.,?!]*?)(?=($|</p>|</li>|<br\s*/?>|[ \n\r\t\)]))`i", '_oembed_preg_parse', $text);
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

function _oembed_oembedprovider() {
  $oembeddable = array(
    'flickr' => array(
      'scheme'   => array('http://*.flickr.com/*', 'http://flickr.com/*'),
      'endpoint' => 'http://www.flickr.com/services/oembed/',
    ),
    'viddler' => array(
      'scheme'   => 'http://*.viddler.com/*',
      'endpoint' => 'http://lab.viddler.com/services/oembed/',
    ),
    'qik' => array(
      'scheme'   => array('http://qik.com/video/*', 'http://qik.com/*'),
      'endpoint' => 'http://qik.com/api/oembed.json',
    ),
    'revision3' => array(
      'scheme'   => 'http://*.revision3.com/*',
      'endpoint' => 'http://revision3.com/api/oembed/',
    ),
    'vimeo' => array(
      'scheme'   => array('http://www.vimeo.com/*', 'http://www.vimeo.com/groups/*/*'),
      'endpoint' => 'http://www.vimeo.com/api/oembed.json',
    ),
    'oohembed' => array(
      'scheme'   => array(
        'http://*.youtube.com/watch*',
        'http://*.wikipedia.org/wiki/*',
        'http://*.slideshare.net/*',
        'http://*.imdb.com/title/tt*/',
        'http://*.collegehumor.com/video:*',
      ),
      'endpoint' => 'http://oohembed.com/oohembed/',
    ),
  );

  $providers = array();
  foreach ($oembeddable as $site) {
    foreach ((array)$site['scheme'] as $scheme) {
      $providers[$scheme] = array(
        'endpoint' => $site['endpoint'],
      );
    }
  }
  return $providers;
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
    $return = oembedcore_oembed_html($embed, $url);
  }
  if (empty($return)) {
    $return  = l($match, $url, array('absolute' => TRUE));
  }

  return $prefix . $return . $suffix;
}