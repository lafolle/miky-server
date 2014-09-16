from django.db import models

class Playlist(models.Model):
	name = models.CharField(default='playlist',
							max_length=256)

class Song(models.Model):
	STATE_CHOICES = (
		('P', 'playing'),
		('N', 'not_playing'),
	)

	name = models.CharField(default='noname',
							max_length=200)
	slug = models.SlugField(max_length=300)
	filepath = models.CharField(default='/tmp/songs/default.mp3',
								max_length=512)
	played_count = models.PositiveSmallIntegerField(default=0)
	state = models.CharField(choices=STATE_CHOICES,
							max_length=256)
	playlist = models.ForeignKey('Playlist',
								related_name='songs',
								null=True)

class Client(models.Model):
	ip_address = models.IPAddressField()
