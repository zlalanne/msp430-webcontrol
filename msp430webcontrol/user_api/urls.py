from django.conf.urls import patterns, include, url

urlpatterns = patterns('',
    url(r'^msp430s/$', 'user_api.views.msp430_list'),
)

