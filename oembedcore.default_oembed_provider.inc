<?php

/**
 * Implementation of hook_default_oembed_provider().
 */
function oembedcore_default_oembed_provider() {
  $providers = array();

  $provider = new stdClass;
  $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
  $provider->name = 'viddler';
  $provider->title = 'Viddler';
  $provider->endpoint = 'http://lab.viddler.com/services/oembed/';
  $provider->scheme = 'http://*.viddler.com/*';
  $provider->api_version = 1;
  $providers['viddler'] = $provider;

  $provider = new stdClass;
  $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
  $provider->name = 'flickr';
  $provider->title = 'Flickr';
  $provider->endpoint = 'http://www.flickr.com/services/oembed/';
  $provider->scheme = 'http://*.flickr.com/*
http://flickr.com/*';
  $provider->api_version = 1;
  $providers['flickr'] = $provider;

  $provider = new stdClass;
  $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
  $provider->name = 'qik';
  $provider->title = 'Qik';
  $provider->endpoint = 'http://qik.com/api/oembed.json';
  $provider->scheme = 'http://qik.com/video/*
http://qik.com/*';
  $provider->api_version = 1;
  $providers['qik'] = $provider;

  $provider = new stdClass;
  $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
  $provider->name = 'revision3';
  $provider->title = 'Revision3';
  $provider->endpoint = 'http://revision3.com/api/oembed/';
  $provider->scheme = 'http://*.revision3.com/*';
  $provider->api_version = 1;
  $providers['revision3'] = $provider;

  $provider = new stdClass;
  $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
  $provider->name = 'vimeo';
  $provider->title = 'Vimeo';
  $provider->endpoint = 'http://www.vimeo.com/api/oembed.json';
  $provider->scheme = 'http://www.vimeo.com/groups/*/videos/*
http://www.vimeo.com/*
http://vimeo.com/groups/*/videos/*
http://vimeo.com/*';
  $provider->api_version = 1;
  $providers['vimeo'] = $provider;

  $provider = new stdClass;
  $provider->disabled = TRUE; /* Edit this to true to make a default provider disabled initially */
  $provider->name = 'youtube';
  $provider->title = 'YouTube';
  $provider->endpoint = 'http://www.youtube.com/oembed';
  $provider->scheme = 'http://*.youtube.com/watch*';
  $provider->api_version = 1;
  $providers['youtube'] = $provider;

  return $providers;
}

