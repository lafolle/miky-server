from django.shortcuts import render
from django.http import HttpResponse
from rest_framework.renderers import JSONRenderer

from serializers import SongSerializer
from models import Song

class JSONResponse(HttpResponse):
    """
    An HttpResponse that renders its content into JSON.
    """
    def __init__(self, data, **kwargs):
        content = JSONRenderer().render(data)
        kwargs['content_type'] = 'application/json'
        super(JSONResponse, self).__init__(content, **kwargs)

def list_songs(request):

	if request.method == 'GET':
		songs = Song.objects.all()
		serializer = SongSerializer(songs, many=True)
		return JSONResponse(serializer.data)
