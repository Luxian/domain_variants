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
   * @see RedirectorComponentInterface::getStorageColumns()
   */
  public function getStorageColumns() {
    return array('domain_variant');
  }

	/** (non-PHPdoc)
   * @see RedirectorComponentInterface::getAvailableOptions()
   */
  public function getAvailableOptions($column_name = NULL) {
    $domains = domain_domains(TRUE);
    $options['domain_variant'] = array();
    foreach ($domains as $domain) {
      // Skip the domain if it has no domain variants.
      if (empty($domain['variants'])) {
        continue;
      }
      foreach ($domain['variants'] as $variant) {
        $options['domain_variant'][$variant->variant_id] = $domain['sitename'] . ' (' . $variant->name . ')';
      }
    }
    if (!empty($column_name)) {
      return $options[$column_name];
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
      return array('domain_variant' => $_domain_variant->variant_id);
    }
    return array('domain_variant' => NULL);
  }

  /**
   * (non-PHPdoc)
   * @see RedirectorComponentInterface::prepareRedirect()
   */
  public function prepareRedirect($values, &$url, &$options) {
    if (!empty($values['domain_variant'])) {
      $new_domain_variant_id = NULL;
      if (is_string($values['domain_variant']['value'])) {
        $new_domain_variant_id= $values['domain_variant']['value'];
      }
      elseif (is_array($values['domain_variant']['value'])) {
        $new_domain_variant_id = reset($values['domain_variant']['value']);
      }
      // Alter the domain variant of the options array.
      if (!empty($new_domain_variant_id)) {
        $new_domain_variant = domain_variants_load($new_domain_variant_id);
        if (!empty($new_domain_variant)) {
          $options['domain_variant'] = $new_domain_variant;
        }
      }
    }
  }
}
