<?php
// $Id$

/**
 * Callback handler for oembed requests.
 *
 * @param string $format
 *  Optional. The response format to use.
 *  Defaults to $_GET['format'] or 'json', in that order.
 */
function _oembedprovider_handle_request($format='') {
  if (empty($format)) {
    $format = empty($_GET['format']) ? 'json' : $_GET['format'];
  }

  // Get the supported formats, and abort if a unsupported format it requested
  $formats = oembedprovider_formats();
  if (!isset($formats[$format])) {
    header('HTTP/1.0 501 Not implemented. Unsupported response format "' . check_plain($format) . '"');
    die;
  }
  $f = $formats[$format];
  header('Content-type: ' . $f['mime']);

  // Check that we got a url
  if (empty($_GET['url'])) {
    header('HTTP/1.0 406 Not acceptable. The url parameter is required.');
    print call_user_func($f['callback'], _oembedprovider_result('rich', array(
      'title' => t('Not acceptable'),
      'error' => 1,
      'html' => t('The url parameter is required'),
    )));
    die;
  }

  $url = $_GET['url'];
  $width = isset($_GET['width']) ? $_GET['width'] : 0;
  $height = isset($_GET['height']) ? $_GET['height'] : 0;

  $matches = array();
  $provider = oembedcore_get_provider($url, $matches);

  if ($provider) {
    $callback = isset($provider['callback']) ? $provider['callback'] : 'oembedcore_oembed_fetch';
    $data = call_user_func($callback, $provider, $url, $matches);
    print call_user_func($f['callback'], $data);
    exit;
  }
  else {
    header('HTTP/1.0 404 Not found.');
    print call_user_func($f['callback'], _oembedprovider_result('rich', array(
      'title' => t('Not found'),
      'error' => 1,
      'html' => t('Could not find a provider that supports this url'),
    )));
    die;
  }
}

/**
 * Merges a result with some standard values.
 *
 * @param string $type
 * @param array $result
 */
function _oembedprovider_result($type, $props) {
  return array_merge(array(
    'type' => $type,
    'version' => '1.0',
    'provider_name' => variable_get('site_name', ''),
    'width' => 0,
    'height' => 0,
  ), $props);
}


/**
 * The default provider to handle nodes
 *
 * @param string $url
 * @param array $matches
 */
function _oembedprovider_node_provider($provider, $url, $matches) {
  $nid = $matches[1];
  $node = node_load($nid);
  $author = user_load(array('uid' => $node->uid));
  return _oembedprovider_result('rich', array(
    'html' => theme('oembed_node', $node),
    'title' => $node->title,
    'author_name' => $author->name,
    'author_url' => url('user/' . $author->uid, array('absolute' => TRUE)),
  ));
}

/**
 * JSONP formatter
 */
function _oembedprovider_formats_jsonp($data) {
  $callback = !empty($_GET['callback']) ? $_GET['callback'] : 'jsonp';
  return sprintf('%s(%s)', $callback, _oembedprovider_formats_json($data));
}

/**
 * JSON formatter
 */
function _oembedprovider_formats_json($data) {
  return json_encode($data);
}

/**
 * XML formatter
 */
function _oembedprovider_formats_xml($data) {
  $doc = new DomDocument('1.0', 'utf-8');
  $oembed = $doc->createElement('oembed');
  $doc->appendChild($oembed);
  foreach ($data as $key => $value) {
    $e = $doc->createElement($key);
    $e->appendChild($doc->createTextNode($value));
    $oembed->appendChild($e);
  }
  return $doc->saveXml();
}