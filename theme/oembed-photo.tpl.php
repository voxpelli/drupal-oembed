<div class="oembed">
  <?php if (!empty($title)): ?>
    <?php print l($title, $url, array('absolute' => TRUE, 'attributes' => array('class' => 'oembed-title'))); ?>
  <?php endif; ?>
  <?php print l(theme('image', $embed->url), $url, array('html' => TRUE, 'absolute' => TRUE, 'attributes' => array('class' => 'oembed-photo oembed-content'))); ?>
</div>
