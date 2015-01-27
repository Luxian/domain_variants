<?php

/**
 * @file
 *  Switcher component for domain variants.
 */

class SwitcherDomainVariantComponent implements SwitcherComponentInterface {

  /**
   * (non-PHPdoc)
   * @see SwitcherComponentInterface::getTitle()
   */
  public function getTitle() {
    return t('Domain Variant');
  }

  /**
   * (non-PHPdoc)
   * @see SwitcherComponentInterface::getAvailableOptions()
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
   * @see SwitcherComponentInterface::summary()
   */
  public function summary($values) {
    $domain_variants_names = array();
    if (!empty($values['domain_variant'])) {
      $domain_variant = domain_variants_load($values['domain_variant']);
      $domain_variants_names[] = $domain_variant->name;
    }
    return t('Domain variants: @domain_variants', array('@domain_variants' => implode(', ', $domain_variants_names)));
  }

  /**
   * (non-PHPdoc)
   * @see SwitcherComponentInterface::prepareSwitcherItem()
   */
  public function prepareSwitcherItem(&$values, &$url, &$options) {
    if (!empty($values['domain_variant'])) {
      $domain_variant = domain_variants_load($values['domain_variant']);
      if (!empty($domain_variant)) {
        $options['domain_variant'] = $domain_variant;
      }
    }
  }

  /**
   * (non-PHPdoc)
   * @see SwitcherComponentInterface::getCurrentValue()
   */
  public function getCurrentValue() {
    global $_domain_variant;
    if (!empty($_domain_variant)) {
      return $_domain_variant->variant_id;
    }
    return NULL;
  }
}
