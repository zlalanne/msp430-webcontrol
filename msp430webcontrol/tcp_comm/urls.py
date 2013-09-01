from django.conf.urls import patterns, url

urlpatterns = patterns('',
    url(r'^register/$', 'tcp_comm.views.register'),
    url(r'^disconnect/$', 'tcp_comm.views.disconnect'),
)

