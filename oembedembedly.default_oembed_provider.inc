<?php

/**
 * Implementation of hook_default_oembed_provider().
 */
function oembedembedly_default_oembed_provider() {
  $providers = array();

  $json = drupal_http_request('http://api.embed.ly/v1/api/services');

  if ($json->code == 200) {
    $json = json_decode($json->data);
  }
  else {
    $json = FALSE;
  }

  if (is_array($json)) {
    foreach ($json as $embedly_provider) {
      $provider = new stdClass;
      $provider->disabled = !variable_get('oembedembedly_default', TRUE);
      $provider->name = 'oembedembedly_' . $embedly_provider->name;
      $provider->title = $embedly_provider->displayname . ' (Embed.ly)';
      $provider->endpoint = 'http://api.embed.ly/v1/api/oembed';
      $provider->scheme = implode("\n", $embedly_provider->regex);
      $provider->type = $embedly_provider->type;
      $provider->api_version = 1;
      $providers['oembedembedly_' . $embedly_provider->name] = $provider;
    }
  }

  return $providers;
}
