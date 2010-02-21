<?php
// $Id$
/**
 * @file
 * Default template file for oembed data of the photo type
 */
?>
<div class="oembed">
  <?php if (!empty($title)): ?>
    <?php print l($title, $original_url, array('absolute' => TRUE, 'attributes' => array('class' => 'oembed-title'))); ?>
  <?php endif; ?>
  <?php print l(theme('image', $embed->url, '', '', NULL, FALSE), $original_url, array('html' => TRUE, 'absolute' => TRUE, 'attributes' => array('class' => 'oembed-photo oembed-content'))); ?>
</div>
