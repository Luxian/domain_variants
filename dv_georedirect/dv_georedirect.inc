<?php

/**
 * @file
 *   Utility functions for the Domain Variants Georedirect module.
 */

/**
 * Gets the language from the browser.
 *
 * This is a copy of the locale_language_from_browser() from the locale.inc file,
 * for the cases when the language module is not enabled, and the locale.inc
 * file is not loaded. The difference is that this function does not compare the
 * enabled languages on the site with the languages from the browser.
 */
function dv_georedirect_locale_language_from_browser() {
  if (empty($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
    return FALSE;
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
    return $lang_codes[0];
  }
  // No language found.
  return NULL;
}
