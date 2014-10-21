<?php

/**
 * @file
 *   Utility functions for the Domain Variants module.
 */

/**
 * Gets the language from the browser.
 *
 * This is a modified copy of the locale_language_from_browser(),
 * for the cases when the language module is not enabled, and the locale.inc
 * file is not loaded. The difference is that this function does not compare the
 * enabled languages on the site with the languages from the browser, and it
 * returns an array of all detected languages.
 *
 * @return array
 *   An indexed array of languages in acceptability order.
 */
function _domain_variants_locale_languages_from_browser() {
  if (empty($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
    return array();
  }

  // The Accept-Language header contains information about the language
  // preferences configured in the user's browser / operating system.
  // RFC 2616 (section 14.4) defines the Accept-Language header as follows:
  //   Accept-Language = "Accept-Language" ":"
  //                  1#( language-range [ ";" "q" "=" qvalue ] )
  //   language-range  = ( ( 1*8ALPHA *( "-" 1*8ALPHA ) ) | "*" )
  // Samples: "hu, en-us;q=0.66, en;q=0.33", "hu,en-us;q=0.5"
  $browser_langcodes = array();
  if (preg_match_all('@(?<=[, ]|^)([a-zA-Z-]+|\*)(?:;q=([0-9.]+))?(?:$|\s*,\s*)@', trim($_SERVER['HTTP_ACCEPT_LANGUAGE']), $matches, PREG_SET_ORDER)) {
    foreach ($matches as $match) {
      // We can safely use strtolower() here, tags are ASCII.
      // RFC2616 mandates that the decimal part is no more than three digits,
      // so we multiply the qvalue by 1000 to avoid floating point comparisons.
      $langcode = strtolower($match[1]);
      $qvalue = isset($match[2]) ? (float) $match[2] : 1;
      $browser_langcodes[$langcode] = (int) ($qvalue * 1000);
    }
  }

  // We should take pristine values from the HTTP headers, but Internet Explorer
  // from version 7 sends only specific language tags (eg. fr-CA) without the
  // corresponding generic tag (fr) unless explicitly configured. In that case,
  // we assume that the lowest value of the specific tags is the value of the
  // generic language to be as close to the HTTP 1.1 spec as possible.
  // See http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.4 and
  // http://blogs.msdn.com/b/ie/archive/2006/10/17/accept-language-header-for-internet-explorer-7.aspx
  if (!empty($browser_langcodes)) {
    arsort($browser_langcodes);
    foreach ($browser_langcodes as $langcode => $qvalue) {
      $generic_tag = strtok($langcode, '-');
      if (!isset($browser_langcodes[$generic_tag])) {
        $browser_langcodes[$generic_tag] = $qvalue;
      }
    }
    $lang_codes = array_keys($browser_langcodes);
    return $lang_codes;
  }
  // No language found.
  return array();
}

/**
 * Returns two-letters lowercase country ISO code detected from IP address.
 *
 * @return null|string
 */
function _domain_variants_get_country_code_from_geoip() {
  $geoip = array();
  if (function_exists('geoip_record_by_name')) {
    $geoip = @geoip_record_by_name(ip_address());
  }
  if (isset($geoip['country_code'])) {
    return drupal_strtolower($geoip['country_code']);
  }
  return NULL;
}

/**
 * Returns domain variant languages.
 *
 * @param int $domain_variant_id
 * @param string $mode
 *   Allowed values:
 *   - all: all languages
 *   - enabled: languages that are enabled for the given domain variant
 *   - preferred: user preferred languages from the "enabled" list
 *
 * @return array
 *   An associative array(
 *     {string:langcode} => {boolean:enabled},
 *     ...,
 *   ) according to $mode parameter value.
 */
function _domain_variants_get_domain_languages($domain_variant_id, $mode) {
  $cache =& drupal_static(__FUNCTION__, array());
  if (!isset($cache[$domain_variant_id])) {
    $cfg = variable_get('domain_variants_languages', array());
    $configured_languages = isset($cfg[$domain_variant_id]) ? $cfg[$domain_variant_id] : array();
    $system_languages = language_list('enabled');
    $system_languages = array_fill_keys(array_keys($system_languages[1]), FALSE);
    $all = array_intersect_key($configured_languages, $system_languages) + $system_languages;
    $enabled = array_filter($all);
    $user_languages = array_fill_keys(_domain_variants_locale_languages_from_browser(), TRUE);
    $preferred = array_intersect_key($user_languages, $enabled);
    $cache[$domain_variant_id] = array(
      'all' => $all,
      'enabled' => $enabled,
      'preferred' => $preferred,
    );
  }
  return isset($cache[$domain_variant_id][$mode]) ? $cache[$domain_variant_id][$mode] : array();
}

/**
 * Checks whether language is allowed for domain variant.
 *
 * @param int $domain_variant_id
 * @param string $langcode
 *
 * @return string|null
 *   NULL if language is allowed. Otherwise: the most appropriate langcode.
 */
function _domain_variant_check_language($domain_variant_id, $langcode) {
  $enabled = _domain_variants_get_domain_languages($domain_variant_id, 'enabled');
  if (!empty($enabled) && !isset($enabled[$langcode])) {
    $preferred = _domain_variants_get_domain_languages($domain_variant_id, 'preferred');
    return !empty($preferred) ? key($preferred) : key($enabled);
  }
  return NULL;
}
