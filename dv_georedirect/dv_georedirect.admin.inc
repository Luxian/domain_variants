<?php

/**
 * Configuration form callback.
 */
function dv_georedirect_config_form($form, &$form_state) {
  $form['ensure_on'] = array(
    '#type' => 'radios',
    '#title' => t('Ensure domain variant on'),
    '#options' => array(
      'front' => t('front page only'),
      'all' => t('all pages'),
    ),
    '#default_value' => _dv_georedirect_setting('ensure_on'),
  );
  $form['exclude_admin_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not ensure domain variant on admin pages'),
    '#states' => array(
      'visible' => array(
        ':input[name="ensure_on"]' => array('value' => 'all'),
      ),
    ),
    '#default_value' => _dv_georedirect_setting('exclude_admin_pages'),
  );
  $form['redirect_to'] = array(
    '#type' => 'radios',
    '#title' => t('Redirect to'),
    '#options' => array(
      'front' => t('front page'),
      'same' => t('the same page'),
    ),
    '#default_value' => _dv_georedirect_setting('redirect_to'),
  );
  $form['country_detection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Country detection'),
  );
  $form['country_detection']['override_geoip_from_get'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use GET parameter to override geoip-detected country'),
    '#default_value' => _dv_georedirect_setting('override_geoip_from_get'),
  );
  $form['country_detection']['get_param_name'] = array(
    '#type' => 'textfield',
    '#title' => t('GET parameter name'),
    '#description' => t('Use only lowercase characters and underscore symbols'),
    '#states' => array(
      'visible' => array(
        ':input[name="override_geoip_from_get"]' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => _dv_georedirect_setting('get_param_name'),
  );
  $form['country_detection']['get_param_represents'] = array(
    '#type' => 'radios',
    '#title' => t('GET parameter represents'),
    '#options' => array(
      'country_iso_code' => t('Country ISO code'),
      'domain_machine_name' => t('Domain machine name'),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="override_geoip_from_get"]' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => _dv_georedirect_setting('get_param_represents'),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Validation handler for the configuration form.
 */
function dv_georedirect_config_form_validate(&$form, &$form_state) {
  $form_state['values']['get_param_name'] = trim($form_state['values']['get_param_name']);
  if ($form_state['values']['override_geoip_from_get']
      && ($form_state['values']['get_param_name'] == ''
        || preg_match('#[^a-z_]#', $form_state['values']['get_param_name']))) {
    form_error($form['country_detection']['get_param_name'], t('GET parameter name should only contain lowercase characters and underscore symbols.'));
  }
}

/**
 * Submit handler for the configuration form.
 */
function dv_georedirect_config_form_submit(&$form, &$form_state) {
  foreach (_dv_georedirect_setting('LIST') as $name) {
    if (isset($form_state['values'][$name])) {
      _dv_georedirect_setting($name, $form_state['values'][$name]);
    }
  }
}
