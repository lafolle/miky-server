# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Client',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('ip_address', models.IPAddressField()),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='Playlist',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(default=b'playlist', max_length=256)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='Song',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(default=b'noname', max_length=200)),
                ('slug', models.SlugField(max_length=300)),
                ('filepath', models.CharField(default=b'/tmp/songs/default.mp3', max_length=512)),
                ('played_count', models.PositiveSmallIntegerField(default=0)),
                ('state', models.CharField(max_length=256, choices=[(b'P', b'playing'), (b'N', b'not_playing')])),
                ('playlist', models.ForeignKey(related_name=b'songs', to='app.Playlist', null=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
