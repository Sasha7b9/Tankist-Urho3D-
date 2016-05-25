from django.db import models
from django.utils import timezone
from django.contrib.auth.models import User

class Post(models.Model):
    message = models.TextField()
    publish = models.DateTimeField(default=timezone.now)
    author = models.ForeignKey(User, related_name='forum_posts', default='')

    class Meta:
        ordering = ('-publish',)

    def __str__(self):
        return self.message

class Section(models.Model):
    title = models.TextField()

    def __str__(self):
        return self.title

class Subsectioin(models.Model):
    title = models.TextField()

    def __str__(self):
        return self.title

class Topic(models.Model):
    title = models.TextField()
    publish = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return self.title

