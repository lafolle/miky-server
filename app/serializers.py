from rest_framework import serializers
from models import Song

class SongSerializer(serializers.ModelSerializer):

	class Meta:
		model = Song
		fields = ('name', 'played_count',)	

	def restore_object(self, attrs, instance=None):

		if instance:
			return instance

		return Song(**attrs)
