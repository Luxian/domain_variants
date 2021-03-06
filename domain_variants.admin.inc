<?php

/**
 * Admin menu callbacks for the Domain Variants module.
 */

function domain_variants_admin($domain, $variant = NULL) {
  $variants = domain_variants_loockup($domain['domain_id']);
  $output = '<div>' . t('Domain variants for %domain (%domain_url). !domain_list', array('%domain' => $domain['sitename'], '%domain_url' => $domain['subdomain'], '!domain_list' => l(t('Back to the domains list.'), 'admin/structure/domain/variants'))) . '</div>';
  return array(
    'domain_info' => array('#markup' => $output),
    'variants_table' => array('#markup' => domain_variants_table($variants)),
    'variants_form' => drupal_get_form('domain_variants_form', $domain, $variant),
  );
}

/**
 * Returns a list of variants as a html table.
 */
function domain_variants_table($variants) {
  if (empty($variants)) {
    return t('There are no domain variants.');
  }
  $header = array();
  $header[] = array('data' => t('Path'));
  $header[] = array('data' => t('Name'));
  $header[] = array('data' => t('Actions'));
  $rows = array();
  foreach ($variants as $variant) {
    $row = array();
    $variant = domain_variants_load($variant->variant_id);
    $row[] = array('data' => $variant->path);
    $row[] = array('data' => $variant->name);
    $actions = array();
    $actions[] = l(t('edit'), 'admin/structure/domain/variants/' . $variant->domain_id . '/edit/' . $variant->variant_id);
    $actions[] = l(t('delete'), 'admin/structure/domain/variants/' . $variant->domain_id . '/delete/' . $variant->variant_id);
    $row[] = array('data' => implode(' | ', $actions));
    $rows[] = $row;
  }
  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Form constructor for adding or updating a domain variant.
 */
function domain_variants_form($form, &$form_state, $domain, $variant = NULL) {
  $form = array();
  if (isset($variant)) {
    $form['variant_id'] = array(
      '#type' => 'value',
      '#value' => $variant->variant_id,
    );
  }
  $form['domain_id'] = array(
    '#type' => 'value',
    '#value' => $domain['domain_id'],
  );
  $form['domain_variant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Domain variant'),
    '#collapsible' => TRUE,
    '#collapsed' => isset($variant) ? FALSE : TRUE,
  );
  $form['domain_variant']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#description' => t('The name of the variant'),
    '#default_value' => isset($variant) ? $variant->name : '',
  );
  $form['domain_variant']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#required' => TRUE,
    '#description' => t('The path of the domain variant using the full <em>path.example.com</em> format. Leave off the http:// and the trailing slash. Can only contain lowercase alphanumeric characters, dashes and a colon (if using alternative ports).'),
    '#default_value' => isset($variant) ? $variant->path : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function domain_variants_form_validate($form, &$form_state) {
  // Strip the end slash and the schema, if any.
  form_set_value($form['domain_variant']['path'], rtrim(str_replace(array('ftp://', 'http://', 'https://'), '', $form_state['values']['path']), '/'), $form_state);
  // Also, check that the path is unique.
  $variant_id = isset($form_state['values']['variant_id']) ? $form_state['values']['variant_id'] : 0;
  $query = db_select('domain_variants', 'dm')->fields('dm', array('variant_id'));
  $query->condition('dm.path', $form_state['values']['path'])->condition('dm.variant_id', $variant_id, '<>');
  $result = $query->execute()->fetch();
  if ($result) {
    form_set_error('path', t('The path is already in use. Please choose another one.'));
  }
}

function domain_variants_form_submit(&$form, &$form_state) {
  $domain_variant = new stdClass();
  $domain_variant->is_new = TRUE;
  if (!empty($form_state['values']['variant_id'])) {
    $domain_variant = domain_variants_load($form_state['values']['variant_id']);
  }
  $domain_variant->path = $form_state['values']['path'];
  $domain_variant->domain_id = $form_state['values']['domain_id'];
  $domain_variant->name = $form_state['values']['name'];

  domain_variants_save($domain_variant);
  drupal_set_message(t('The domain variant has been saved.'));
  $form_state['redirect'] = 'admin/structure/domain/variants/' . $form_state['values']['domain_id'];
}

/**
 * Confirmation form for deleting a domain variant.
 */
function domain_variants_delete_confirm($form, &$form_state, $domain, $variant) {
  $form = array();
  $form['domain_id'] = array(
    '#type' => 'value',
    '#value' => $domain['domain_id'],
  );
  $form['variant_id'] = array(
    '#type' => 'value',
    '#value' => $variant->variant_id,
  );
  return confirm_form($form, t('Are you sure you want to delete the domain variant?'), 'admin/structure/domain/variants/' . $domain['domain_id'],
                      t('Are you sure you want to delete the domain variant %variant? This action cannot be undone.', array('%variant' => $variant->path)));
}

function domain_variants_delete_confirm_submit($form, &$form_state) {
  domain_variants_delete($form_state['values']['variant_id']);
  drupal_set_message(t('The domain variant has been deleted.'));
  $form_state['redirect'] = 'admin/structure/domain/variants/' . $form_state['values']['domain_id'];
}

/**
 * Menu callback for the domain variants page.
 */
function domain_variants_page() {
  // Show a list with all the domains, and also with a link to the variants for
  // each of them. We cannot use the default domains list because in new version
  // of the module, there is no hook_domainlinks() anymore, so it is rather hard
  // to alter the view of that page.
  // @todo: Pager needed?
  $query = db_select('domain', 'domain')->fields('domain')->orderBy('domain.weight');
  $result = $query->execute()->fetchAll();
  $header = array(t('Name'), t('Operations'));
  $rows = array();
  foreach ($result as $data) {
    $row = array();
    $row[] = $data->sitename;
    $actions = array(l(t('edit variants'), 'admin/structure/domain/variants/' . $data->domain_id));
    $row[] = implode(' | ', $actions);
    $rows[] = $row;
  }
  return array(
    'variants' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    ),
  );
}
