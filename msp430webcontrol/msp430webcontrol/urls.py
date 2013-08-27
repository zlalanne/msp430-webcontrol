from django.conf.urls import patterns, include, url

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    url(r'^$', 'msp430.views.home', name='home'),
    # url(r'^msp430webcontrol/', include('msp430webcontrol.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    url(r'^user_api/', include('user_api.urls')),
    url(r'^tcp_comm/', include('tcp_comm.urls')),
    url(r'^displays/(?P<msp430_mac>.+)$', 'msp430.views.msp430_displays'),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),
)
