from django.conf.urls import patterns, url
from views import list_songs

urlpatterns = patterns('app.views',
	url('^api/v1/list$', list_songs),
)
