<?php

/**
 * @file
 *  Domain Variants Redirect component class.
 */

class DVRedirectorComponent extends RedirectorComponent {

	/** (non-PHPdoc)
   * @see RedirectorComponentInterface::getTitle()
   */
  public function getTitle() {
    return 'Domain Variant';
  }

	/** (non-PHPdoc)
   * @see RedirectorComponentInterface::getAvailableOptions()
   */
  public function getAvailableOptions() {
    $domains = domain_domains(TRUE);
    $options = array();
    foreach ($domains as $domain) {
      // Skip the domain if it has no domain variants.
      if (empty($domain['variants'])) {
        continue;
      }
      foreach ($domain['variants'] as $variant) {
        $options[$variant->variant_id] = $domain['sitename'] . ' (' . $variant->name . ')';
      }
    }
    return $options;
  }

  /**
  * (non-PHPdoc)
  * @see RedirectorComponentInterface::getCurrentValue()
  */
  public function getCurrentValue() {
    global $_domain_variant;
    if (!empty($_domain_variant)) {
      return $_domain_variant->variant_id;
    }
    return NULL;
  }

  /**
   * (non-PHPdoc)
   * @see RedirectorComponentInterface::prepareRedirect()
   */
  public function prepareRedirect($values, &$url, &$options) {
    if (!empty($values)) {
      $new_domain_variant_id = NULL;
      if (is_string($values['value'])) {
        $new_domain_variant_id= $values['value'];
      }
      elseif (is_array($values['value'])) {
        $new_domain_variant_id = reset($values['value']);
      }
      // Alter the domain variant of the options array.
      if (!empty($new_domain_variant_id) && $new_domain_variant_id != '--any--') {
        $new_domain_variant = domain_variants_load($new_domain_variant_id);
        if (!empty($new_domain_variant)) {
          $options['domain_variant'] = $new_domain_variant;
        }
      }
    }
  }
}
