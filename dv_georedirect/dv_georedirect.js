(function($) {

  /**
   * The name of a cookie that indicates that user should not be redirected
   * from the International domain.
   */
  var DV_COOKIE_INTERNATIONAL_IS_ALLOWED = 'dv_international_is_allowed';

  /**
   * The identifier of the International domain in the dropdown values.
   */
  var DV_INTERNATIONAL_DOMAIN = 'd_1';

  // We don't need Drupal.behaviors for this.
  $(function() {
    var previousUrl = $.cookie('dv_previous_url');
    $.cookie('dv_previous_url', document.location.href, {path: '/'});
    if (document.location.href == previousUrl) {
      // Probably we met an infinite redirect loop.
      return;
    }

    var settings = Drupal && Drupal.settings && Drupal.settings.dv_georedirect
      ? Drupal.settings.dv_georedirect : false;
    if (!settings) {
      return;
    }

    // Redirect if required.
    if (settings.isInternationalDomain && !$.cookie(DV_COOKIE_INTERNATIONAL_IS_ALLOWED)) {
      $.post(settings.ajaxCallbackUrl, {
        'redirect_to': settings.currentInternalPath
      }, function(url) {
        if (url && url != document.location.href) {
          document.location.href = url;
        }
      });
    }
  });

  /**
   * Set/remove cookie on the Domain dropdown change.
   */
  Drupal.behaviors.dvGeoredirect = {
    attach: function(context, settings) {
      $('#edit-domain-variant-swicher', context).once().change(function() {
        var value = $(this).val() == DV_INTERNATIONAL_DOMAIN
            ? 'true' // The value not really important.
            : null; // This will delete the cookie.
        $.cookie(DV_COOKIE_INTERNATIONAL_IS_ALLOWED, value, {path: '/'});
      });
    }
  };

})(jQuery);